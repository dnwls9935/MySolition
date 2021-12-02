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
	class Time*			FindTimer(const _tchar* _timerTag);

private:
	map<const _tchar*, Time*>			mapTimer;


public:
	virtual void Free() override;
};

END