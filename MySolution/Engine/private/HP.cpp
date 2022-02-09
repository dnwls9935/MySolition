#include "..\public\HP.h"

#include "GameInstance.h"
#include "VIBuffer_Rect.h"

HP::HP(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

HP::HP(const HP & rhs)
	: CComponent(rhs)
{
}

HRESULT HP::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT HP::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int HP::Update(_vector Position, _float _Barpercent)
{
	CPipeLine*		PipeLine = GET_INSTANCE(CPipeLine);
	_matrix ViewMat = PipeLine->Get_Transform(CPipeLine::D3DTS_VIEW);
	_matrix ViewMatInv = XMMatrixInverse(nullptr, ViewMat);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatInv.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatInv.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Position);

	m_pTransformCom->Set_Scale(XMVectorSet(1.5f, 0.5f, 1.f, 1.f));

	m_Barpercent = _Barpercent;

	RELEASE_INSTANCE(CPipeLine);
	return _int();
}

HRESULT HP::Render()
{
	CPipeLine*		PipeLine = GET_INSTANCE(CPipeLine);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(PipeLine->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(PipeLine->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pVIBufferCom->SetUp_ValueOnShader("g_BarPercent", &m_Barpercent, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_vCamPosition", (void*)&PipeLine->Get_CamPosition(), sizeof(_vector));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(5);


	RELEASE_INSTANCE(CPipeLine);
	return S_OK;
}

HRESULT HP::SetUp_Components()
{
	m_pVIBufferCom = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Rect.hlsl"));
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTextureCom = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/UI_HUD/HUD_I2CD.png"));
	if (nullptr == m_pTextureCom)
		return E_FAIL;

	m_pTransformCom = CTransform::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pTransformCom)
		return E_FAIL;

	return S_OK;
}

HP * HP::Create(ID3D11Device * _Device, ID3D11DeviceContext * _DeviceContext)
{
	HP*		pInstance = new HP(_Device, _DeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

HP * HP::Clone(void * pArg)
{
	HP*		pInstance = new HP(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning HP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void HP::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
