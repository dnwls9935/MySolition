#pragma once

#include "Base.h"

BEGIN(Engine)

class RenderTarget final : public CBase
{
public:
	typedef struct tagRenderTargetDesc
	{
		_uint					Width = 0;
		_uint					Height = 0;
		DXGI_FORMAT Format = DXGI_FORMAT_UNKNOWN;
		_float4				Color = _float4();
	}RTDESC;

private:
	explicit RenderTarget(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext);
	virtual ~RenderTarget() = default;

public:
	ID3D11RenderTargetView*		Get_RTV() {
		return m_RTV;
	}
	ID3D11ShaderResourceView*		Get_SRV() {
		return m_SRV;
	}

	HRESULT		Clear();

private:
	HRESULT		NativeConstruct(RTDESC RTDesc);
	
#ifdef _DEBUG
public:
	HRESULT		ReadyDebugBuffer(_float X, _float Y, _float SizeX, _float SizeY);
	HRESULT		RenderDebugBuffer();
#endif // DEBUG

private:
	ID3D11Device*						m_Device = nullptr;
	ID3D11DeviceContext*		m_DeviceContext = nullptr;

private:
	ID3D11Texture2D*						m_Texture = nullptr;
	ID3D11RenderTargetView*		m_RTV = nullptr;
	ID3D11ShaderResourceView*	m_SRV = nullptr;
	_float4											m_ClearColor;

#ifdef _DEBUG
private:
	class CVIBuffer_RectViewPort*				m_DebugBuffer = nullptr;
#endif // _DEBUG


public:
	static RenderTarget*	Create(ID3D11Device* _Device, ID3D11DeviceContext*	_DeviceContext, RTDESC RTDesc);
	virtual void Free() override;
};

END