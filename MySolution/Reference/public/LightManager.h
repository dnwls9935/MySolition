#pragma once

#include "Base.h"

BEGIN(Engine)

class LightManager final : public CBase
{
	DECLARE_SINGLETON(LightManager);
private:
	explicit LightManager();
	virtual ~LightManager() = default;

public:
	const LIGHTDESC*		GetLightDesc(_uint	Index = 0);

public:
	HRESULT		AddLight(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext, const LIGHTDESC& _LightDesc);
	HRESULT		RenderLights();

private:
	list<class Light*>					m_Lights;
	typedef list<class Light*>	LIGHTS;

public:
	virtual void Free() override;
};

END