#pragma once

#include "Base.h"

BEGIN(Engine)
class CCollider;
END

class NaviPoint final : public CBase
{
private:
	explicit NaviPoint(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext);
	virtual ~NaviPoint() = default;
private:
	HRESULT NativeConstruct(_int Index, _fvector  _Position);

public:
	_int				Update();
	_bool			LateUpdate();
	HRESULT		Render();

	_bool			CollisionSphere(_fvector _MousePos,_fvector _MouseDir , NaviPoint** _NaviPoint);

	_float3 GetPosition()const {
		return m_Position;
	}

private:
	ID3D11Device*						m_Device = nullptr;
	ID3D11DeviceContext*			m_DeviceContext = nullptr;

	CCollider*		m_ColliderCom = nullptr;
	_uint				m_Index = 0;
	_float3			m_Position = _float3(0.f, 0.f, 0.f);
public:
	static NaviPoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _int _Index, _fvector _Position);
	virtual void Free() override;
};

