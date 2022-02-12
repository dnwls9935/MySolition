#include "stdafx.h"
#include "..\public\Terrain.h"

#include "GameInstance.h"
#include <iostream>

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CTerrain::Tick(_double TimeDelta)
{
	if (FAILED(__super::Tick(TimeDelta)))
		return -1;

	if (nullptr == m_Player)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_Player = pGameInstance->GetPlayer(LEVEL_GAMEPLAY);
		Safe_AddRef(m_Player);
		RELEASE_INSTANCE(CGameInstance);
	}
	
	_matrix		ColliderMatrix = XMMatrixIdentity();
	ColliderMatrix = XMMatrixTranslation(41.2f, 0.f, 28.0f);
	if (nullptr != m_FirstColliderCom)
		m_FirstColliderCom->Update(ColliderMatrix);

	ColliderMatrix = XMMatrixTranslation(44.4f, 0.f, 113.7f);
	if (nullptr != m_SecondColliderCom)
		m_SecondColliderCom->Update(ColliderMatrix);

	ColliderMatrix = XMMatrixTranslation(131.1f, 0.f, 100.9f);
	if (nullptr != m_BossColliderCom)
		m_BossColliderCom->Update(ColliderMatrix);

	return _int();
}

_int CTerrain::LateTick(_double TimeDelta)
{
	if (FAILED(__super::LateTick(TimeDelta)))
		return -1;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (nullptr != m_Player)
	{
		if (nullptr != m_FirstColliderCom)
			m_FirstColliderCom->CollisionAABB(static_cast<CCollider*>(m_Player->GetComponent(TEXT("Com_AABB"))));

		if (nullptr != m_SecondColliderCom)
			m_SecondColliderCom->CollisionAABB(static_cast<CCollider*>(m_Player->GetComponent(TEXT("Com_AABB"))));

		if (nullptr != m_BossColliderCom)
			m_BossColliderCom->CollisionAABB(static_cast<CCollider*>(m_Player->GetComponent(TEXT("Com_AABB"))));
	}
		
	return _int();
}

HRESULT CTerrain::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseSourTexture", m_pTextureCom, 0);
	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseDestTexture", m_pTextureCom, 0);

	m_pVIBufferCom->SetUp_ValueOnShader("g_vCamPosition", (void*)&pGameInstance->Get_CamPosition(), sizeof(_vector));

	m_pVIBufferCom->Render(0);


#ifdef _DEBUG
	if(nullptr != m_FirstColliderCom)
		m_FirstColliderCom->Render();
	if(nullptr != m_SecondColliderCom)
		m_SecondColliderCom->Render();
	if(nullptr != m_BossColliderCom)
		m_BossColliderCom->Render();
#endif // _DEBUG



	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CTerrain::SetVerticeY(_uint Index, _float Y)
{
	m_pVIBufferCom->SetVerticeY(Index, Y);
};

HRESULT CTerrain::SetUp_Components()
{
	/* Com_Transform */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Navigation*/
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_NavigationCom)))
		return E_FAIL;


	/* Com_Collider */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(10.0f, 3.f, 0.5f);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_FirstColliderCom"), (CComponent**)&m_FirstColliderCom, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(10.0f, 3.f, 0.5f);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_SecondColliderCom"), (CComponent**)&m_SecondColliderCom, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(10.0f, 3.f, 10.0f);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_BossColliderCom"), (CComponent**)&m_BossColliderCom, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTerrain*	pInstance = new CTerrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*	pInstance = new CTerrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();
	
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_FirstColliderCom);
	Safe_Release(m_SecondColliderCom);
	Safe_Release(m_BossColliderCom);
	Safe_Release(m_Player);
}
