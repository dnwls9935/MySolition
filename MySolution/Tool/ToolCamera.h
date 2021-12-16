#pragma once

#include "GameObject.h"
#include "Camera.h"

BEGIN(Engine)
END
class ToolCamera final : public CCamera
{
private:
	explicit ToolCamera(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit ToolCamera(const ToolCamera& rhs);
	virtual ~ToolCamera() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT		DeviceInputKeyChecking(_double TimeDelta);

private:
	_float		m_mouseSenitive = 0.05f;

public:
	static ToolCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free();
};

