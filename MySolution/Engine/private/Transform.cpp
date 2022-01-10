#include "..\public\Transform.h"
#include "GameObject.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)	
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
	
}

void CTransform::Set_Scale(_fvector _scale)
{
	_vector		vRight = XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * XMVectorGetX(_scale);
	_vector		vUp = XMVector3Normalize(Get_State(CTransform::STATE_UP)) * XMVectorGetY(_scale);
	_vector		vLook = XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * XMVectorGetZ(_scale);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

HRESULT CTransform::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if(nullptr != pArg)
		memcpy(&m_TransformDesc, pArg,sizeof(TRANSFORMDESC));
	return S_OK;
}

void CTransform::Go_Straight(_double TimeDelta)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vRight) * -1.f * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);	
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_BackWard(_double TimeDelta)
{
	_vector		vLook = Get_State(CTransform::STATE_LOOK);
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);

	vPosition += XMVector3Normalize(vLook) * -1.f * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Chase_Target(const CTransform * pTargetTransform, _double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = pTargetTransform->Get_State(CTransform::STATE_POSITION) - vPosition;

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * (_float)TimeDelta;

	Face_Target(pTargetTransform->Get_State(CTransform::STATE_POSITION));

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Face_Target(_fvector vTargetPos)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = vTargetPos - vPosition;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	vLook = XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f));

	vLook = XMVector3Normalize(vLook) * Get_Scale(CTransform::STATE_LOOK);
	vRight = XMVector3Normalize(vRight) * Get_Scale(CTransform::STATE_RIGHT);

	Set_State(CTransform::STATE_LOOK, vLook);
	Set_State(CTransform::STATE_RIGHT, vRight);
}

void CTransform::Rotation_Axis(_fvector vAxis, _double TimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * (_float)TimeDelta);

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::SetUp_Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * Get_Scale(CTransform::STATE_RIGHT);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * Get_Scale(CTransform::STATE_UP);
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * Get_Scale(CTransform::STATE_LOOK);

	vRight = XMVector4Transform(vRight, RotationMatrix);
	vUp = XMVector4Transform(vUp, RotationMatrix);
	vLook = XMVector4Transform(vLook, RotationMatrix);

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}


CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTransform");
		Safe_Release(pInstance);
	}		

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();

}