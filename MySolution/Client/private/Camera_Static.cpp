#include "stdafx.h"
#include "..\public\Camera_Static.h"
#include "GameInstance.h"

CCamera_Static::CCamera_Static(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{

}

CCamera_Static::CCamera_Static(const CCamera_Static & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Static::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Static::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CCamera_Static::Tick(_double TimeDelta)
{
	
	return CCamera::Tick(TimeDelta);
}

_int CCamera_Static::LateTick(_double TimeDelta)
{	
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrixInverse());

	pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJECTION, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	RELEASE_INSTANCE(CPipeLine);
	return CCamera::LateTick(TimeDelta);
}

HRESULT CCamera_Static::Render()
{
	return S_OK;
}

CCamera_Static * CCamera_Static::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Static*	pInstance = new CCamera_Static(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CCamera_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Static::Clone(void * pArg)
{
	CCamera_Static*	pInstance = new CCamera_Static(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CCamera_Static");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Static::Free()
{
	__super::Free();
	

}
