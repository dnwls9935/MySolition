#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	enum class MOVE_TYPE{FRONT, BACK, RIGHT ,LEFT , MOVE_TYPE_END};
	enum class ROTATION_TYPE{ X, Y, ROTATION_TYPE_END};
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
	void Rotation_Axis(ROTATION_TYPE _rotation, _double TimeDelta, _long	MouseMove);
	void MoveCamera(MOVE_TYPE _moveType, _double TimeDelta);
public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

END
