#include "stdafx.h"
#include "..\public\PickUps.h"

#include "GameInstance.h"
#include "DahlEpicCrate.h"
#include "DahlWeaponLocker.h"
#include "HyperionChest.h"
#include "Player.h"

PickUps::PickUps(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

PickUps::PickUps(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT PickUps::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT PickUps::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_PUDesc, pArg, sizeof(PickUps::PUDESC));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_PUDesc.m_WorldMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, m_PUDesc.m_WorldMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_PUDesc.m_WorldMatrix.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_PUDesc.m_WorldMatrix.r[3]);

	return S_OK;
}


_int PickUps::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* Player = static_cast<CGameObject*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player")).front());

	_matrix BoneMatrix = XMMatrixIdentity();
	if (PickUps::TYPE_ID::SMG_AMMO == m_PUDesc.m_TypeID) {
		m_Chase = m_IsShow = static_cast<DahlWeaponLocker*>(m_PUDesc.m_Parent)->GetOpend();

		if (FALSE == m_Chase) {
			BoneMatrix = static_cast<DahlWeaponLocker*>(m_PUDesc.m_Parent)->GetBoneMatrix();
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, BoneMatrix.r[3]);
			m_IsShow = static_cast<DahlWeaponLocker*>(m_PUDesc.m_Parent)->GetIteShow();
			m_ColliderSphere->Update(BoneMatrix);
		}
		else {
			m_pTransformCom->Chase_Target(static_cast<CTransform*>(Player->GetComponent(TEXT("Com_Transform"))), TimeDelta);
			m_ColliderSphere->Update(m_pTransformCom->Get_WorldMatrix());
		}
	}else if (PickUps::TYPE_ID::SHOTGUN_AMMO == m_PUDesc.m_TypeID) {
/*
		BoneMatrix = static_cast<DahlEpicCrate*>(m_PUDesc.m_Parent)->GetBoneMatrix();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, BoneMatrix.r[3]);*/
	}
	else if (PickUps::TYPE_ID::BOOSTER_SHILED == m_PUDesc.m_TypeID) {
		m_Chase = m_IsShow = static_cast<DahlEpicCrate*>(m_PUDesc.m_Parent)->GetOpend();

		if (FALSE == m_Chase) {
			BoneMatrix = static_cast<DahlEpicCrate*>(m_PUDesc.m_Parent)->GetBoneMatrix();
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, BoneMatrix.r[3]);
			m_IsShow = static_cast<DahlEpicCrate*>(m_PUDesc.m_Parent)->GetIteShow();
			m_ColliderSphere->Update(BoneMatrix);
		}
		else {
			m_pTransformCom->Chase_Target(static_cast<CTransform*>(Player->GetComponent(TEXT("Com_Transform"))), TimeDelta);
			m_ColliderSphere->Update(m_pTransformCom->Get_WorldMatrix());
		}
	}
	else if (PickUps::TYPE_ID::HEALTH_VIRAL == m_PUDesc.m_TypeID) {
		m_Chase = m_IsShow = static_cast<HyperionChest*>(m_PUDesc.m_Parent)->GetOpend();

		if (FALSE == m_Chase) {
			BoneMatrix = static_cast<HyperionChest*>(m_PUDesc.m_Parent)->GetBoneMatrix();
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, BoneMatrix.r[3]);
			m_IsShow = static_cast<HyperionChest*>(m_PUDesc.m_Parent)->GetIteShow();
			m_ColliderSphere->Update(BoneMatrix);
		}
		else {
			m_pTransformCom->Chase_Target(static_cast<CTransform*>(Player->GetComponent(TEXT("Com_Transform"))), TimeDelta);
			m_ColliderSphere->Update(m_pTransformCom->Get_WorldMatrix());
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int PickUps::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderSphere);
#endif // _DEBUG
	}


	if (FALSE == m_Chase)
		return _int();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* Player = static_cast<CGameObject*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player")).front());
	if (TRUE == m_ColliderSphere->CollisionSphereToAABB(static_cast<CCollider*>(Player->GetComponent(TEXT("Com_AABB")))))
	{
		static_cast<CPlayer*>(Player)->PickUp(m_PUDesc.m_TypeID);
		m_Dead = TRUE;
	}

	RELEASE_INSTANCE(CGameInstance);
	return m_Dead;
}

HRESULT PickUps::Render()
{
	if (FALSE == m_IsShow)
		return S_OK;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_ModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_ModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_ModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_ModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_ModelCom->Get_NumMeshContainer(); ++i)
	{
		m_ModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_ModelCom->Render(i, 0);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT PickUps::SetUp_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 3.5f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	/* Com_Transform */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Collider */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(0.5f, 0.5f, 0.5f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Sphere"), (CComponent**)&m_ColliderSphere, &CollisionDesc)))
		return E_FAIL;

	switch (m_PUDesc.m_TypeID)
	{
	case PickUps::TYPE_ID::BOOSTER_SHILED:
		/* Com_Model */
		if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BoosterSheld"), TEXT("Com_Model"), (CComponent**)&m_ModelCom)))
			return E_FAIL;

		///* Com_Texture */
		//if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT(""), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		//	return E_FAIL;
		break;
	case PickUps::TYPE_ID::HEALTH_VIRAL:
		/* Com_Model */
		if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HealthViral"), TEXT("Com_Model"), (CComponent**)&m_ModelCom)))
			return E_FAIL;

		///* Com_Texture */
		//if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT(""), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		//	return E_FAIL;
		break;
	case PickUps::TYPE_ID::SHOTGUN_AMMO:
		/* Com_Model */
		if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ShotgunAmmoPickUp"), TEXT("Com_Model"), (CComponent**)&m_ModelCom)))
			return E_FAIL;

		///* Com_Texture */
		//if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT(""), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		//	return E_FAIL;
		break;
	case PickUps::TYPE_ID::SMG_AMMO:
		if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SMGAmmoPickUp"), TEXT("Com_Model"), (CComponent**)&m_ModelCom)))
			return E_FAIL;

		///* Com_Texture */
		//if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT(""), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		//	return E_FAIL;
		break;
	}

	return S_OK;
}

PickUps * PickUps::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	PickUps*	pInstance = new PickUps(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating PickUps");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * PickUps::Clone(void * pArg)
{
	PickUps*	pInstance = new PickUps(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating PickUps");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void PickUps::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_ModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_ColliderSphere);
}
