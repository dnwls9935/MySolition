#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class TimeManager : public SuperBase
{
	DECLARE_SINGLETON(TimeManager)
private:
	explicit TimeManager();
	virtual ~TimeManager() = default;

public:
	_double					GetTimeDelta(const _tchar* _timerTag);
	void					UpdateTimeDelta(const _tchar* _timerTag);
	HRESULT					ReadyTimer(const _tchar* _timerTag);

private:
	class Timer*			FindTimer(const _tchar* _timerTag);

private:
	map<const _tchar*, Timer*>			mapTimer;


public:
	virtual void Free() override;
};

END