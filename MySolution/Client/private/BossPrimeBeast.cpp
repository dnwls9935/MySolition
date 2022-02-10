#include "stdafx.h"
#include "..\public\BossPrimeBeast.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player.h"
#include "SMG.h"
#include "BossPrimeBeastRock.h"
#include "HitBullet.h"

BossPrimeBeast::BossPrimeBeast(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

BossPrimeBeast::BossPrimeBeast(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT BossPrimeBeast::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT BossPrimeBeast::NativeConstruct(void * pArg)
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

	m_MaxHP = 10000;
	m_HP = m_MaxHP;

	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::SPAWN_CLIMBOVER);

	m_HeadBone = m_pModelCom->Get_BoneMatrix("Head");
	Safe_AddRef(m_HeadBone);
	m_rHand1Bone = m_pModelCom->Get_BoneMatrix("rHand1");
	Safe_AddRef(m_rHand1Bone);
	m_rHand2Bone = m_pModelCom->Get_BoneMatrix("rHand2");
	Safe_AddRef(m_rHand2Bone);
	m_lHand1Bone = m_pModelCom->Get_BoneMatrix("lHand1");
	Safe_AddRef(m_lHand1Bone);
	m_lHand2Bone = m_pModelCom->Get_BoneMatrix("lHand2");
	Safe_AddRef(m_lHand2Bone);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*> TargetObjectList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	auto& iter = TargetObjectList.begin();
	advance(iter, 0);
	m_TargetPlayer = *iter;
	advance(iter, 1);
	m_TargetPlayerWeapon = *iter;

	m_Terrain = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Terrain")).front();
	RELEASE_INSTANCE(CGameInstance);

	//
	m_NavigationCom->SettingDefaultIndex(m_pTransformCom);

	return S_OK;
}

_int BossPrimeBeast::Tick(_double TimeDelta)
{
	if(TRUE == m_FrameStart && 
		(_uint)ANIMATION_STATE::RUN_F_V1)
		m_ChargeTime += TimeDelta * 3.f;

	GetDistance();
	Animation(TimeDelta);
	BoneColliderTick(TimeDelta);

	if (TRUE == m_FrameStart)
		m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	else
		m_pModelCom->Update_CombinedTransformationMatrix(0.0);
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if ((_uint)ANIMATION_STATE::ATT_TR_V1 == m_pModelCom->GetCurrentAnimation() ||
		(_uint)ANIMATION_STATE::ATT_P_V1 == m_pModelCom->GetCurrentAnimation() ||
		(_uint)ANIMATION_STATE::ATT_P_V2 == m_pModelCom->GetCurrentAnimation() ||
		(_uint)ANIMATION_STATE::ATT_P_V3 == m_pModelCom->GetCurrentAnimation() ||
		(_uint)ANIMATION_STATE::ATT_P_V4 == m_pModelCom->GetCurrentAnimation() ||
		(_uint)ANIMATION_STATE::ATT_P_V5 == m_pModelCom->GetCurrentAnimation())
	{
		_vector Look = m_PlayerPosition - m_MyPosition;
		Look = XMVector3Normalize(Look);
		_vector Right = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f) ,Look );
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Right);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, Look);
	}

	if (TRUE == m_IntroEnd)
		HitCheck();

	AttackBlockCreate(m_pModelCom->GetCurrentAnimation());
	AttackCollision(TimeDelta);

	return _int();
}

_int BossPrimeBeast::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

#ifdef _DEBUG
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderCom);
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_HeadCollider);
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_rHand1Collider);
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_rHand2Collider);
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_lHand1Collider);
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_lHand2Collider);
#endif // _DEBUG
	}

	
	if (m_pModelCom->GetAnimationFinished())
	{
		if ((_uint)ANIMATION_STATE::SPAWN_CLIMBOVER == m_pModelCom->GetCurrentAnimation())
		{
			m_IntroEnd = TRUE;
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F_V1);
		}
		else if ((_uint)ANIMATION_STATE::ATT_C_START == m_pModelCom->GetCurrentAnimation())
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::ATT_C_LOOP);
		else if ((_uint)ANIMATION_STATE::ATT_C_LOOP == m_pModelCom->GetCurrentAnimation())
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::ATT_C_LOOP);
		else if ((_uint)ANIMATION_STATE::ATT_C_HITWALL == m_pModelCom->GetCurrentAnimation() ||
			(_uint)ANIMATION_STATE::ATT_C_STRIKE == m_pModelCom->GetCurrentAnimation())
		{
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::RUN_F_V1);
			m_ChargeATT = FALSE;
			m_ChargeTime = 0.0;
		}
		else
		{
			m_MeleeAttackIsCollisionCheck = FALSE;
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F_V1);
		}
	}

	if (0 >= m_HP)
	{
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::DEA_CRITICAL);
		if (m_pModelCom->GetAnimationFinished())
		{
			m_Dead = TRUE;
		}
	}

	return _int();
}

HRESULT BossPrimeBeast::Render()
{
	if (TRUE == m_Dead)
		return S_OK;

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
	m_HeadCollider->Render();
	m_rHand1Collider->Render();
	m_rHand2Collider->Render();
	m_lHand1Collider->Render();
	m_lHand2Collider->Render();
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_bool BossPrimeBeast::Picked()
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
	CalDesc._rayDir = XMVector3Normalize(CalDesc._rayDir);

	_float Distance = 0.f;

	if (TRUE == m_ColliderCom->CollisionAABBToRay(CalDesc._rayPos, CalDesc._rayDir, Distance))
	{
		return TRUE;
	}
	return FALSE;
}

void BossPrimeBeast::Animation(_double TimeDelta)
{
	if (!m_FrameStart) {
		if (TRUE == static_cast<CCollider*>(m_Terrain->GetComponent(TEXT("Com_BossColliderCom")))->GetIsCollision())
			m_FrameStart = TRUE;
	}
	else {
		if (TRUE == m_pModelCom->GetAnimationFinished() && m_ChargeTime >= 15)
		{
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::ATT_C_START);
			m_TargetPosition = static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
			m_ChargeATT = TRUE;
			m_ChargeTime = 0.0;
		}
		if (TRUE == m_ChargeATT)
		{
			ChargeAttack(TimeDelta);
		}
		else  if (10 >= m_TargetDistance)
		{
			if ((_uint)ANIMATION_STATE::RUN_F_V1 != m_pModelCom->GetCurrentAnimation() && FALSE == m_pModelCom->GetAnimationFinished())
				return;
			_int num = rand() % 2;
			if (0 == num)
				ThrowAttack(TimeDelta);
			else if (1 == num)
				MeleeAttack(TimeDelta);
		}
		else
		{
			Moving(TimeDelta);
		}
	}
}

void BossPrimeBeast::GetDistance()
{
	m_PlayerPosition = static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_MyPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector Distance = m_PlayerPosition - m_MyPosition;

	m_TargetDistance = XMVectorGetX(XMVector3Length(Distance));
}

void BossPrimeBeast::ThrowAttack(_double _TimeDelta)
{
	_int NumRand = 0;//rand() % 6;
	_uint	AnimationState = (_uint)ANIMATION_STATE::ANIMATION_STATE_END;
	switch (NumRand)
	{
	case 0:
		AnimationState = (_uint)ANIMATION_STATE::ATT_TR_V1;
		break;
	case 1:
		AnimationState = (_uint)ANIMATION_STATE::ATT_TR_V2;
		break;
	case 2:
		AnimationState = (_uint)ANIMATION_STATE::ATT_TR_V3;
		break;
	case 3:
		AnimationState = (_uint)ANIMATION_STATE::ATT_TR_V4;
		break;
	case 4:
		AnimationState = (_uint)ANIMATION_STATE::ATT_TG_V1;
		break;
	case 5:
		AnimationState = (_uint)ANIMATION_STATE::ATT_TG_V2;
		break;
	}
	m_pModelCom->SetUp_AnimationIndex(AnimationState);

}

void BossPrimeBeast::MeleeAttack(_double _TimeDetla)
{
	_int NumRand = rand() % 5;
	_uint	AnimationState = (_uint)ANIMATION_STATE::ANIMATION_STATE_END;
	switch (NumRand)
	{
	case 0:
		AnimationState = (_uint)ANIMATION_STATE::ATT_P_V1;
		break;
	case 1:
		AnimationState = (_uint)ANIMATION_STATE::ATT_P_V2;
		break;
	case 2:
		AnimationState = (_uint)ANIMATION_STATE::ATT_P_V3;
		break;
	case 3:
		AnimationState = (_uint)ANIMATION_STATE::ATT_P_V4;
		break;
	case 4:
		AnimationState = (_uint)ANIMATION_STATE::ATT_P_V5;
		break;
	}
	m_pModelCom->SetUp_AnimationIndex(AnimationState);
}

void BossPrimeBeast::Moving(_double _TimeDelta)
{
	if ((_uint)ANIMATION_STATE::RUN_F_V1 == m_pModelCom->GetCurrentAnimation())
	{
		CTransform*		PlayerTransformCom = static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform")));
		m_pTransformCom->Chase_Target(PlayerTransformCom, _TimeDelta * 2.f);
	}
}

void BossPrimeBeast::AttackBlockCreate(_uint _AnimationState)
{
	_int		ThrowRockFrame = 0;
	switch ((ANIMATION_STATE)_AnimationState)
	{
	case ANIMATION_STATE::ATT_TR_V1:
		ThrowRockFrame = 18;
		break;
	case ANIMATION_STATE::ATT_TR_V2:
		ThrowRockFrame = 21;
		break;
	case ANIMATION_STATE::ATT_TR_V3:
		ThrowRockFrame = 29;
		break;
	case ANIMATION_STATE::ATT_TR_V4:
		ThrowRockFrame = 32;
		break;
	case ANIMATION_STATE::ATT_TG_V1:
		ThrowRockFrame = 38;
		break;
	case ANIMATION_STATE::ATT_TG_V2:
		ThrowRockFrame = 50;
		break;
	}

	if (ThrowRockFrame == m_pModelCom->GetCurrentAnimationFrame())
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
		BossPrimeBeastRock::BPRBDESC		BPRBDesc;
		ZeroMemory(&BPRBDesc, sizeof(BossPrimeBeastRock::BPRBDESC));
		BPRBDesc.m_PrimeBeast = this;
		BPRBDesc.m_Target = m_TargetPlayer;
		switch ((ANIMATION_STATE)_AnimationState)
		{
		case ANIMATION_STATE::ATT_TR_V1: // ¿À¸¥ÂÊ µÞ ÆÈ
			BPRBDesc.m_TypeRock = BossPrimeBeastRock::TYPE_ROCK::SMALL;
			BPRBDesc.m_TypeThrow = BossPrimeBeastRock::TYPE_THROW::DIRECT;
			BPRBDesc.m_BossBone = m_rHand2Bone;
			BPRBDesc.m_Type_Attack = BossPrimeBeastRock::TYPE_BOSS_ATTACK::ATT_TR_V1;

			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), TEXT("Prototype_GameObject_Model_BossPrimeBeastRock"), &BPRBDesc)))
				return;

			break;
		case ANIMATION_STATE::ATT_TR_V2:
			ThrowRockFrame = 21;
			break;
		case ANIMATION_STATE::ATT_TR_V3:
			ThrowRockFrame = 29;
			break;
		case ANIMATION_STATE::ATT_TR_V4:
			ThrowRockFrame = 32;
			break;
		case ANIMATION_STATE::ATT_TG_V1:
			ThrowRockFrame = 38;
			break;
		case ANIMATION_STATE::ATT_TG_V2:
			ThrowRockFrame = 50;
			break;
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

void BossPrimeBeast::ChargeAttack(_double _TimeDelta)
{
	if ((_uint)ANIMATION_STATE::ATT_C_START == m_pModelCom->GetCurrentAnimation())
	{
		if (40 <= m_pModelCom->GetCurrentAnimationFrame())
			m_pTransformCom->Go_Straight(_TimeDelta * 2.f, m_NavigationCom);
	}
	else if ((_uint)ANIMATION_STATE::ATT_C_LOOP == m_pModelCom->GetCurrentAnimation())
	{
		CGameInstance* GameInstance = GET_INSTANCE(CGameInstance);
		CCollider* GlacialFlowWall = static_cast<CCollider*>(GameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Wall")).front()->GetComponent(TEXT("Com_AABB")));
		RELEASE_INSTANCE(CGameInstance);

		if (FALSE == m_pTransformCom->Go_Straight(_TimeDelta * 4.f, m_NavigationCom))
		{
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::ATT_C_HITWALL);
		}
		else if ( nullptr != GlacialFlowWall && TRUE == m_ColliderCom->CollisionAABB(GlacialFlowWall))
		{
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::ATT_C_HITWALL);
		}
	}
}

_matrix BossPrimeBeast::GetBoneMatrix(CHierarchyNode*	_HierachyNode)
{
	_matrix Transform = XMMatrixTranslation(0.f, -30.f, 0.f);
	_matrix Offset = _HierachyNode->Get_OffsetMatrix();
	_matrix Combined = _HierachyNode->Get_CombinedMatrix();
	_matrix Pivot = m_pModelCom->Get_PivotMatrix();
	_matrix World = m_pTransformCom->Get_WorldMatrix();

	return Transform * Offset * Combined * Pivot * XMMatrixRotationY(XMConvertToRadians(180.f))  * World;
}

void BossPrimeBeast::BoneColliderTick(_double TimeDelta)
{
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

	Combined = m_HeadBone->Get_CombinedMatrix();
	OffsetMatrix = m_HeadBone->Get_OffsetMatrix();
	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix;
	m_HeadCollider->Update(BoneMatrix);

	Combined = m_rHand1Bone->Get_CombinedMatrix();
	OffsetMatrix = m_rHand1Bone->Get_OffsetMatrix();
	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix;
	m_rHand1Collider->Update(BoneMatrix);

	Combined = m_rHand2Bone->Get_CombinedMatrix();
	OffsetMatrix = m_rHand2Bone->Get_OffsetMatrix();
	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix;
	m_rHand2Collider->Update(BoneMatrix);

	Combined = m_lHand1Bone->Get_CombinedMatrix();
	OffsetMatrix = m_lHand1Bone->Get_OffsetMatrix();
	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix;
	m_lHand1Collider ->Update(BoneMatrix);

	Combined = m_lHand2Bone->Get_CombinedMatrix();
	OffsetMatrix = m_lHand2Bone->Get_OffsetMatrix();
	BoneMatrix = Transform * OffsetMatrix * Combined * PivotMatrix * XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix;
	m_lHand2Collider->Update(BoneMatrix);

}

void BossPrimeBeast::AttackCollision(_double _TimeDelta)
{
	if (TRUE == m_MeleeAttackIsCollisionCheck)
		return;
	ANIMATION_STATE AnimationState = (ANIMATION_STATE)m_pModelCom->GetCurrentAnimation();

	CCollider* PlayerAABB = static_cast<CCollider*>(m_TargetPlayer->GetComponent(TEXT("Com_AABB")));

	switch (AnimationState)
	{
	case ANIMATION_STATE::ATT_P_V1:
		if (TRUE == m_rHand1Collider->CollisionSphereToAABB(PlayerAABB) ||
			TRUE == m_rHand2Collider->CollisionSphereToAABB(PlayerAABB))
		{
			m_MeleeAttackIsCollisionCheck = TRUE;
			static_cast<CPlayer*>(m_TargetPlayer)->Hit(-45);
		}
		break;
	case ANIMATION_STATE::ATT_P_V2:
		if (TRUE == m_rHand1Collider->CollisionSphereToAABB(PlayerAABB) ||
			TRUE == m_lHand1Collider->CollisionSphereToAABB(PlayerAABB) ||
			TRUE == m_lHand2Collider->CollisionSphereToAABB(PlayerAABB))
		{
			m_MeleeAttackIsCollisionCheck = TRUE;
			static_cast<CPlayer*>(m_TargetPlayer)->Hit(-45);
		}
		break;
	case ANIMATION_STATE::ATT_P_V3:
		if (TRUE == m_rHand1Collider->CollisionSphereToAABB(PlayerAABB) ||
			TRUE == m_rHand2Collider->CollisionSphereToAABB(PlayerAABB) ||
			TRUE == m_lHand1Collider->CollisionSphereToAABB(PlayerAABB) ||
			TRUE == m_lHand2Collider->CollisionSphereToAABB(PlayerAABB))
		{
			m_MeleeAttackIsCollisionCheck = TRUE;
			static_cast<CPlayer*>(m_TargetPlayer)->Hit(-45);
		}
		break;
	case ANIMATION_STATE::ATT_P_V4:
		if (TRUE == m_lHand1Collider->CollisionSphereToAABB(PlayerAABB) ||
			TRUE == m_lHand2Collider->CollisionSphereToAABB(PlayerAABB))
		{
			m_MeleeAttackIsCollisionCheck = TRUE;
			static_cast<CPlayer*>(m_TargetPlayer)->Hit(-45);
		}
		break;
	case ANIMATION_STATE::ATT_P_V5:
		if (TRUE == m_rHand2Collider->CollisionSphereToAABB(PlayerAABB) ||
			TRUE == m_lHand1Collider->CollisionSphereToAABB(PlayerAABB))
		{
			m_MeleeAttackIsCollisionCheck = TRUE;
			static_cast<CPlayer*>(m_TargetPlayer)->Hit(-45);
		}
		break;
	case ANIMATION_STATE::ATT_C_LOOP:
		if (TRUE == m_ColliderCom->CollisionAABB(PlayerAABB))
		{
			m_MeleeAttackIsCollisionCheck = TRUE;
			static_cast<CPlayer*>(m_TargetPlayer)->Hit(-500);
		}
		break;
	}
}

void BossPrimeBeast::HitCheck()
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
		CalDesc._rayDir = XMVector3Normalize(CalDesc._rayDir);

		_float	Distance;

		if (TRUE == m_HeadCollider->CollisionSphereToRay(CalDesc._rayPos, CalDesc._rayDir))
		{
			_int AttDmg = static_cast<SMG*>(m_TargetPlayerWeapon)->GetAttackDamage();
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_HP -= AttDmg = pGameInstance->CalcRandom(AttDmg * 1.5f);
			RELEASE_INSTANCE(CGameInstance);
		}
		else if (TRUE == m_ColliderCom->CollisionAABBToRay(CalDesc._rayPos, CalDesc._rayDir, Distance))
		{
			_int AttDmg = static_cast<SMG*>(m_TargetPlayerWeapon)->GetAttackDamage();
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_HP -= AttDmg = pGameInstance->CalcRandom(AttDmg);
			RELEASE_INSTANCE(CGameInstance);
			/* ÇÇ°Ý ÀÌÆÑÆ® */
			_vector Position = CalDesc._rayPos + CalDesc._rayDir * Distance;
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_HitBullet"), &Position)))
				MSGBOX("Failed to Create HitBullet Effect!!!");
		}

		RELEASE_INSTANCE(CGameInstance);
	}
}

HRESULT BossPrimeBeast::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(60.0f);

	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BossPrimeBeast"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_NavigationCom)))
		return E_FAIL;

	/* Com_Collider */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(3.0f, 5.0f, 3.0f);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_ColliderCom, &CollisionDesc)))
		return E_FAIL;
	

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.5f, 1.5f, 1.5f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Head"), (CComponent**)&m_HeadCollider, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(2.0f, 2.0f, 2.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_rHand1"), (CComponent**)&m_rHand1Collider, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(2.0f, 2.0f, 2.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_rHand2"), (CComponent**)&m_rHand2Collider, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(2.0f, 2.0f, 2.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_lHand1"), (CComponent**)&m_lHand1Collider, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(2.0f, 2.0f, 2.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_lHand2"), (CComponent**)&m_lHand2Collider, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

BossPrimeBeast * BossPrimeBeast::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	BossPrimeBeast*	pInstance = new BossPrimeBeast(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating BossPrimeBeast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * BossPrimeBeast::Clone(void * pArg)
{
	BossPrimeBeast*	pInstance = new BossPrimeBeast(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning BossPrimeBeast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void BossPrimeBeast::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_ColliderCom);
	Safe_Release(m_NavigationCom);
	Safe_Release(m_rHand1Bone);
	Safe_Release(m_rHand2Bone);
	Safe_Release(m_lHand1Bone);
	Safe_Release(m_lHand2Bone);
	Safe_Release(m_rHand1Collider);
	Safe_Release(m_rHand2Collider);
	Safe_Release(m_lHand1Collider);
	Safe_Release(m_lHand2Collider);

}
