#include "stdafx.h"
#include "..\public\Muzzle.h"
#include "GameInstance.h"

Muzzle::Muzzle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

Muzzle::Muzzle(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT Muzzle::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Muzzle::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	_vector Position = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	memcpy(&Position, pArg, sizeof(_vector));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Position);

	m_pTransformCom->Set_Scale(XMVectorSet(2.5f, 1.5f, 1.5f, 1.f));
	
	return S_OK;
}

_int Muzzle::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_matrix ViewMat = pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	_matrix ViewMatInv = XMMatrixInverse(nullptr, ViewMat);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatInv.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatInv.r[2]);

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int Muzzle::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);

	return m_Dead;
}

HRESULT Muzzle::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_VIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	_int numRand = rand() % 4;
	m_VIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_TextureCom[0], numRand);
	numRand = rand() % 4;
	m_VIBufferCom->SetUp_TextureOnShader("g_CompTexture", m_TextureCom[1], numRand);


	m_VIBufferCom->Render(1);

	m_Dead = TRUE;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Muzzle::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Muzzle"), TEXT("Com_Texture"), (CComponent**)&m_TextureCom[0])))
		return E_FAIL;
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MuzzleComp"), TEXT("Com_Texture2"), (CComponent**)&m_TextureCom[1])))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Effect_Rect"), TEXT("Com_Buffer"), (CComponent**)&m_VIBufferCom)))
		return E_FAIL;

	return S_OK;
}

Muzzle * Muzzle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Muzzle*	pInstance = new Muzzle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating HitBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * Muzzle::Clone(void * pArg)
{
	Muzzle*	pInstance = new Muzzle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning HitBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Muzzle::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_VIBufferCom);
	Safe_Release(m_pRendererCom);

	for (auto& pTexture : m_TextureCom)
		Safe_Release(pTexture);

}
