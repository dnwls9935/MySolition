#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	typedef struct tagCollisionDesc {
		_float3		Scale;
		_float3		Rotation;
		_float3		Position;
	}COLLISIONDESC;

	typedef struct tagOBBDESC {
		_float3		Center;
		_float3		AlignAxis[3];
		_float3		CenterAxis[3];
	}OBBDESC;

public:
	enum class TYPE {TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END};

private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(TYPE _type);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	void			Update(_fmatrix _transform);
	HRESULT			Render();

public:
	_bool	CollisionAABB(CCollider* _targetCollider);
	_bool	CollisionOBB(CCollider* _targetCollider);
	_bool	CollisionSPHERE(CCollider* _targetCollider);
	_bool	CollisionAABBToRay(_fvector Ray, _fvector Dir);
	_bool	CollisionSphereToRay(_fvector Ray, _fvector Dir);

	void	IsCollision(_bool b) { m_IsCollision = b; };

private:
	_fmatrix RemoveScale(_fmatrix _transform);
	_fmatrix RemoveRotation(_fmatrix _transform);

	_vector	GetMin(BoundingBox* _boundingBox);
	_vector GetMax(BoundingBox* _boundingBox);
	OBBDESC ComputeOBBDesc();

private:
	BoundingBox*		m_OriginAABB = nullptr;
	BoundingBox*		m_AABB = nullptr;

	BoundingOrientedBox*		m_OriginOBB = nullptr;
	BoundingOrientedBox*		m_OBB = nullptr;

	BoundingSphere*		m_OriginSphere = nullptr;
	BoundingSphere*		m_Sphere = nullptr;

private:
	BasicEffect*												m_Effect = nullptr;
	PrimitiveBatch<VertexPositionColor>*		m_Batch = nullptr;
	ID3D11InputLayout*									m_InputLayout = nullptr;
	_float4														m_Color;

	class CTransform*										m_Transform = nullptr;
	_float4x4													m_ParentMatrix;

	TYPE														m_Type = TYPE::TYPE_END;
	_bool														m_IsCollision = FALSE;

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE _type);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END