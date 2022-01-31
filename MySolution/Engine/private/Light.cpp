#include "..\public\Light.h"
#include "VIBuffer_RectViewPort.h"
#include "RenderTargetManager.h"

Light::Light(ID3D11Device * _Device, ID3D11DeviceContext * _DeviceContext)
	: m_Device(_Device)
	, m_DeviceContext(_DeviceContext)
{
	Safe_AddRef(m_Device);
	Safe_AddRef(m_DeviceContext);
}

HRESULT Light::NativeConstruct(const LIGHTDESC & _LightDesc)
{
	m_LightDesc = _LightDesc;

	D3D11_VIEWPORT		ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	_uint	NumViewPorts = 1;

	m_DeviceContext->RSGetViewports(&NumViewPorts, &ViewPortDesc);
	
	CVIBuffer_RectViewPort::RVPDESC RVPDesc;
	ZeroMemory(&RVPDesc, sizeof(CVIBuffer_RectViewPort::RVPDESC));
	RVPDesc.X = 0.f;
	RVPDesc.Y = 0.f;
	RVPDesc.SizeX = ViewPortDesc.Width;
	RVPDesc.SizeY = ViewPortDesc.Height;

	m_VIBuffer = CVIBuffer_RectViewPort::Create(m_Device, m_DeviceContext, RVPDesc, TEXT("../Bin/ShaderFiles/Shader_RectViewPort.hlsl"));
	if (nullptr == m_VIBuffer)
		return E_FAIL;


	return S_OK;
}

HRESULT Light::Render()
{
	RenderTargetManager*		RTMgr = GET_INSTANCE(RenderTargetManager);

	m_VIBuffer->SetUp_TextureOnShader("g_NormalTexture", RTMgr->Get_SRV(TEXT("Target_Normal")));
	m_VIBuffer->SetUp_ValueOnShader("g_vLightDir", &_float4(m_LightDesc.vDirection.x, m_LightDesc.vDirection.y, m_LightDesc.vDirection.z, 0.f), sizeof(_float4));

	m_VIBuffer->Render(1);

	RELEASE_INSTANCE(RenderTargetManager);
	return E_NOTIMPL;
}

Light * Light::Create(ID3D11Device * _Device, ID3D11DeviceContext * _DeviceContext, const LIGHTDESC& _LightDesc)
{
	Light*	pInstance = new Light(_Device, _DeviceContext);
	if (FAILED(pInstance->NativeConstruct(_LightDesc)))
	{
		MSGBOX("Failed to Create Light");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Light::Free()
{
	Safe_Release(m_VIBuffer);
	Safe_Release(m_Device);
	Safe_Release(m_DeviceContext);
}
