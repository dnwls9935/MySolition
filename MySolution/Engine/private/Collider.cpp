#include "..\public\Collider.h"

#include "Transform.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_Effect(rhs.m_Effect)
	, m_Batch(rhs.m_Batch)
	, m_InputLayout(rhs.m_InputLayout)
	, m_Color(rhs.m_Color)
	, m_Type(rhs.m_Type)
{
	Safe_AddRef(m_InputLayout);
}

CCollider::OBBDESC CCollider::ComputeOBBDesc()
{
	OBBDESC obbDesc;
	ZeroMemory(&obbDesc, sizeof(OBBDESC));

	_float3		pPoint[8];
	m_OBB->GetCorners(pPoint);

	XMStoreFloat3(&obbDesc.Center, (XMLoadFloat3(&pPoint[0]) + XMLoadFloat3(&pPoint[6])) * 0.5f);

	XMStoreFloat3(&obbDesc.AlignAxis[0], XMVector3Normalize(XMLoadFloat3(&pPoint[2]) - XMLoadFloat3(&pPoint[3])));
	XMStoreFloat3(&obbDesc.AlignAxis[1], XMVector3Normalize(XMLoadFloat3(&pPoint[2]) - XMLoadFloat3(&pPoint[1])));
	XMStoreFloat3(&obbDesc.AlignAxis[2], XMVector3Normalize(XMLoadFloat3(&pPoint[2]) - XMLoadFloat3(&pPoint[6])));

	XMStoreFloat3(&obbDesc.CenterAxis[0], ((XMLoadFloat3(&pPoint[2]) + XMLoadFloat3(&pPoint[5])) * 0.5f) - XMLoadFloat3(&obbDesc.Center));
	XMStoreFloat3(&obbDesc.CenterAxis[1], ((XMLoadFloat3(&pPoint[2]) + XMLoadFloat3(&pPoint[7])) * 0.5f) - XMLoadFloat3(&obbDesc.Center));
	XMStoreFloat3(&obbDesc.CenterAxis[2], ((XMLoadFloat3(&pPoint[2]) + XMLoadFloat3(&pPoint[0])) * 0.5f) - XMLoadFloat3(&obbDesc.Center));


	return obbDesc;
}
HRESULT CCollider::NativeConstruct_Prototype(TYPE _type)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_Type = _type;

	m_Effect = new BasicEffect(m_pDevice);
	m_Effect->SetVertexColorEnabled(TRUE);

	const void*	pShaderByteColor = nullptr;
	SIZE_T			pShaderByteCodeLength = 0;
	m_Effect->GetVertexShaderBytecode(&pShaderByteColor, &pShaderByteCodeLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteColor, pShaderByteCodeLength, &m_InputLayout)))
		return E_FAIL;

	m_Batch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);

	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	COLLISIONDESC* pCollisionDesc = (COLLISIONDESC*)pArg;

	m_Transform = CTransform::Create(m_pDevice, m_pDeviceContext);
	m_Transform->Set_Scale(XMLoadFloat3(&pCollisionDesc->Scale));
	m_Transform->SetUp_Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), pCollisionDesc->Rotation.x);
	m_Transform->SetUp_Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), pCollisionDesc->Rotation.y);
	m_Transform->SetUp_Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), pCollisionDesc->Rotation.z);

	m_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&pCollisionDesc->Position));
	
	switch (m_Type)
	{
	case TYPE::TYPE_AABB:
		m_OriginAABB = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f) );
		m_OriginAABB->Transform(*m_OriginAABB, m_Transform->Get_WorldMatrix());

		m_AABB = new BoundingBox;
		break;
	case TYPE::TYPE_OBB:
		m_OriginOBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 1.f));
		m_OriginOBB->Transform(*m_OriginOBB, m_Transform->Get_WorldMatrix());

		m_OBB = new BoundingOrientedBox;
		break;
	case TYPE::TYPE_SPHERE:
		m_OriginSphere = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		m_OriginSphere->Transform(*m_OriginSphere, m_Transform->Get_WorldMatrix());

		m_Sphere = new BoundingSphere;
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix _transform)
{
	_matrix WorldMatrix = RemoveScale(_transform);

	switch (m_Type)
	{
	case Engine::CCollider::TYPE::TYPE_AABB:
		WorldMatrix = RemoveRotation(WorldMatrix);
		m_OriginAABB->Transform(*m_AABB, WorldMatrix);
		break;
	case Engine::CCollider::TYPE::TYPE_OBB:
		m_OriginOBB->Transform(*m_OBB, WorldMatrix);
		break;
	case Engine::CCollider::TYPE::TYPE_SPHERE:
		m_OriginSphere->Transform(*m_Sphere, WorldMatrix);
		break;
	}
}

HRESULT CCollider::Render()
{
	CPipeLine* pipeLine = GET_INSTANCE(CPipeLine);

	m_Color = m_IsCollision == TRUE ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);
	m_Effect->SetView(pipeLine->Get_Transform(CPipeLine::D3DTS_VIEW));
	m_Effect->SetProjection(pipeLine->Get_Transform(CPipeLine::D3DTS_PROJECTION));

	m_pDeviceContext->IASetInputLayout(m_InputLayout);

	m_Effect->Apply(m_pDeviceContext);
	m_Batch->Begin();

	switch (m_Type)
	{
	case Engine::CCollider::TYPE::TYPE_AABB:
		DX::Draw(m_Batch, *m_AABB, XMLoadFloat4(&m_Color));
		break;
	case Engine::CCollider::TYPE::TYPE_OBB:
		DX::Draw(m_Batch, *m_OBB, XMLoadFloat4(&m_Color));
		break;
	case Engine::CCollider::TYPE::TYPE_SPHERE:
		DX::Draw(m_Batch, *m_Sphere, XMLoadFloat4(&m_Color));
		break;
	}

	m_Batch->End();

	RELEASE_INSTANCE(CPipeLine);
	return S_OK;
}

_bool CCollider::CollisionAABB(CCollider * _targetCollider)
{
	if (TYPE::TYPE_AABB != m_Type ||
		TYPE::TYPE_AABB != _targetCollider->m_Type)
		return FALSE;

	_vector SourMin, SourMax;
	_vector DestMin, DestMax;

	SourMin = GetMin(m_AABB);
	SourMax = GetMax(m_AABB);
	DestMin = _targetCollider->GetMin(_targetCollider->m_AABB);
	DestMax = _targetCollider->GetMax(_targetCollider->m_AABB);

	m_IsCollision = FALSE;

	if (max(XMVectorGetX(SourMin), XMVectorGetX(DestMin)) >
		min(XMVectorGetX(SourMax), XMVectorGetX(DestMax)))
		return FALSE;

	if (max(XMVectorGetY(SourMin), XMVectorGetY(DestMin)) >
		min(XMVectorGetY(SourMax), XMVectorGetY(DestMax)))
		return FALSE;

	if (max(XMVectorGetZ(SourMin), XMVectorGetZ(DestMin)) >
		min(XMVectorGetZ(SourMax), XMVectorGetZ(DestMax)))
		return FALSE;
	
	m_IsCollision = TRUE;

	return TRUE;
}

_bool CCollider::CollisionOBB(CCollider * _targetCollider)
{
	m_IsCollision = FALSE;

	OBBDESC		obbDesc[2];
	obbDesc[0] = ComputeOBBDesc();
	obbDesc[1] = _targetCollider->ComputeOBBDesc();

	_float		pDistance[3];

	_vector		pCenterDir = XMLoadFloat3(&obbDesc[0].Center) - XMLoadFloat3(&obbDesc[1].Center);

	for (_uint i = 0; i < 2; i++) {
		for (_uint j = 0; j < 2; j++) {
			pDistance[0] = fabs(XMVectorGetX(XMVector3Dot(pCenterDir, XMLoadFloat3(&obbDesc[i].AlignAxis[j]))));

			pDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obbDesc[0].CenterAxis[0]), XMLoadFloat3(&obbDesc[i].AlignAxis[j]))) + 
				XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obbDesc[0].CenterAxis[1]), XMLoadFloat3(&obbDesc[i].AlignAxis[j]))) + 
				XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obbDesc[0].CenterAxis[2]), XMLoadFloat3(&obbDesc[i].AlignAxis[j])))
			);

			pDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obbDesc[1].CenterAxis[0]), XMLoadFloat3(&obbDesc[i].AlignAxis[j]))) +
				XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obbDesc[1].CenterAxis[1]), XMLoadFloat3(&obbDesc[i].AlignAxis[j]))) +
				XMVectorGetX(XMVector3Dot(XMLoadFloat3(&obbDesc[1].CenterAxis[2]), XMLoadFloat3(&obbDesc[i].AlignAxis[j])))
			);

			if (pDistance[0] > pDistance[1] + pDistance[2])
				return FALSE;
		}
	}

	m_IsCollision = TRUE;
	return TRUE;
}

_bool CCollider::CollisionSPHERE(CCollider * _targetCollider)
{
	m_IsCollision = FALSE;

	_float	pDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Sphere->Center) - XMLoadFloat3(&_targetCollider->m_Sphere->Center)));

	_float SourDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_Sphere->Center) - XMVectorSet(m_Sphere->Radius, m_Sphere->Radius, m_Sphere->Radius,1.f)));
	_float DestDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&_targetCollider->m_Sphere->Center) - XMVectorSet(_targetCollider->m_Sphere->Radius, _targetCollider->m_Sphere->Radius, _targetCollider->m_Sphere->Radius, 1.f)));

	if (pDistance > SourDistance + DestDistance)
		return FALSE;

	m_IsCollision = TRUE;
	return TRUE;
}

_bool CCollider::CollisionAABBToRay(_fvector Ray, _fvector Dir)
{
	_float pDistance = 0;
	m_IsCollision = FALSE;

	if (!m_AABB->Intersects(Ray, Dir, pDistance))
	{
		return FALSE;
	}

	m_IsCollision = TRUE;

	return TRUE;
}

_bool CCollider::CollisionSphereToRay(_fvector Ray, _fvector Dir)
{
	_float pDistance = 0;
	m_IsCollision = FALSE;

	if (!m_Sphere->Intersects(Ray, Dir, pDistance))
	{
		return FALSE;
	}

	m_IsCollision = TRUE;
	return TRUE;
}

_bool CCollider::CollisionSphereToAABB(CCollider * _TargetCollider)
{
	if (TYPE::TYPE_AABB != _TargetCollider->m_Type)
		return FALSE;

	if (!m_Sphere->Intersects(*_TargetCollider->m_AABB))
		return FALSE;

	m_IsCollision = TRUE;

	return TRUE;
}

_fmatrix CCollider::RemoveScale(_fmatrix _transform)
{
	_matrix RemoveScaleMatrix = _transform;

	RemoveScaleMatrix.r[0] = XMVector3Normalize(_transform.r[0]);
	RemoveScaleMatrix.r[1] = XMVector3Normalize(_transform.r[1]);
	RemoveScaleMatrix.r[2] = XMVector3Normalize(_transform.r[2]);

	return RemoveScaleMatrix;
}

_fmatrix CCollider::RemoveRotation(_fmatrix _transform)
{
	_matrix RemoveRotationMatrix = _transform;

	_vector pScale = XMVectorSet(XMVectorGetX(XMVector3Length(_transform.r[0])) ,
		XMVectorGetX(XMVector3Length(_transform.r[1])),
		XMVectorGetX(XMVector3Length(_transform.r[2])),
		0.f);

	_vector Right, Up, Look;
	Right = XMVector3Normalize(_transform.r[0]) * XMVectorGetX(pScale);
	Up = XMVector3Normalize(_transform.r[1]) * XMVectorGetY(pScale);
	Look = XMVector3Normalize(_transform.r[2]) * XMVectorGetZ(pScale);

	RemoveRotationMatrix.r[0] = Right;
	RemoveRotationMatrix.r[1] = Up;
	RemoveRotationMatrix.r[2] = Look;

	return RemoveRotationMatrix;
}

_vector CCollider::GetMin(BoundingBox * _boundingBox)
{
	return XMVectorSet(_boundingBox->Center.x - _boundingBox->Extents.x ,
		_boundingBox->Center.y - _boundingBox->Extents.y,
		_boundingBox->Center.z - _boundingBox->Extents.z,
		1.f);
}

_vector CCollider::GetMax(BoundingBox * _boundingBox)
{
	return XMVectorSet(_boundingBox->Center.x + _boundingBox->Extents.x,
		_boundingBox->Center.y + _boundingBox->Extents.y,
		_boundingBox->Center.z + _boundingBox->Extents.z,
		1.f); 
}



CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE _type)
{
	CCollider* pInstance = new CCollider(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(_type)))
	{
		MSGBOX("Failed to Create Collider Component");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider* pInstance = new CCollider(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Create Collider Component");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	if (FALSE == m_isCloned)
	{
		Safe_Delete(m_Effect);
		Safe_Delete(m_Batch);
	}

	Safe_Release(m_Transform);
	Safe_Release(m_InputLayout);

	Safe_Delete(m_OriginAABB);
	Safe_Delete(m_AABB);
	Safe_Delete(m_OriginOBB);
	Safe_Delete(m_OBB);
	Safe_Delete(m_OriginSphere);
	Safe_Delete(m_Sphere);
}
