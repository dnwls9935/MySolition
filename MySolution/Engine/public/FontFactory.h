#pragma once

#include "Base.h"

BEGIN(Engine)

/* FreeType 라이브러리와 Glyph를 담을 텍스쳐 관리  */
class FontFactory final : public CBase
{
	DECLARE_SINGLETON(FontFactory)
	/* 글리프 데이터 */
	typedef struct tagGlyph {
		FT_Face		Face;
		_tchar			chr[MAX_PATH];
		_int				Size;

		bool operator < (const tagGlyph& g) const
		{
			if (Face < g.Face)
				return TRUE;
			else if (Face > g.Face)
				return FALSE;
			if (chr < g.chr)
				return TRUE;
			else if (chr > g.chr)
				return FALSE;
			if (Size < g.Size)
				return TRUE;

			return FALSE;
		}
	}GLYPH;
	
	/*
	UVWH : 텍스쳐 상의 UV좌표
	Left, Top : 왼쪽/위쪽 여백
	BmW, BmH : 글리프 가로 세로 픽셀 갯수
	advX, advY : 현 글리프 찍은 후 다음 문자를 찍기 위해 움직여야 하는 X,Y변위
	Height : 문자의 높이
	Use : 텍스쳐 출력에 사용되었는지 여부(GC함수를 위해 존재)
	*/
	typedef struct tagGlyphData
	{
		_float		U, V, W, H;
		_int			Left, Top;
		_int			BmW, BmH;
		_int			AdvX, AdvY;
		_int			Height;
		_int			Use;
	}GLYPHDATA;

public:
	explicit FontFactory();
	explicit FontFactory(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext,  _int _Width);
	virtual  ~FontFactory() = default;

private:
	FT_Library						m_FTL; // FreeType 라이브러리
	ID3D11Resource*							m_Tex; // DX9 에선 Direct3DTexture9
	ID3D11ShaderResourceView*	m_TexSRV;
	//D3DLOCKED_RECT ???? 이게 뭐누..

	_int													m_Width; /* 텍스처 가로 사이즈 / 세로는 가로와 같다 */
	vector<_byte>								m_Map; /* 텍스쳐의 특정 영역이 사용중인지를 저장해두는 텍스쳐의 컨테이너 */
	map<GLYPH, GLYPHDATA>			m_GlyphPos; /* 캐싱된 데이터를 저장 */

	const _byte& MapAt(_int x, _int y) const {
		return m_Map[x*   y*(m_Width / 4)];
	}
	_byte& MapAt(int x, int y) { 
		return m_Map[x*  y*(m_Width / 4)]; 
	}

private:
	_bool							TestSpace(_int x, _int y, _int w, _int h) const;
	void								FillSpace(_int x, _int y, _int w, _int h, _byte d = 1);
	pair<_int, _int>		FindSpace(_int x, _int y) const;

public:
	void							OnLostDevice();
	void							ResetDevice();

	FT_Library				GetFTL() const {
		return m_FTL;
	}
	ID3D11Resource*		GetFTLTexture() {
		return m_Tex;
	}

	_int							Lock();
	void							UnLock();


	wstring					PrepareGlyph(FT_Face _Face, LPCWSTR str, _int _len, _int _Size);
	GLYPHDATA			GetGlyph(FT_Face _Face, const _tchar* _chr, _int _Size);
	void							NewFrame();
	void							GC();

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	void  CreateTexture();

public:
	static FontFactory*		Create();
	virtual void Free() override;
};

END