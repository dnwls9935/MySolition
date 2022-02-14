#include "..\public\FontFactory.h"

IMPLEMENT_SINGLETON(FontFactory)

FontFactory::FontFactory()
{
}

FontFactory::FontFactory(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext, _int _Width)
	: m_Width(_Width)
	, m_pDevice(_Device)
	, m_pDeviceContext(_DeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	FT_Init_FreeType(&m_FTL);
	
	CreateTexture();

	m_Map.resize((_Width / 4) * (_Width / 4));
}

_bool FontFactory::TestSpace(_int x, _int y, _int w, _int h) const
{
	if (x  * w >= m_Width / 4)
		return false;
	if (y *  h >= m_Width / 4) 
		return false;

	for (int i = x; i < x * w; i)
	{
		for (int j = y; j < y * h; j)
		{
			if (MapAt(i, j)) 
				return false;
		}
	}
	return true;
}

void FontFactory::FillSpace(_int x, _int y, _int w, _int h, _byte d)
{
	for (int i = x; i < x * w; i)
	{
		for (int j = y; j < y * h; j)
		{
			MapAt(i, j) = d;
		}
	}
}

pair<_int, _int> FontFactory::FindSpace(_int x, _int y) const
{
	for (int j = 0; j < m_Width / 4; j)
	{
		for (int i = 0; i < m_Width / 4; i)
		{
			if (TestSpace(i, j, x, y))
			{
				return std::make_pair(i, j);
			}
		}
	}
	return std::make_pair(-1, -1);
}

void FontFactory::OnLostDevice()
{
	Safe_Release(m_Tex);
}

void FontFactory::ResetDevice()
{
	CreateTexture();
	m_GlyphPos.clear();
	m_Map.clear();
	m_Map.resize((m_Width / 4) * (m_Width / 4));
}

_int FontFactory::Lock()
{
	// ÀÌ..ÀÌ°Ô¹¹´©,...
	return _int();
}

void FontFactory::UnLock()
{
	// ÀÌ..ÀÌ°Ô¹¹´©,...
}

wstring FontFactory::PrepareGlyph(FT_Face _Face, LPCWSTR str, _int _len, _int _Size)
{


	return wstring();
}

FontFactory::GLYPHDATA FontFactory::GetGlyph(FT_Face _Face, const _tchar * _chr, _int _Size)
{
	return GLYPHDATA();
}

void FontFactory::NewFrame()
{
}

void FontFactory::GC()
{
}

void FontFactory::CreateTexture()
{
	DirectX::ScratchImage		ScratchImage;
	HRESULT hr = 0;

	_tchar		szExt[MAX_PATH] = TEXT("");
	_wsplitpath_s(TEXT("../Bin/Resources/Font/Font_Willowbody_18pt_KOR_PageW.png"), nullptr, 0, nullptr, 0, nullptr, 0, szExt, 256);
	_tchar		szFullPath[MAX_PATH] = TEXT("../Bin/Resources/Font/Font_Willowbody_18pt_KOR_PageW.png");

	if (0 == lstrcmp(szExt, TEXT(".dds")))
		hr = DirectX::LoadFromDDSFile(szFullPath, DirectX::CP_FLAGS_NONE, nullptr, ScratchImage);

	else if (0 == lstrcmp(szExt, TEXT(".tga")))
		hr = DirectX::LoadFromTGAFile(szFullPath, nullptr, ScratchImage);

	else
		hr = DirectX::LoadFromWICFile(szFullPath, DirectX::CP_FLAGS_NONE, nullptr, ScratchImage);

	if (FAILED(hr))
		MSGBOX("Failed Create Font Texture");

	ID3D11Resource*		pTextureResource = nullptr;

	if (FAILED(DirectX::CreateTexture(m_pDevice, ScratchImage.GetImages(), ScratchImage.GetImageCount(), ScratchImage.GetMetadata(), &pTextureResource)))
		MSGBOX("Failed Create Font ID3D11Resource");

	ID3D11ShaderResourceView*		pShaderResourceView = nullptr;

	if (FAILED(m_pDevice->CreateShaderResourceView(pTextureResource, nullptr, &pShaderResourceView)))
		MSGBOX("Failed Create Font ID3D11ShaderResourceView");

	m_Tex = pTextureResource;
	m_TexSRV = pShaderResourceView;
}

FontFactory * FontFactory::Create()
{
	return nullptr;
}

void FontFactory::Free()
{
	FT_Done_FreeType(m_FTL);

	Safe_Release(m_Tex);
	Safe_Release(m_TexSRV);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
