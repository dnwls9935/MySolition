#include "..\Public\Font_Manager.h"
#include "CustumFont.h"

IMPLEMENT_SINGLETON(CFont_Manager)


CFont_Manager::CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pFontTag, const _tchar * pFontPath)
{
	CCustomFont*		pFont = CCustomFont::Create(pDevice, pDeviceContext, pFontPath);

	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.insert(FONT::value_type(pFontTag, pFont));

	return S_OK;
}

HRESULT CFont_Manager::Render_Font(const _tchar* pFontTag, _float2 _Position, _fvector vColor, _float2 _Scale, const _tchar* pString)
{
	auto	iter = find_if(m_Fonts.begin(), m_Fonts.end(), CTag_Finder(pFontTag));

	if (iter == m_Fonts.end())
		return E_FAIL;

	return iter->second->Render_CustomFont(_Position, vColor, _Scale, pString);
}

void CFont_Manager::Free()
{
	for (auto& pFont : m_Fonts)
		Safe_Release(pFont.second);

	m_Fonts.clear();
}

