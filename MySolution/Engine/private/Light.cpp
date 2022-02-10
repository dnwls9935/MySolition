#include "..\public\Light.h"
#include "VIBuffer_RectViewPort.h"
#include "TargetManager.h"
#include "GameInstance.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CLight::NativeConstruct(const LIGHTDESC& LightDesc)
{
	m_LightDesc = LightDesc;

	D3D11_VIEWPORT		ViewportDesc;
	_uint		iNumViewports = 1;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_pVIBuffer = CVIBuffer_RectViewPort::Create(m_pDevice, m_pDeviceContext, 0.f, 0.f, ViewportDesc.Width, ViewportDesc.Height, TEXT("../Bin/ShaderFiles/Shader_RectViewPort.hlsl"));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	return S_OK;
}

HRESULT CLight::Render()
{
	if (FALSE == m_Lighting)
		return S_OK;

	CTarget_Manager*		pTarget_Manager = GET_INSTANCE(CTarget_Manager);
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	_uint		iPassIndex = 0;

	if (m_LightDesc.eType == tagLightDesc::TYPE_DIRECTIONAL)
	{
		iPassIndex = 1;
		if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_vLightDir", &_float4(m_LightDesc.vDirection.x, m_LightDesc.vDirection.y, m_LightDesc.vDirection.z, 0.f), sizeof(_float4))))
			return E_FAIL;
	}
	else if (m_LightDesc.eType == tagLightDesc::TYPE_POINT)
	{
		iPassIndex = 2;
		if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_vLightPos", &_float4(m_LightDesc.vPosition.x, m_LightDesc.vPosition.y, m_LightDesc.vPosition.z, 1.f), sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_fRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;
	}
	
	if (FAILED(m_pVIBuffer->SetUp_TextureOnShader("g_NormalTexture", pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_TextureOnShader("g_DepthTexture", pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	_vector		vCamPosition = pGameInstance->Get_CamPosition();

	_matrix		ViewMatrix = pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	_matrix		ProjMatrix = pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION);
	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix);

	if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_vCamPosition", &vCamPosition, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_ViewMatrixInv", &XMMatrixTranspose(ViewMatrix), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_ValueOnShader("g_ProjMatrixInv", &XMMatrixTranspose(ProjMatrix), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render(iPassIndex)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CTarget_Manager);


	return S_OK;
}

_int CLight::Update(_vector _Position)
{
	XMStoreFloat3(&m_LightDesc.vPosition, _Position);

	return _int();
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const LIGHTDESC& LightDesc)
{
	CLight*	pInstance = new CLight(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(LightDesc)))
	{
		MSGBOX("Failed to Creating CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
