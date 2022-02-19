#include "stdafx.h"
#include "NaviPoint.h"
#include "GameInstance.h"

NaviPoint::NaviPoint(ID3D11Device * _Device, ID3D11DeviceContext * _DeviceContext)
	: m_Device(_Device)
	, m_DeviceContext(_DeviceContext)
{
	Safe_AddRef(m_Device);
	Safe_AddRef(m_DeviceContext);
}

HRESULT NaviPoint::NativeConstruct(_int _Index, _fvector  _Position)
{
	m_Index = _Index;
	XMStoreFloat3(&m_Position, _Position);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CCollider::COLLISIONDESC pCollisionDesc;
	pCollisionDesc.Position = m_Position;
	pCollisionDesc.Rotation = _float3(0.f, 0.f, 0.f);
	pCollisionDesc.Scale = _float3(0.5f, 0.5f, 0.5f);
	m_ColliderCom = (CCollider*)pGameInstance->Clone_Component(0, TEXT("Prototype_Component_Collider_Sphere"), &pCollisionDesc);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int NaviPoint::Update()
{
	m_ColliderCom->Update(XMMatrixIdentity());

	return _int();
}

_bool NaviPoint::LateUpdate()
{
	

	return _bool();
}

HRESULT NaviPoint::Render()
{
	m_ColliderCom->Render();
	return S_OK;
}

_bool NaviPoint::CollisionSphere(_fvector _MousePos, _fvector _MouseDir, NaviPoint** _NaviPoint)
{
	_float Distance;
	if (m_ColliderCom->CollisionSphereToRay(_MousePos, _MouseDir, Distance))
	{
		*_NaviPoint = this;
		return TRUE;
	}

	return FALSE;
}

NaviPoint * NaviPoint::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _int _Index, _fvector  _Position)
{
	NaviPoint* pInstance = new NaviPoint(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct(_Index, _Position)))
	{
		MSGBOX(L"Failed to Create NaviPoint");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void NaviPoint::Free()
{
	Safe_Release(m_Device);
	Safe_Release(m_DeviceContext);
}
