#include "stdafx.h"
#include "..\public\Loader.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

_uint APIENTRY Thread_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	HRESULT			hr = 0;

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		hr = pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		hr = pLoader->Loading_ForLogo();
		break;
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}


HRESULT CLoader::NativeConstruct(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CS);

	/* 스레드를 생성한다. -> 진입점함수를 정의한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	/* 컴포넌트 원형을 생성한다. */
	wsprintf(m_szLoading, TEXT("버퍼를 생성한다. "));

	for (_uint i = 0; i < 999999999; ++i)
		_uint iData = 10;

	wsprintf(m_szLoading, TEXT("텍스쳐를 생성한다. "));

	for (_uint i = 0; i < 999999999; ++i)
		_uint iData = 10;

	/* 객체원형을 생성한다. */
	


	wsprintf(m_szLoading, TEXT("로딩이 완료되었습니다. "));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	return S_OK;
}

void CLoader::Print_LoaderText()
{
	SetWindowText(g_hWnd, m_szLoading);
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSGBOX("Failed to Creating CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);	
	DeleteCriticalSection(&m_CS);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);		

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
