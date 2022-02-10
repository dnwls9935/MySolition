#include "stdafx.h"
#include "..\public\HitBullet.h"
#include "GameInstance.h"

HitBullet::HitBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

HitBullet::HitBullet(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT HitBullet::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT HitBullet::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	_vector Position = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	memcpy(&Position, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Position);
	
	return S_OK;
}

_int HitBullet::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix ViewMat = pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	_matrix ViewMatInv = XMMatrixInverse(nullptr, ViewMat);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatInv.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatInv.r[2]);


	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int HitBullet::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT HitBullet::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_VIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_VIBufferCom->SetUp_TextureOnShader("g_vDiffuseTexture", m_TextureCom, 0);


	m_VIBufferCom->Render(6);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT HitBullet::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HitBullet"), TEXT("Com_Texture"), (CComponent**)&m_TextureCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_Buffer"), (CComponent**)&m_VIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HitBullet * HitBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	HitBullet*	pInstance = new HitBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating HitBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * HitBullet::Clone(void * pArg)
{
	HitBullet*	pInstance = new HitBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning HitBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void HitBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_VIBufferCom);
	Safe_Release(m_pRendererCom);
}
