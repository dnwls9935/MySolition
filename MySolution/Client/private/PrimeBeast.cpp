#include "stdafx.h"
#include "..\public\PrimeBeast.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player.h"
#include "SMG.h"
#include "PrimeBeastRock.h"

#include <iostream>

PrimeBeast::PrimeBeast(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

PrimeBeast::PrimeBeast(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT PrimeBeast::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT PrimeBeast::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameObject::TOOLOBJDESC		ToolObjDesc;
	ZeroMemory(&ToolObjDesc, sizeof(CGameObject::TOOLOBJDESC));
	memcpy(&ToolObjDesc, (CGameObject::TOOLOBJDESC*)pArg, sizeof(CGameObject::TOOLOBJDESC));
	
	_matrix TransformMatrix = XMLoadFloat4x4(&ToolObjDesc.m_pTransformMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, TransformMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, TransformMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, TransformMatrix.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);

	m_HP = 3000;
	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::SPAWN_WALLDROPJUMP);

	m_rHand1Bone = m_pModelCom->Get_BoneMatrix("rHand1");
	m_rHand2Bone = m_pModelCom->Get_BoneMatrix("rHand2");
	m_lHand1Bone = m_pModelCom->Get_BoneMatrix("lHand1");
	m_lHand2Bone = m_pModelCom->Get_BoneMatrix("lHand2");

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*> TargetObjectList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	auto& iter = TargetObjectList.begin();
	advance(iter, 0);
	m_TargetPlayer = *iter;
	advance(iter, 1);
	m_TargetPlayerWeapon = *iter;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int PrimeBeast::Tick(_double TimeDelta)
{
	GetTargetDistance();
	Animation(TimeDelta);
	
	if(TRUE == m_FrameStart)
		m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	else
		m_pModelCom->Update_CombinedTransformationMatrix(0.0);
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	HitCheck();

	_matrix BoneMatrix = XMMatrixIdentity();
	_matrix Transform = XMMatrixIdentity();
	_matrix OffsetMatrix = XMMatrixIdentity();
	_matrix Combined = XMMatrixIdentity();
	_matrix PivotMatrix = XMMatrixIdentity();
	_matrix WorldMatrix = XMMatrixIdentity();

	Transform = XMMatrixIdentity();
	OffsetMatrix = m_rHand1Bone->Get_OffsetMatrix();
	Combined = m_rHand1Bone->Get_CombinedMatrix();
	PivotMatrix = m_pModelCom->Get_PivotMatrix();
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * WorldMatrix;
	m_ColliderSphere1->Update(BoneMatrix);
	
	Combined = m_rHand2Bone->Get_CombinedMatrix();
	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * WorldMatrix;
	m_ColliderSphere2->Update(BoneMatrix);

	Combined = m_lHand1Bone->Get_CombinedMatrix();
	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * WorldMatrix;
	m_ColliderSphere3->Update(BoneMatrix);

	Combined = m_lHand2Bone->Get_CombinedMatrix();
	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * WorldMatrix;
	m_ColliderSphere4->Update(BoneMatrix);



	return _int();
}

_int PrimeBeast::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (m_pModelCom->GetAnimationFinished())
	{
		if((_uint)ANIMATION_STATE::SPAWN_WALLDROPJUMP == m_pModelCom->GetCurrentAnimation())
			m_IntroEnd = TRUE;
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F_V1);
	}

	if (0 >= m_HP)
	{
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::DEATH_CRITICAL);
		if (m_pModelCom->GetAnimationFinished())
		{
			m_Dead = TRUE;
			return m_Dead;
		}
	}

	return _int();
}

HRESULT PrimeBeast::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_pModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshContainer(); ++i)
	{
		m_pModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Render(i, 1);
	}

#ifdef _DEBUG
	m_ColliderCom->Render();
	m_ColliderSphere1->Render();
	m_ColliderSphere2->Render();
	m_ColliderSphere3->Render();
	m_ColliderSphere4->Render();
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_bool PrimeBeast::ThrowMotionFrame()
{
	if ((_uint)ANIMATION_STATE::ATTACK_THROWROCK_V1 == m_pModelCom->GetCurrentAnimation() &&
		38 == m_pModelCom->GetCurrentAnimationFrame())
		return TRUE;

	return FALSE;
}

_matrix PrimeBeast::SetRockPosition()
{
	_matrix		Transform = XMMatrixIdentity();
	_matrix		OffsetMatrix = m_rHand2Bone->Get_OffsetMatrix();
	_matrix		CombinedMatrix = m_rHand2Bone->Get_CombinedMatrix();
	_matrix		PivotMatrix = m_pModelCom->Get_PivotMatrix();
	_matrix		WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	return Transform * OffsetMatrix * CombinedMatrix * PivotMatrix * WorldMatrix;
}

void PrimeBeast::GetTargetDistance()
{
	m_PlayerPosition = static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_MyPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector Distance = m_PlayerPosition - m_MyPosition;

	m_TargetDistance = XMVectorGetX(XMVector3Length(Distance));
}

void PrimeBeast::HitCheck()
{
	if (TRUE == static_cast<SMG*>(m_TargetPlayerWeapon)->GetFireFrame())
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		Calculator::CALCDESC CalDesc;
		CalDesc._width = g_iWinCX;
		CalDesc._height = g_iWinCY;
		CalDesc._transformCom = m_pTransformCom;
		CalDesc._hWnd = g_hWnd;

		pGameInstance->CalcMousePos(&CalDesc);

		CalDesc._rayPos = XMVector3TransformCoord(CalDesc._rayPos, m_pTransformCom->Get_WorldMatrix());
		CalDesc._rayDir = XMVector3TransformNormal(CalDesc._rayDir, m_pTransformCom->Get_WorldMatrix());
		CalDesc._rayDir =  XMVector3Normalize(CalDesc._rayDir);
		
		if (TRUE == m_ColliderCom->CollisionAABBToRay(CalDesc._rayPos, CalDesc._rayDir))
		{
			m_HP--;
		}
		
		RELEASE_INSTANCE(CGameInstance);
	}
}

void PrimeBeast::Animation(_double TimeDelta)
{
	if (15 >= m_TargetDistance && !m_FrameStart)
	{
		m_FrameStart = TRUE;
	}else if(m_FrameStart && m_IntroEnd)
	{
		if (5 >= m_TargetDistance)
			Attack();
		else
			Moving(TimeDelta);
	}
}

void PrimeBeast::Moving(_double TimeDelta)
{
	m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::RUN_F_V1);
	CTransform* PlayerTransformCom =	static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform")));
	m_pTransformCom->Chase_Target(PlayerTransformCom, TimeDelta);
}

void PrimeBeast::Attack()
{
	m_pModelCom->SetUp_AnimationIndex( (_uint)ANIMATION_STATE::ATTACK_THROWROCK_V1 );

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (0 == m_pModelCom->GetCurrentAnimationFrame()) {
		PrimeBeastRock::PRBDESC PrbDesc;
		ZeroMemory(&PrbDesc, sizeof(PrimeBeastRock::PRBDESC));
		PrbDesc.m_PrimeBeast = this;
		XMStoreFloat3(&PrbDesc.m_TargetPosition ,(m_PlayerPosition - m_MyPosition));
		PrbDesc.m_TypeRock = PrimeBeastRock::TYPE_ROCK::BIG;
		PrbDesc.m_TypeThrow= PrimeBeastRock::TYPE_THROW::DIRECT;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), TEXT("Prototype_GameObject_Model_PrimeBeastRock"), &PrbDesc)))
			return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT PrimeBeast::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 6.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PrimeBeast"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Model */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(0.5f, 1.3f, 0.5f);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_ColliderCom, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.0f, 1.0f, 1.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Sphere1"), (CComponent**)&m_ColliderSphere1, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.0f, 1.0f, 1.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Sphere2"), (CComponent**)&m_ColliderSphere2, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.0f, 1.0f, 1.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Sphere3"), (CComponent**)&m_ColliderSphere3, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.0f, 1.0f, 1.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Sphere4"), (CComponent**)&m_ColliderSphere4, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

PrimeBeast * PrimeBeast::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	PrimeBeast*	pInstance = new PrimeBeast(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * PrimeBeast::Clone(void * pArg)
{
	PrimeBeast*	pInstance = new PrimeBeast(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void PrimeBeast::Free()
{
	__super::Free();

	Safe_Release(m_ColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
