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

	return CCamera::Tick(TimeDelta);
}

_int CCamera_Dynamic::LateTick(_double TimeDelta)
{	
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrixInverse());

	pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJECTION, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	RELEASE_INSTANCE(CPipeLine);
	return CCamera::LateTick(TimeDelta);
}

HRESULT CCamera_Dynamic::Render()
{
	return S_OK;
}

void CCamera_Dynamic::SetCameraPosition(_matrix camPos, _matrix _PWM)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector		vPosition = camPos.r[3];
	vPosition = XMVectorSetW(vPosition, 1.f);

	_vector		vLook = _PWM.r[2];
	vLook = XMVector3Normalize(vLook);

	_vector		vRight = XMVector3Cross(_PWM.r[1], vLook);
	vRight = XMVector3Normalize(vRight);

	_vector		vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE_UP, vUp);
	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_Dynamic::RotationXY(ROTATION_TYPE _type, _double TimeDelta)
{
	switch (_type)
	{
	case Client::CCamera_Dynamic::ROTATION_TYPE::X:
		m_pTransform->Rotation_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta);
		break;
	case Client::CCamera_Dynamic::ROTATION_TYPE::Y:
		m_pTransform->Rotation_Axis(m_pTransform->Get_State(CTransform::STATE_RIGHT), TimeDelta);
		break;
	}
}

void CCamera_Dynamic::SetFOV(_float _FOV)
{
	m_CameraDesc.fFovy = _FOV;
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
