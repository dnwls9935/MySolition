#include "stdafx.h"
#include "..\public\Player.h"

#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Camera_Dynamic.h"
#include "SMG.h"
#include "Sky.h"
#include "UI.h"
#include "HITUI.h"

#include <iostream>

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameObject::TOOLOBJDESC		ToolObjDesc;
	ZeroMemory(&ToolObjDesc, sizeof(CGameObject::TOOLOBJDESC));
	memcpy(&ToolObjDesc, (CGameObject::TOOLOBJDESC*)pArg, sizeof(CGameObject::TOOLOBJDESC));

	_matrix TransformMatrix = XMLoadFloat4x4(&ToolObjDesc.m_pTransformMatrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, TransformMatrix.r[3]);

	m_CameraBone = m_pModelCom->Get_BoneMatrix("Head");
	m_WeaponBone = m_pModelCom->Get_BoneMatrix("R_Weapon_Bone");
	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::IDLE);

	m_Navigation->SettingDefaultIndex(m_pTransformCom);

	m_MaxHP = 1000;
	m_HP = m_MaxHP;

	m_MaxShield = 500;
	m_Shield = m_MaxShield;

	return S_OK;
}

_int CPlayer::Tick(_double TimeDelta)
{
	KeyCheck(TimeDelta);
	m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	SetCamAndSkyBox();
	SetUpWeapon();

	_vector Position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(Position), 0.f, XMVectorGetZ(Position), XMVectorGetW(Position)));

	m_HPPercent = (_float)m_HP / (_float)m_MaxHP;
	m_ShieldPercent = (_float)m_Shield / (_float)m_MaxShield;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*> pObjectList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
	for (auto& pObject : pObjectList)
	{
		if (CGameObject::UITYPE_ID::PLAYER_HP == static_cast<UI*>(pObject)->GetUIDesc().m_UITypeID)
		{
			static_cast<UI*>(pObject)->SetLength(m_HPPercent, TRUE);
		}else if (CGameObject::UITYPE_ID::PLAYER_SHILED == static_cast<UI*>(pObject)->GetUIDesc().m_UITypeID)
		{
			static_cast<UI*>(pObject)->SetLength(m_ShieldPercent, TRUE);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CPlayer::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
#ifdef _DEBUG
		m_pRendererCom->Add_RenderComGroup(CRenderer::RENDERCOM_COLLIDER, m_ColliderCom);
#endif // !_DEBUG
	}

	if (m_pModelCom->GetAnimationFinished())
	{
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::IDLE);
	}
		

	return _int();
}

HRESULT CPlayer::Render()
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
	//m_ColliderCom->Render();
	m_Navigation->Render();
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_fmatrix CPlayer::GetCameraMatrix()
{
	_matrix		Transform =  XMMatrixTranslation(10.f, 0.f, -10.f);
	_matrix		OffsetMatrix = XMMatrixIdentity();
	_matrix		CombinedMatrix = m_CameraBone->Get_CombinedMatrix();
	_matrix		PivotMatrix = m_pModelCom->Get_PivotMatrix();
	_matrix		WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	return Transform * OffsetMatrix * CombinedMatrix * PivotMatrix * WorldMatrix;
}


void CPlayer::SetUpWeapon()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TransMatrix = XMMatrixIdentity();
	_matrix OffsetMatrix = XMMatrixIdentity();
	_matrix CombinedMatrix = m_WeaponBone->Get_CombinedMatrix();
	_matrix PivotMatrix = m_pModelCom->Get_PivotMatrix();
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	_matrix BoneMatrix = TransMatrix * OffsetMatrix * CombinedMatrix * PivotMatrix * WorldMatrix;

	list<CGameObject*> m_LayerPlayer = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	auto& iter = m_LayerPlayer.begin();
	std::advance(iter, 1);
	static_cast<SMG*>(*iter)->SetUpWeapon(BoneMatrix, WorldMatrix);

	RELEASE_INSTANCE(CGameInstance);
}

CPlayer::RAY CPlayer::CreateRay()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	RAY ray;
	ZeroMemory(&ray, sizeof(RAY));

	Calculator::CALCDESC calDesc;
	ZeroMemory(&calDesc, sizeof(Calculator::CALCDESC));

	calDesc._height = g_iWinCY;
	calDesc._width = g_iWinCX;
	calDesc._hWnd = g_hWnd;
	calDesc._transformCom = m_pTransformCom;
	pGameInstance->CalcMousePos(&calDesc);

	calDesc._rayPos = XMVector3TransformCoord(calDesc._rayPos, m_pTransformCom->Get_WorldMatrix());
	calDesc._rayDir =XMVector3TransformNormal(calDesc._rayDir, m_pTransformCom->Get_WorldMatrix());
	XMVector3Normalize(calDesc._rayDir);

	ray.Ray = calDesc._rayPos;
	ray.Dir = calDesc._rayDir;

	RELEASE_INSTANCE(CGameInstance);

	return ray;
}

void CPlayer::PickUpHealth()
{
	m_HP = m_MaxHP;
}

void CPlayer::PickUpShield()
{
	m_Shield = m_MaxShield;
}

void CPlayer::PickUpSMGAmmo()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	auto iter = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player")).begin();
	advance(iter, 1);
	RELEASE_INSTANCE(CGameInstance);

	static_cast<SMG*>(*iter)->PickUpAmmo();
}

void CPlayer::PickUpShotGunAmmo()
{
}


void CPlayer::Hit(_int _HP)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*> UIs = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

	if (0 < m_Shield) {
		m_Shield += _HP;

		if (m_Shield <= 0)
			m_Shield = 0;

		for (auto& UI : UIs)
		{
			static_cast<HITUI*>(UI)->SetShow(HITUI::SHOW::SHIELD);
		}
	}
	else {
		m_HP += _HP;

		if (m_HP <= 0)
			m_HP = 0;

		for (auto& UI : UIs)
		{
			static_cast<HITUI*>(UI)->SetShow(HITUI::SHOW::HP);
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::PickUp(PickUps::TYPE_ID _ID)
{
	switch (_ID)
	{
	case Client::PickUps::TYPE_ID::SMG_AMMO:
		PickUpSMGAmmo();
		break;
	case Client::PickUps::TYPE_ID::SHOTGUN_AMMO:
		PickUpShotGunAmmo();
		break;
	case Client::PickUps::TYPE_ID::HEALTH_VIRAL:
		PickUpHealth();
		break;
	case Client::PickUps::TYPE_ID::BOOSTER_SHILED:
		PickUpShield();
		break;
	}
}

void CPlayer::KeyCheck(_double TimeDelta)
{
	m_InteractPick = FALSE;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if ((_int)ANIMATION_STATE::RE_HYPERION != m_pModelCom->GetCurrentAnimation() &&
		pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x8000)
		{
			m_pTransformCom->Go_Straight(TimeDelta * 1.5f, m_Navigation);
			if((_int)ANIMATION_STATE::RE_HYPERION != m_pModelCom->GetCurrentAnimation())
				m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::SPRINT);
			m_Move = MOVE_TYPE::FRONT;
		}
	}
	else {
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x8000)
		{
			m_pTransformCom->Go_Straight(TimeDelta, m_Navigation);
			if ((_int)ANIMATION_STATE::RE_HYPERION != m_pModelCom->GetCurrentAnimation())
				m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F);
			m_Move = MOVE_TYPE::FRONT;
		}
		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x8000)
		{
			m_pTransformCom->Go_BackWard(TimeDelta, m_Navigation);
			if ((_int)ANIMATION_STATE::RE_HYPERION != m_pModelCom->GetCurrentAnimation())
				m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F);
			m_Move = MOVE_TYPE::BACK;
		}
		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x8000)
		{
			m_pTransformCom->Go_Left(TimeDelta, m_Navigation);
			if ((_int)ANIMATION_STATE::RE_HYPERION != m_pModelCom->GetCurrentAnimation())
				m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_L);
			m_Move = MOVE_TYPE::LEFT;
		}
		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x8000)
		{
			m_pTransformCom->Go_Right(TimeDelta, m_Navigation);
			if ((_int)ANIMATION_STATE::RE_HYPERION != m_pModelCom->GetCurrentAnimation())
				m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_R);
			m_Move = MOVE_TYPE::RIGHT;
		}
		if ((_int)ANIMATION_STATE::SPRINT != m_pModelCom->GetCurrentAnimation() &&
			pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
		{
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RE_HYPERION);
		}
	}

	_long	MouseMove = 0;

	if (MouseMove = pGameInstance->Get_MouseMoveState(CInput_Device::MMS_X))
	{
		m_pTransformCom->Rotation_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
		m_Rotation = ROTATION_TYPE::X;
		//static_cast<CCamera_Dynamic*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Camera")).front())->RotationXY((CCamera_Dynamic::ROTATION_TYPE)m_Rotation, TimeDelta * MouseMove * 0.1f);
	}

	if (MouseMove = pGameInstance->Get_MouseMoveState(CInput_Device::MMS_Y))
	{
		m_pTransformCom->Rotation_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
		m_Rotation = ROTATION_TYPE::Y;
		//static_cast<CCamera_Dynamic*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Camera")).front())->RotationXY((CCamera_Dynamic::ROTATION_TYPE)m_Rotation, TimeDelta * MouseMove * 0.1f);
	}

	if (pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
		m_InteractPick = TRUE;

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::SetCamAndSkyBox()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	static_cast<CCamera_Dynamic*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Camera")).front())->SetCameraPosition(GetCameraMatrix(), m_pTransformCom->Get_WorldMatrix());
	static_cast<Sky*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_SkyBox")).front())->SetCamTransform();
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPlayer::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 6.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Navigation*/
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_Navigation)))
		return E_FAIL;

	CCollider::COLLISIONDESC CollisionDesc;
	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(0.7f, 1.8f, 0.7f);
	CollisionDesc.Position = _float3(0.f, 1.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"), (CComponent**)&m_ColliderCom, &CollisionDesc)))
		return E_FAIL;

	ZeroMemory(&CollisionDesc, sizeof(CCollider::COLLISIONDESC));
	CollisionDesc.Scale = _float3(0.5f, 0.5, 0.5f);
	CollisionDesc.Position = _float3(0.f, 0.f, 0.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Sphere"), (CComponent**)&m_ColliderSphereCom, &CollisionDesc)))
		return E_FAIL;

	return S_OK;

}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_ColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_ColliderSphereCom);
	Safe_Release(m_Navigation);
}
