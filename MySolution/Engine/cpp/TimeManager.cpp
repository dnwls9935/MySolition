#include "..\header\TimeManager.h"
#include "Timer.h"
IMPLEMENT_SINGLETON(TimeManager)

TimeManager::TimeManager()
{
}

_double TimeManager::GetTimeDelta(const _tchar * _timerTag)
{
	Timer* pInstance = FindTimer(_timerTag);
	if (nullptr == pInstance)
		return 0.0;

	return pInstance->GetTimeDelta();
}

void TimeManager::UpdateTimeDelta(const _tchar * _timerTag)
{
	Timer* pInstance = FindTimer(_timerTag);
	if (nullptr == pInstance)
		return;

	pInstance->UpdateTime();
}

HRESULT TimeManager::ReadyTimer(const _tchar * _timerTag)
{
	Timer* pInstance = FindTimer(_timerTag);
	if (nullptr != pInstance)
		return E_FAIL;

	pInstance = Timer::Create();
	if (nullptr == pInstance)
		return E_FAIL;

	mapTimer.emplace(_timerTag, pInstance);

	return S_OK;
}

Timer * TimeManager::FindTimer(const _tchar * _timerTag)
{
	auto iter = find_if(mapTimer.begin(), mapTimer.end(), CTag_Finder(_timerTag));
	if (mapTimer.end() == iter)
		return nullptr;

	return iter->second;
}

void TimeManager::Free()
{
	for (auto& pair : mapTimer)
		Safe_Release(pair.second);

	mapTimer.clear();
}
