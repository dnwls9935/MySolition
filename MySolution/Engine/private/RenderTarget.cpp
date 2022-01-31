#include "..\public\RenderTarget.h"
#include "VIBuffer_RectViewPort.h"

RenderTarget::RenderTarget(ID3D11Device * _Device, ID3D11DeviceContext * _DeviceContext)
	:	m_Device(_Device)
	,	m_DeviceContext(_DeviceContext)
{
	Safe_AddRef(m_Device);
	Safe_AddRef(m_DeviceContext);
}

HRESULT RenderTarget::NativeConstruct(RTDESC RTDesc)
{
	if (nullptr == m_Device)
		return E_FAIL;

	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = RTDesc.Width;
	TextureDesc.Height = RTDesc.Height;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = RTDesc.Format;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_Device->CreateTexture2D(&TextureDesc, nullptr, &m_Texture)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	RTVDesc.Format = RTDesc.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_Device->CreateRenderTargetView(m_Texture, &RTVDesc, &m_RTV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	SRVDesc.Format = RTDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_Device->CreateShaderResourceView(m_Texture, &SRVDesc, &m_SRV)))
		return E_FAIL;

	m_ClearColor = RTDesc.Color;

	return S_OK;
}

HRESULT RenderTarget::Clear()
{
	if (nullptr == m_RTV &&
		nullptr == m_DeviceContext)
		return E_FAIL;

	m_DeviceContext->ClearRenderTargetView(m_RTV, (_float*)&m_ClearColor);

	return S_OK;
}

#ifdef _DEBUG
HRESULT RenderTarget::ReadyDebugBuffer(_float X, _float Y, _float SizeX, _float SizeY)
{
	m_DebugBuffer = CVIBuffer_RectViewPort::Create(m_Device, m_DeviceContext, CVIBuffer_RectViewPort::RVPDESC(X, Y, SizeX, SizeY), TEXT("../Bin/ShaderFiles/Shader_RectViewPort.hlsl"));
	if (nullptr == m_DebugBuffer)
		return E_FAIL;

	Clear();

	return S_OK;
}

HRESULT RenderTarget::RenderDebugBuffer()
{
	if (FAILED(m_DebugBuffer->SetUp_TextureOnShader("g_DiffuseTexture", m_SRV)))
		return E_FAIL;

	m_DebugBuffer->Render(0);

	return S_OK;
}
#endif // _DEBUG

RenderTarget * RenderTarget::Create(ID3D11Device* _Device, ID3D11DeviceContext*	_DeviceContext, RTDESC RTDesc)
{
	RenderTarget*		pInstance = new RenderTarget(_Device, _DeviceContext);
	if (FAILED(pInstance->NativeConstruct(RTDesc)))
	{
		MSGBOX("Failed to Create RenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void RenderTarget::Free()
{
#ifdef _DEBUG
	Safe_Release(m_DebugBuffer);
#endif // _DEBUG

	Safe_Release(m_RTV);
	Safe_Release(m_SRV);
	Safe_Release(m_Texture);

	Safe_Release(m_Device);
	Safe_Release(m_DeviceContext);
}
