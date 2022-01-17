#include "stdafx.h"
#include "..\public\Camera_Dynamic.h"
#include "GameInstance.h"
#include "Player.h"


CCamera_Dynamic::CCamera_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{

}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CCamera_Dynamic::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix camPos = static_cast<CPlayer*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player")).front())->m_pTransformCom->Get_WorldMatrix();
	
	_vector Right = camPos.r[0];
	_vector Up = camPos.r[1];
	_vector Look = camPos.r[2];
	_vector Position = camPos.r[3];

	Position = XMVectorSetY(Position, XMVectorGetY(Position) + 2.f);

	m_pTransform->Set_State(CTransform::STATE_POSITION, Right);
	m_pTransform->Set_State(CTransform::STATE_POSITION, Up);
	m_pTransform->Set_State(CTransform::STATE_POSITION, Look);
	m_pTransform->Set_State(CTransform::STATE_POSITION, Position);

/*
	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	{
		m_pTransform->Go_Straight(TimeDelta);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	{
		m_pTransform->Go_BackWard(TimeDelta);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	{
		m_pTransform->Go_Right(TimeDelta);
	}
	else if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	{
		m_pTransform->Go_Left(TimeDelta);
	}
*/
	RELEASE_INSTANCE(CGameInstance);
	return CCamera::Tick(TimeDelta);
}

_int CCamera_Dynamic::LateTick(_double TimeDelta)
{
	
	return _int();
}

HRESULT CCamera_Dynamic::Render()
{
	return S_OK;
}

void CCamera_Dynamic::Rotation_Axis(ROTATION_TYPE _rotation,_double TimeDelta, _long	MouseMove)
{
	if (ROTATION_TYPE::X == _rotation)
	{
		m_pTransform->Rotation_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
	}
	else if (ROTATION_TYPE::Y == _rotation)
	{
		m_pTransform->Rotation_Axis(m_pTransform->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
	}
}

void CCamera_Dynamic::MoveCamera(MOVE_TYPE _moveType, _double TimeDelta)
{
	switch (_moveType)
	{
	case Client::CCamera_Dynamic::MOVE_TYPE::FRONT:
		m_pTransform->Go_Straight(TimeDelta);
		break;
	case Client::CCamera_Dynamic::MOVE_TYPE::BACK:
		m_pTransform->Go_BackWard(TimeDelta);
		break;
	case Client::CCamera_Dynamic::MOVE_TYPE::RIGHT:
		m_pTransform->Go_Right(TimeDelta);
		break;
	case Client::CCamera_Dynamic::MOVE_TYPE::LEFT:
		m_pTransform->Go_Left(TimeDelta);
		break;
	}
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void * pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();
	

}
