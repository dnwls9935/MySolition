#include "Timer.h"


Engine::CTimer::CTimer(void)
	: m_fTimeDelta(0.0)
{

}



HRESULT Engine::CTimer::Ready_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime);			// 1077
	QueryPerformanceCounter(&m_LastTime);			// 1085
	QueryPerformanceCounter(&m_FixTime);			// 1090

	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void Engine::CTimer::Update_Timer(void)
{
	QueryPerformanceCounter(&m_FrameTime);			// 1500

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_FixTime = m_FrameTime;
	}
		
	m_fTimeDelta = (m_FrameTime.QuadPart - m_LastTime.QuadPart) / (_double)m_CpuTick.QuadPart;
	m_LastTime = m_FrameTime;
}

Engine::CTimer* Engine::CTimer::Create(void)
{
	CTimer*		pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CTimer::Free(void)
{

}

