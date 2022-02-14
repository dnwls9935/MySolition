#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
public:
	explicit CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCustomFont() = default;
public:
	HRESULT Ready_CustomFont(const _tchar* pFontPath);
	HRESULT Render_CustomFont(_float2 _Position, _fvector vColor, _float2 _Scale, const _tchar* pString);
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
private:
	DirectX::SpriteBatch*	m_pSpriteBatch = nullptr;
	DirectX::SpriteFont*	m_pSpriteFont = nullptr;
public:
	static CCustomFont* Create(ID3D11Device* pGraphic_Device, ID3D11DeviceContext* pGraphic_Device_Context, const _tchar* pFontPath);
	virtual void Free();
};

END