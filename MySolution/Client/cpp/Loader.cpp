#include "stdafx.h"
#include "..\header\Loader.h"


Client::Loader::Loader(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext)
	: dx11Device(_dx11Direct)
	, dx11DirectContext(_dx11DirectContext)
{
	Safe_AddRef(dx11Device);
	Safe_AddRef(dx11DirectContext);
}

_uint APIENTRY ThreadMain(void* _arg) {
	/* 실질적인 작업 구간 */

	Loader* pLoader = (Loader*)_arg;

	EnterCriticalSection(&pLoader->GetCriticalSection());

	HRESULT hr = 0;

	switch ((_int)pLoader->GetNextLVL())
	{
	case (_int)LVL::LVL_LOGO:
		hr = pLoader->LoadingLogo();
		break;
	case (_int)LVL::LVL_GAMEPLAY:
		hr = pLoader->LoadingGamePlaye();
		break;
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&pLoader->GetCriticalSection());

	return 0;
}

HRESULT Client::Loader::NativeConstruct(LVL _nextLVL)
{
	nextLVL = _nextLVL;

	InitializeCriticalSection(&criticalSection);

	threadHandle = (HANDLE)_beginthreadex(nullptr, 0, ThreadMain, this, 0, nullptr);
	if (0 == threadHandle)
		return E_FAIL;

	return S_OK;
}

HRESULT Loader::LoadingLogo()
{
	threadFinished = TRUE;
	return S_OK;
}

HRESULT Loader::LoadingGamePlaye()
{
	threadFinished = TRUE;
	return S_OK;
}

Client::Loader* Client::Loader::Create(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext, LVL _nextLVL)
{
	Loader* pInstance = new Loader(_dx11Direct, _dx11DirectContext);
	if (FAILED(pInstance->NativeConstruct(_nextLVL)))
	{
		MSGBOX("Failed Create Loader(Thread)");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Loader::Free()
{
	WaitForSingleObject(threadHandle, INFINITE);
	DeleteCriticalSection(&criticalSection);
	DeleteObject(threadHandle);
	CloseHandle(threadHandle);

	Safe_Release(dx11Device);
	Safe_Release(dx11DirectContext);
}
