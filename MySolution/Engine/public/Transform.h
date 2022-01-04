#pragma once

#include "Component.h"

/* 객체의 월드공간상에서의 상태.(WorldMatrix) */
/* 모든 정점들의 월드변환. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
public:
	typedef struct tagTransformDesc
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORMDESC;
private:
	explicit CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;
public:
	_fvector	Get_State(STATE eState) const {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	} 

	_float Get_Scale(STATE eState) const {
		return XMVectorGetX(XMVector3Length(Get_State(eState)));
	}

	_fmatrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_fmatrix Get_WorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4((_float4*)&m_WorldMatrix.m[eState][0], vState);
	}		

	void Set_TransformDesc(const TRANSFORMDESC& TransformDesc) {
		m_TransformDesc = TransformDesc;
	}
	void Set_Scale(_float _scale);
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	void Go_Straight(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	void Go_BackWard(_double TimeDelta);
	void Chase_Target(const CTransform* pTargetTransform, _double TimeDelta);
	void Face_Target(_fvector	vTargetPos);
	void Rotation_Axis(_fvector vAxis, _double TimeDelta); /* 동적으로 TimeDelta이용한 회전. */
	void SetUp_Rotation(_fvector vAxis, _float fRadian); /* 디폴트상태기준으로 특정축으로 몇도만큼 회전시켜놓겠다. */

private:
	_float4x4				m_WorldMatrix;	
	TRANSFORMDESC			m_TransformDesc;
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END