#pragma once

#include "ClientDefine.h"
#include "SuperBase.h"

BEGIN(Engine)
class GameInstance;
END

class MainApp final : public SuperBase
{
private:
	MainApp();
	virtual ~MainApp() = default;

public:
	HRESULT				NativeConstruct();
	_int				Tick(_double _timeDelta);
	_int				LateTick(_double _timeDelta);
	HRESULT				Render();
	
private:
	GameInstance*			gameInstance = nullptr;

public:
	static MainApp* Create();
	void Free() override;
};

