#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	enum class ROTATION_TYPE { X, Y, ROTATION_TYPE_END };
private:
	explicit CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;
public:	
	virtual HRESULT NativeConstruct_Prototype() override;	
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	SetCameraPosition(_matrix camPos, _matrix _PWM);
	void	RotationXY(ROTATION_TYPE _type, _double MouseMove);
	
public:
	_matrix	Get_WorldMatrix() {
		return m_pTransform->Get_WorldMatrix(); 
	};
	_bool		GetFocus() {
		return m_Focus;
	}

public:
	void			SetFOV(_float _FOV);
	void			ForcusCamera();

private:
	_matrix		m_OriginWM = XMMatrixIdentity();
	_bool			m_Focus = FALSE;
	_bool			m_IntroEnd = FALSE;
	
public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

END
