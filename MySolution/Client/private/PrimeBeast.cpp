#include "stdafx.h"
#include "..\public\PrimeBeast.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player.h"
#include "SMG.h"
#include "PrimeBeastRock.h"
#include "HitBullet.h"
#include "HitNum.h"

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

	m_MaxHP = 1000;
	m_HP = m_MaxHP;
	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::SPAWN_WALLJUMP);
	
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

	m_Terrain = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Terrain")).front();

	m_BurnColor = { 1.f, 0.49f, 0.f };

	RELEASE_INSTANCE(CGameInstance);

	m_Navigation->SettingDefaultIndex(m_pTransformCom);

	return S_OK;
}

_int PrimeBeast::Tick(_double TimeDelta)
{
	_vector Position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(Position), 0.f, XMVectorGetZ(Position), XMVectorGetW(Position)));

	if ((_uint)ANIMATION_STATE::DEA_CRITICAL == m_pModelCom->GetCurrentAnimation())
	{
		m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
		return _int();
	}

	if (TRUE == m_Hit && m_AccDodgeTime >= 6)
		m_Dodge = TRUE;
	else
		m_AccDodgeTime += TimeDelta;

	GetTargetDistance();
	Animation(TimeDelta);
	
	if(TRUE == m_FrameStart)
		m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	else
		m_pModelCom->Update_CombinedTransformationMatrix(0.0);
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_HpCom->Update(XMVectorSet(XMVectorGetX(Position), 2.5f, XMVectorGetZ(Position), 1.f), (_float)m_HP / (_float)m_MaxHP);

	if ((_uint)ANIMATION_STATE::ATT_TR_V1 == m_pModelCom->GetCurrentAnimation() ||
		(_uint)ANIMATION_STATE::RUN_R == m_pModelCom->GetCurrentAnimation())
	{
		_vector Look = m_PlayerPosition - m_MyPosition;
		Look = XMVector3Normalize(Look);
		_vector Right = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), Look);
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Right);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, Look);
	}

	HitCheck();
	
	return _int();
}

_int PrimeBeast::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM::RENDERCOM_HP, m_HpCom);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderCom);
#endif // !_DEBUG
	}

	if (m_pModelCom->GetAnimationFinished())
	{
		if ((_uint)ANIMATION_STATE::SPAWN_WALLJUMP == m_pModelCom->GetCurrentAnimation())
			m_IntroEnd = TRUE;

		//else if ((_uint)ANIMATION_STATE::ATT_TR_V1 == m_pModelCom->GetCurrentAnimation())
			//m_Dodge = TRUE;

		else if ((_uint)ANIMATION_STATE::RUN_R == m_pModelCom->GetCurrentAnimation())
		{
			m_Dodge = FALSE;
			m_Hit = FALSE;
			m_AccDodgeTime = 0.0;
		}

		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F_V1);
	}

	if (0 >= m_HP)
	{
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::DEA_CRITICAL);

		m_Burned = TRUE;
		m_BurnedTime += TimeDelta;

		if (m_pModelCom->GetAnimationFinished())
		{
			m_Dead = TRUE;
		}
	}

	return _int();
}

HRESULT PrimeBeast::Render()
{
	if (TRUE == m_Dead)
		return S_OK;

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
	//m_ColliderCom->Render();
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_bool PrimeBeast::Picked()
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

_bool PrimeBeast::ThrowMotionFrame()
{
	if ((_uint)ANIMATION_STATE::ATT_TR_V1 == m_pModelCom->GetCurrentAnimation() &&
		38 == m_pModelCom->GetCurrentAnimationFrame())
		return TRUE;

	return FALSE;
}

_matrix PrimeBeast::SetRockPosition()
{
	_matrix		Transform = XMMatrixTranslation(0.f, -30.f, 0.f);
	_matrix		OffsetMatrix = m_rHand2Bone->Get_OffsetMatrix();
	_matrix		CombinedMatrix = m_rHand2Bone->Get_CombinedMatrix();
	_matrix		PivotMatrix = m_pModelCom->Get_PivotMatrix();
	_matrix		WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	return Transform * OffsetMatrix * CombinedMatrix * PivotMatrix * XMMatrixRotationY(XMConvertToRadians(180.f)) * WorldMatrix;
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
		
		_float Distance = 0.f;

		if (TRUE == m_ColliderCom->CollisionAABBToRay(CalDesc._rayPos, CalDesc._rayDir, Distance))
		{
			_int AttDmg = static_cast<SMG*>(m_TargetPlayerWeapon)->GetAttackDamage();
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			m_HP-= AttDmg = pGameInstance->CalcRandom(AttDmg);
			RELEASE_INSTANCE(CGameInstance);

			m_Hit = TRUE;

			_vector Position = CalDesc._rayPos + CalDesc._rayDir * Distance;
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_HitBullet"), &Position)))
				MSGBOX("Failed to Create HitBullet Effect!!!");
/*
			HitNum::HUDESC HUD;
			ZeroMemory(&HUD, sizeof(HUD));
			HUD.Num = AttDmg;
			HUD.Position = Position;
			HUD.Parent = this;
			if(FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_HitNumber"), &HUD)))
				MSGBOX("Failed to Create HitNum!!!");
*/
		}
		
		RELEASE_INSTANCE(CGameInstance);
	}
}

void PrimeBeast::Animation(_double TimeDelta)
{
	if (FALSE == m_FrameStart)
	{
		if(TRUE == static_cast<CCollider*>(m_Terrain->GetComponent(TEXT("Com_FirstColliderCom")))->GetIsCollision())
			m_FrameStart = TRUE;
	}else if(TRUE == m_FrameStart && TRUE == m_IntroEnd)
	{
		if (FALSE == m_Dodge)
		{
			if (10 >= m_TargetDistance)
				Attack();
			else
				Moving(TimeDelta);
		}
		else
		{
			Dodge(TimeDelta);
		}
	}
}

void PrimeBeast::Moving(_double TimeDelta)
{
	if (TRUE == m_pModelCom->GetAnimationFinished())
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::RUN_F_V1);


	if ((_uint)ANIMATION_STATE::RUN_F_V1 == m_pModelCom->GetCurrentAnimation())
	{
		CTransform* PlayerTransformCom = static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform")));
		m_pTransformCom->Chase_Target(PlayerTransformCom, TimeDelta);
	}
}

void PrimeBeast::Attack()
{
	m_pModelCom->SetUp_AnimationIndex( (_uint)ANIMATION_STATE::ATT_TR_V1);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (18 == m_pModelCom->GetCurrentAnimationFrame()) {
		PrimeBeastRock::PRBDESC PrbDesc;
		ZeroMemory(&PrbDesc, sizeof(PrimeBeastRock::PRBDESC));
		PrbDesc.m_PrimeBeast = this;
		PrbDesc.m_Target = m_TargetPlayer;
		PrbDesc.m_TypeRock = PrimeBeastRock::TYPE_ROCK::SMALL;
		PrbDesc.m_TypeThrow= PrimeBeastRock::TYPE_THROW::DIRECT;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), TEXT("Prototype_GameObject_Model_PrimeBeastRock"), &PrbDesc)))
			return;
	}


	RELEASE_INSTANCE(CGameInstance);
}

void PrimeBeast::Dodge(_double TimeDelta)
{
	if (TRUE == m_pTransformCom->GoSideFDodge(TimeDelta * 2.5f, m_Navigation))
		m_pModelCom->SetUp_AnimationIndex((_uint)ANIMATION_STATE::RUN_R);
	else
		m_pTransformCom->Chase_Target(static_cast<CTransform*>(m_TargetPlayer->GetComponent(TEXT("Com_Transform"))), TimeDelta);
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

	/* Com_Navigation*/
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_Navigation)))
		return E_FAIL;

	/* Com_HP*/
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_UI_HP"), TEXT("Com_HP"), (CComponent**)&m_HpCom)))
		return E_FAIL;


	/* Com_Model */
	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(1.f, 1.f, 1.f);
	CollisionDesc.Position = _float3(0.f, 0.5f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), (CComponent**)&m_ColliderCom, &CollisionDesc)))
		return E_FAIL;

	return S_OK;
}

PrimeBeast * PrimeBeast::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	PrimeBeast*	pInstance = new PrimeBeast(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating PrimeBeast");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * PrimeBeast::Clone(void * pArg)
{
	PrimeBeast*	pInstance = new PrimeBeast(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating PrimeBeast");
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
	Safe_Release(m_Navigation);
	Safe_Release(m_HpCom);
}
