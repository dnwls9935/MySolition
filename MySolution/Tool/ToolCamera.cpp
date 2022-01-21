#include "stdafx.h"
#include "ToolCamera.h"

#include "GameInstance.h"

ToolCamera::ToolCamera(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: CCamera(_dx11Device, _dx11DeviceContext)
{
}

ToolCamera::ToolCamera(const ToolCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT ToolCamera::NativeConstruct_Prototype()
{
	if(FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT ToolCamera::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

_int ToolCamera::Tick(_double TimeDelta)
{
	if(FAILED(DeviceInputKeyChecking(TimeDelta)))
		return -1;

	return CCamera::Tick(TimeDelta);
}

_int ToolCamera::LateTick(_double TimeDelta)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransform->Get_WorldMatrixInverse());

	pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJECTION, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));

	RELEASE_INSTANCE(CPipeLine);
	return CCamera::LateTick(TimeDelta);

	return _int();
}

HRESULT ToolCamera::Render()
{

	return S_OK;
}

HRESULT ToolCamera::DeviceInputKeyChecking(_double TimeDelta)
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	TimeDelta *= 1.5f;

	if (pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
	{
		m_pTransform->Go_Straight(TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
	{
		m_pTransform->Go_BackWard(TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
	{
		m_pTransform->Go_Left(TimeDelta);
	}

	if (pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
	{
		m_pTransform->Go_Right(TimeDelta);
	}

	if (pGameInstance->Get_MouseButtonState(CInput_Device::MBS_RBUTTON))
	{
		_long	MouseMove = 0;

		if (MouseMove = pGameInstance->Get_MouseMoveState(CInput_Device::MMS_X))
		{
			m_pTransform->Rotation_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * m_mouseSenitive);
		}

		if (MouseMove = pGameInstance->Get_MouseMoveState(CInput_Device::MMS_Y))
		{
			m_pTransform->Rotation_Axis(m_pTransform->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * m_mouseSenitive);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

ToolCamera * ToolCamera::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
{
	ToolCamera* pInstance = new ToolCamera(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX(L"Failed to Create ToolCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * ToolCamera::Clone(void * pArg)
{
	ToolCamera* pInstance = new ToolCamera(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(L"Failed to Clone ToolCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void ToolCamera::Free()
{
	__super::Free();
}
