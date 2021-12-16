#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float3		vEye, vAt, vAxisY;
		_float		fAspect, fFovy, fNear, fFar;

		CTransform::TRANSFORMDESC		TransformDesc;
	} CAMERADESC;

protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:	
	virtual HRESULT NativeConstruct_Prototype();	
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	/* 카메라의 월드 상태를 표현한다. */
	class CTransform*			m_pTransform = nullptr;
	CAMERADESC					m_CameraDesc;

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END