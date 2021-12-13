#include "Timer_Manager.h"
#include "Timer.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

Engine::CTimer_Manager::CTimer_Manager(void)
{

}

_double Engine::CTimer_Manager::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer*		pInstance = Find_Timer(pTimerTag);
	if (nullptr == pInstance)
		return 0.0;

	return pInstance->Get_TimeDelta();
}

void CTimer_Manager::Update_TimeDelta(const _tchar * pTimerTag)
{
	CTimer*		pInstance = Find_Timer(pTimerTag);
	if (nullptr == pInstance)
		return;

	pInstance->Update_Timer();
}

HRESULT Engine::CTimer_Manager::Ready_Timer(const _tchar* pTimerTag)
{
	CTimer*		pInstance = Find_Timer(pTimerTag);

	if (nullptr != pInstance)
		return E_FAIL;

	pInstance = CTimer::Create();
	if (nullptr == pInstance)
		return E_FAIL;

	m_mapTimer.emplace(pTimerTag, pInstance);

	return S_OK;
}


Engine::CTimer* Engine::CTimer_Manager::Find_Timer(const _tchar* pTimerTag)
{
	// auto	iter = m_mapTimer.find(pTimerTag);

	auto	iter = find_if(m_mapTimer.begin(), m_mapTimer.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

void Engine::CTimer_Manager::Free(void)
{
	for (auto& Pair : m_mapTimer)
		Safe_Release(Pair.second);

	m_mapTimer.clear();
}


