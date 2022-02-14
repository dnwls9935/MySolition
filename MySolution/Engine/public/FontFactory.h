#pragma once

#include "Base.h"

BEGIN(Engine)

/* FreeType ���̺귯���� Glyph�� ���� �ؽ��� ����  */
class FontFactory final : public CBase
{
	DECLARE_SINGLETON(FontFactory)
	/* �۸��� ������ */
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
	UVWH : �ؽ��� ���� UV��ǥ
	Left, Top : ����/���� ����
	BmW, BmH : �۸��� ���� ���� �ȼ� ����
	advX, advY : �� �۸��� ���� �� ���� ���ڸ� ��� ���� �������� �ϴ� X,Y����
	Height : ������ ����
	Use : �ؽ��� ��¿� ���Ǿ����� ����(GC�Լ��� ���� ����)
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
	FT_Library						m_FTL; // FreeType ���̺귯��
	ID3D11Resource*							m_Tex; // DX9 ���� Direct3DTexture9
	ID3D11ShaderResourceView*	m_TexSRV;
	//D3DLOCKED_RECT ???? �̰� ����..

	_int													m_Width; /* �ؽ�ó ���� ������ / ���δ� ���ο� ���� */
	vector<_byte>								m_Map; /* �ؽ����� Ư�� ������ ����������� �����صδ� �ؽ����� �����̳� */
	map<GLYPH, GLYPHDATA>			m_GlyphPos; /* ĳ�̵� �����͸� ���� */

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