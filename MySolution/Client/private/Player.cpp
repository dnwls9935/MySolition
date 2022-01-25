#include "stdafx.h"
#include "..\public\Player.h"

#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Camera_Dynamic.h"
#include "GunTest.h"
#include "Sky.h"

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

	m_CameraBone = m_pModelCom->Get_BoneMatrix("Camera");
	m_WeaponBone = m_pModelCom->Get_BoneMatrix("R_Weapon_Bone");
	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::IDLE);

	return S_OK;
}

_int CPlayer::Tick(_double TimeDelta)
{
	KeyCheck(TimeDelta);
	m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	m_ColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	SetCamAndSkyBox();
	SetUpWeapon();
	return _int();
}

_int CPlayer::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (m_pModelCom->GetAnimationFinished())
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::IDLE);

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
	m_ColliderCom->Render();
#endif // _DEBUG

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_fmatrix CPlayer::GetCameraMatrix()
{
	_matrix		Transform = XMMatrixTranslation(0.f , -2500.f, 0.f);
	_matrix		OffsetMatrix = XMMatrixIdentity();
	_matrix		CombinedMatrix = m_CameraBone->Get_CombinedMatrix() * XMMatrixScaling(0.01f, 0.01f, 0.01f);
	_matrix		PivotMatrix = m_pModelCom->Get_PivotMatrix();
	_matrix		WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	return Transform* PivotMatrix* OffsetMatrix * CombinedMatrix  * WorldMatrix;
}

void CPlayer::Shotting()
{
	RAY ray = CreateRay();
	// �����丵 �ʼ�
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*> objList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));

	for (auto& iter : objList)
	{
		iter->CheckHit(ray.Ray, ray.Dir);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::SetUpWeapon()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix TransMatrix = XMMatrixTranslation(-100.f, 3500.f, -2000.f);
	_matrix OffsetMatrix = XMMatrixIdentity();//m_WeaponBone->Get_OffsetMatrix();
	_matrix CombinedMatrix = m_WeaponBone->Get_CombinedMatrix() * XMMatrixScaling(0.01f, 0.01f, 0.01f);
	_matrix PivotMatrix = m_pModelCom->Get_PivotMatrix();
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();


	_matrix BoneMatrix = TransMatrix* PivotMatrix* OffsetMatrix * CombinedMatrix  * WorldMatrix;

	list<CGameObject*> m_LayerPlayer = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
	auto& iter = m_LayerPlayer.begin();
	std::advance(iter, 1);
	static_cast<GunTest*>(*iter)->SetUpWeapon(BoneMatrix, WorldMatrix);

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

void CPlayer::KeyCheck(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x8000)
		{
			m_pTransformCom->Go_Straight(TimeDelta * 1.5f);
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::SPRINT);
			m_Move = MOVE_TYPE::FRONT;
		}
	}
	else {
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x8000)
		{
			m_pTransformCom->Go_Straight(TimeDelta);
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F);
			m_Move = MOVE_TYPE::FRONT;
		}
		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x8000)
		{
			m_pTransformCom->Go_BackWard(TimeDelta);
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_F);
			m_Move = MOVE_TYPE::BACK;
		}
		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x8000)
		{
			m_pTransformCom->Go_Left(TimeDelta);
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_L);
			m_Move = MOVE_TYPE::LEFT;
		}
		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x8000)
		{
			m_pTransformCom->Go_Right(TimeDelta);
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RUN_B);
			m_Move = MOVE_TYPE::RIGHT;
		}
		if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
		{
			m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RELOAD);
		}

		if (pGameInstance->Get_MouseButtonState(CInput_Device::MOUSEBUTTONSTATE::MBS_LBUTTON))
		{
			Shotting();
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
	TransformDesc.fSpeedPerSec = 10.f;
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
}
