#ifndef TimerMgr_h__
#define TimerMgr_h__

#include "Base.h"

BEGIN(Engine)

class CTimer_Manager : public CBase 
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	CTimer_Manager(void);
	virtual ~CTimer_Manager(void) = default;

public:
	_double					Get_TimeDelta(const _tchar* pTimerTag);
	void					Update_TimeDelta(const _tchar* pTimerTag);

public:
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

private:
	class CTimer*		Find_Timer(const _tchar* pTimerTag);

private:
	map<const _tchar*, class CTimer*>			m_mapTimer;

public:
	virtual void		Free(void);
};

END
#endif // TimerMgr_h__
