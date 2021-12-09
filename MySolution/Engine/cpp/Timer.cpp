#include "..\header\Timer.h"


Timer::Timer()
	: deltaTimer(0.0)
{
}

HRESULT Timer::ReadyTime()
{
	QueryPerformanceCounter(&frameTimer);
	QueryPerformanceCounter(&lastTimer);
	QueryPerformanceCounter(&fixTimer);

	QueryPerformanceFrequency(&cpuTick);

	return S_OK;
}

void Timer::UpdateTime()
{
	QueryPerformanceCounter(&frameTimer);
	if (frameTimer.QuadPart - fixTimer.QuadPart >= cpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&cpuTick);
		fixTimer = frameTimer;
	}

	deltaTimer = (frameTimer.QuadPart - lastTimer.QuadPart) / (_double)cpuTick.QuadPart;
	lastTimer = frameTimer;
}

Timer * Timer::Create()
{
	Timer* pInstance = new Timer();
	if (FAILED(pInstance->ReadyTime()))
		Safe_Release(pInstance);

	return pInstance;
}

void Timer::Free()
{
}

