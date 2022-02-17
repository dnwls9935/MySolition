#include "stdafx.h"
#include "..\public\BugMorph.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player.h"
#include "SMG.h"
#include "Player.h"
#include "UI.h"
#include "BurrowDust.h"

BugMorph::BugMorph(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

BugMorph::BugMorph(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT BugMorph::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT BugMorph::NativeConstruct(void * pArg)
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

	m_MaxHP = 500;
	m_HP = m_MaxHP;
	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::SPAWN);


	m_LowerJawBone = m_pModelCom->Get_BoneMatrix("LowerJaw");
	if (nullptr == m_LowerJawBone)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*> TargetObjectList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	auto& iter = TargetObjectList.begin();
	advance(iter, 0);
	m_TargetPlayer = *iter;
	if (nullptr == m_TargetPlayer)
		return E_FAIL;

	advance(iter, 1);
	m_TargetPlayerWeapon = *iter;
	if (nullptr == m_TargetPlayerWeapon)
		return E_FAIL;
	
	m_Terrain = pGameInstance->GetObjectList(LEVEL_GAMEPLAY,TEXT("Layer_Terrain")).front();
	if (nullptr == m_Terrain)
		return E_FAIL;

	BurrowDust::BURROWDUST BurrowDustDesc;
	ZeroMemory(&BurrowDustDesc,sizeof(BurrowDust::BURROWDUST));
/*
	BurrowDustDesc.Position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	BurrowDustDesc.Parent = this;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_BurrowDust"), &BurrowDustDesc)))
		return E_FAIL;
*/
	m_BurnColor = { 0.749f, 1.f, 0.f };

	RELEASE_INSTANCE(CGameInstance);

	m_Navigation->SettingDefaultIndex(m_pTransformCom);
	return S_OK;
}

_int BugMorph::Tick(_double TimeDelta)
{
	_vector Position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(Position), 0.f, XMVectorGetZ(Position), XMVectorGetW(Position)));
	m_HpCom->Update(XMVectorSet(XMVectorGetX(Position), 2.5f, XMVectorGetZ(Position), 1.f), (_float)m_HP / (_float)m_MaxHP);
	if (FALSE == m_Burrow || TRUE == m_IntroEnd)
		HitCheck();

	if ((_uint)ANIMATION_STATE::DEA_CRITICAL == m_pModelCom->GetCurrentAnimation())
	{
		m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
		return _int();
	}

	if (TRUE == m_BurrowLoop)
	{
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::BURROW_EXIT);
		m_HP+= 10;
		if (m_MaxHP <= m_HP)
		{
			m_HP = m_MaxHP;
			m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
		}
		else
		{
			m_pModelCom->Update_CombinedTransformationMatrix(0.0);
		}
		return _int();
	}

	GetTargetDistance();

	if((_uint)ANIMATION_STATE::DEA_CRITICAL != m_pModelCom->GetCurrentAnimation() && FALSE == m_Burrow)
		Animation(TimeDelta);

	if (TRUE == m_FrameStart)
	{
		if ((_uint)ANIMATION_STATE::ATT_BITE == m_pModelCom->GetCurrentAnimation())
			m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta * 0.7f);
		else
			m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	}
	else
		m_pModelCom->Update_CombinedTransformationMatrix(0.0);
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	UpdateCollider(TimeDelta);
	AttackCollision(TimeDelta);
	
	return _int();
}

_int BugMorph::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_HP, m_HpCom);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderCom);
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderLowerJaw);
#endif // !_DEBUG
	}

	//if (TRUE == m_FrameStart && FALSE == m_IntroEnd &&
	//	(_uint)ANIMATION_STATE::SPAWN == m_pModelCom->GetCurrentAnimation())
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_BurrowDust"), &m_MyPosition)))
	//		return E_FAIL;
	//}


	if (m_pModelCom->GetAnimationFinished())
	{
		if ((_uint)ANIMATION_STATE::SPAWN == m_pModelCom->GetCurrentAnimation())
			m_IntroEnd = TRUE;
		else  if ((_uint)ANIMATION_STATE::ATT_BITE == m_pModelCom->GetCurrentAnimation())
		{
			m_Dodge = TRUE;
			m_MeleeAttackIsCollisionCheck = FALSE;
		}
		else if ((_uint)ANIMATION_STATE::BURROW_ENTER == m_pModelCom->GetCurrentAnimation())
		{
			m_BurrowCount--;
			m_BurrowLoop = TRUE;
			m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::BURROW_EXIT);
			m_BurrowStart = TRUE;
		}
		else if ((_uint)ANIMATION_STATE::BURROW_EXIT == m_pModelCom->GetCurrentAnimation())
		{
			m_Burrow = FALSE;
			m_BurrowTime = 0.0;
			m_BurrowLoop = FALSE;
		}
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F);
	}

	if (TRUE == m_Burrow)
		return _int();

	if (0 >= m_HP)
	{
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::DEA_CRITICAL);

		m_Burned = TRUE;
		m_BurnedTime += TimeDelta;

		if (m_pModelCom->GetAnimationFinished())
		{
			m_Dead = TRUE;
		}
	}else if( 0 <= m_BurrowCount && m_MaxHP * 0.5f >= m_HP ){
		m_Burrow = TRUE;
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::BURROW_ENTER);
	}

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT BugMorph::Render()
{
	if (TRUE == m_Dead)
		return S_OK;

	if (TRUE == m_BurrowStart) {
		m_BurrowStart = FALSE;
		return S_OK;
	}

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pModelCom->SetUp_ValueOnShader("g_Burned", &m_Burned, sizeof(_bool));
	m_pModelCom->SetUp_ValueOnShader("g_Time", &m_BurnedTime, sizeof(_float));
	m_pModelCom->SetUp_ValueOnShader("g_BurnColor", &m_BurnColor, sizeof(_float3));

	if (FAILED(m_pModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshContainer(); ++i)
	{
		m_pModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->SetUp_TextureOnShader("g_BurnedTexture", i, aiTextureType_OPACITY);

		m_pModelCom->Render(i, 1);
	}

#ifdef _DEBUG
	m_ColliderCom->Render();
	m_ColliderLowerJaw->Render();
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_bool BugMorph::Picked()
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
		m_HpCom->Picked(TRUE);
		return TRUE;
	}
	m_HpCom->Picked(FALSE);
	return FALSE;
}

void BugMorph::HitCheck()
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
		
		_float	Distance;

		if (TRUE == m_ColliderCom->CollisionAABBToRay(CalDesc._rayPos, CalDesc._rayDir, Distance))
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

void BugMorph::Animation(_double _TimeDelta)
{
	if (FALSE == m_FrameStart)
	{
		if (/*TRUE == static_cast<CCollider*>(m_Terrain->GetComponent(TEXT("Com_SecondColliderCom")))->GetIsCollision() && */10 >= m_TargetDistance)
			m_FrameStart= TRUE;
	}
	else if(TRUE == m_FrameStart && TRUE == m_IntroEnd) {
		if (3 >= m_TargetDistance && FALSE == m_Attack)
		{
			Attack();
		}
		else if (3 < m_TargetDistance && FALSE == m_Attack)
			Moving(_TimeDelta);
		else {
			Dodge(_TimeDelta);
		}
	}
}

void BugMorph::GetTargetDistance()
{
	m_PlayerPosition = static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_MyPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector	Distance = m_PlayerPosition - m_MyPosition;

	m_TargetDistance = XMVectorGetX(XMVector3Length(Distance));
}

void BugMorph::UpdateCollider(_double TimeDelta)
{
	_matrix BoneMatrix = XMMatrixIdentity();

	_matrix TransformMatrix = XMMatrixIdentity();
	_matrix OffsetMatrix = XMMatrixIdentity();
	_matrix CombinedMatrix = XMMatrixIdentity();
	_matrix PivotMatrix = XMMatrixIdentity();
	_matrix WorldMatrix = XMMatrixIdentity();


	OffsetMatrix = m_LowerJawBone->Get_OffsetMatrix();
	CombinedMatrix = m_LowerJawBone->Get_CombinedMatrix();
	PivotMatrix = m_pModelCom->Get_PivotMatrix();
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	BoneMatrix = TransformMatrix * OffsetMatrix * CombinedMatrix * PivotMatrix * XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix;
	m_ColliderLowerJaw->Update(BoneMatrix);
}

void BugMorph::Attack()
{
	m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::ATT_BITE);
	m_DodgeCount = 0.0;
	m_Attack = TRUE;
}

void BugMorph::Moving(_double _TimeDelta)
{
	if ((_uint)ANIMATION_STATE::RUN_F == m_pModelCom->GetCurrentAnimation())
	{
		CTransform*		PlayerTransformCom = static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform")));
		m_pTransformCom->Chase_Target(PlayerTransformCom, _TimeDelta);
	}
}

void BugMorph::Dodge(_double _TimeDelta)
{
	if ((_uint)ANIMATION_STATE::ATT_BITE == m_pModelCom->GetCurrentAnimation())
		return;
	if (TRUE == m_Dodge)
	{
		m_pTransformCom->Rotation_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
		m_Navigation->SettingDefaultIndex(m_pTransformCom);
		m_Dodge = FALSE;
	}
	
	m_DodgeCount += _TimeDelta;
	if (1.3 >= m_DodgeCount)
	{
		m_pTransformCom->Go_Straight(_TimeDelta, m_Navigation);
	}
	else {
		m_AttackPosition = _vector();
		m_Attack = FALSE;
		m_DodgeCount = 0.0;
	}
}

void BugMorph::AttackCollision(_double _TimeDelta)
{
	if (TRUE == m_MeleeAttackIsCollisionCheck)
		return;

	ANIMATION_STATE AnimationState = (ANIMATION_STATE)m_pModelCom->GetCurrentAnimation();

	CCollider*		PlayerAABB = static_cast<CCollider*>(m_TargetPlayer->GetComponent(TEXT("Com_AABB")));

	if (ANIMATION_STATE::ATT_BITE == AnimationState)
	{
		if (TRUE == m_ColliderLowerJaw->CollisionSphereToAABB(PlayerAABB))
		{
			m_MeleeAttackIsCollisionCheck = TRUE;
			static_cast<CPlayer*>(m_TargetPlayer)->Hit(-50);
		}
	}
}

HRESULT BugMorph::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 8.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BugMorph"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Navigation*/
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_Navigation)))
		return E_FAIL;

	/* Com_HP*/
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_UI_HP"), TEXT("Com_HP"), (CComponent**)&m_HpCom)))
		return E_FAIL;

	/* Com_Collider */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.0f, 1.0f, 1.0f);
	CollisionDesc.Position = _float3(0.f, 0.5f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_ColliderCom, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.0f, 1.0f, 1.0f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Sphere"), (CComponent**)&m_ColliderLowerJaw, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

BugMorph * BugMorph::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	BugMorph*	pInstance = new BugMorph(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * BugMorph::Clone(void * pArg)
{
	BugMorph*	pInstance = new BugMorph(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void BugMorph::Free()
{
	__super::Free();

	Safe_Release(m_ColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_ColliderLowerJaw);
	Safe_Release(m_Navigation);
	Safe_Release(m_HpCom);
}
