#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class ENGINE_DLL Time final : public SuperBase
{
private:
	Time();
	virtual ~Time() = default;

public:
	_double GetTimeDelta() const {return deltaTime; }
	HRESULT	ReadyTime();
	void	UpdateTime();


private:
	LARGE_INTEGER	frameTime;
	LARGE_INTEGER	fixTime;
	LARGE_INTEGER	lastTime;
	LARGE_INTEGER	cpuTick;

	_double	deltaTime;

public:
	static Time* Create();
	virtual void Free() override;
};

END