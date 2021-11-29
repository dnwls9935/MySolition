#pragma once

#include "DX11GraphicDev.h"

BEGIN(Engine)

class ENGINE_DLL GameInstance final : public SuperBase
{
	DECLARE_SINGLETON(GameInstance)
private:
	GameInstance();
	virtual ~GameInstance() = default;

public:
	HRESULT				InitializeEngine();
	_int				TickEngine(_double _timeDelta);
	_int				LateTick(_double _timeDelta);


private:
	DX11GraphicDev*		dx11GraphicDev = nullptr;

public:
	static void ReleaseEngine();
	void Free() override;
};

END