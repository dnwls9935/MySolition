#include "..\header\Time.h"


Time::Time()
	: deltaTime(0.0)
{
}

HRESULT Time::ReadyTime()
{
	QueryPerformanceCounter(&frameTime);
	QueryPerformanceCounter(&lastTime);
	QueryPerformanceCounter(&fixTime);

	QueryPerformanceFrequency(&cpuTick);

	return S_OK;
}

void Time::UpdateTime()
{
	QueryPerformanceCounter(&frameTime);
	if (frameTime.QuadPart - fixTime.QuadPart >= cpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&cpuTick);
		fixTime = frameTime;
	}

	deltaTime = (frameTime.QuadPart - lastTime.QuadPart) / (_double)cpuTick.QuadPart;
	lastTime = frameTime;
}

Time * Time::Create()
{
	Time* pInstance = new Time();
	if (FAILED(pInstance->ReadyTime()))
		Safe_Release(pInstance);

	return pInstance;
}

void Time::Free()
{
}

