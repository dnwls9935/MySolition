#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class ENGINE_DLL Timer final : public SuperBase
{
private:
	Timer();
	virtual ~Timer() = default;

public:
	_double GetTimeDelta() const {return deltaTimer; }
	HRESULT	ReadyTime();
	void	UpdateTime();


private:
	LARGE_INTEGER	frameTimer;
	LARGE_INTEGER	fixTimer;
	LARGE_INTEGER	lastTimer;
	LARGE_INTEGER	cpuTick;

	_double	deltaTimer;

public:
	static Timer* Create();
	virtual void Free() override;
};

END