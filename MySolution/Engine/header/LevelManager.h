#pragma once

#include "SuperBase.h"

BEGIN(Engine)
class LevelManager final : public SuperBase
{
	DECLARE_SINGLETON(LevelManager)
private:
	LevelManager();
	virtual ~LevelManager() = default;

public:
	HRESULT		OpenLVL(class Level* pLVL);
	_int		Tick(_double _timeDelta);
	_int		LateTick(_double _timeDelta);
	HRESULT		Render();

private:
	class Level*	currentLVL = nullptr;

public:
	virtual void Free() override;
};

END