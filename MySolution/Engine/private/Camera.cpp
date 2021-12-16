#include "..\public\Camera.h"
#include "Transform.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{

}

HRESULT CCamera::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_pTransform = CTransform::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pTransform)
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	/* 셋팅하고자 했던 카메라의 초기상태를 트랜스폼에게 동기화했다. */
	_vector		vPosition = XMLoadFloat3(&m_CameraDesc.vEye);
	vPosition = XMVectorSetW(vPosition, 1.f);

	_vector		vLook = XMLoadFloat3(&m_CameraDesc.vAt) - XMLoadFloat3(&m_CameraDesc.vEye);
	vLook = XMVector3Normalize(vLook);

	_vector		vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
	vRight = XMVector3Normalize(vRight);

	_vector		vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE_UP, vUp);
	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPosition);	

	m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);

	return S_OK;
}

_int CCamera::Tick(_double TimeDelta)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	/* 카메라 월드행렬 역행렬 == 뷰스페이스 변환 행렬. */
	pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrixInverse());	

	pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJECTION, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	RELEASE_INSTANCE(CPipeLine);

	return _int();
}

_int CCamera::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera::Render()
{
	return S_OK;
}


void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
}
