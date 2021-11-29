#include "stdafx.h"
#include "..\header\MainApp.h"

#include "GameInstance.h"

MainApp::MainApp()
	: gameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(gameInstance);
}

HRESULT MainApp::NativeConstruct()
{
	if (nullptr == gameInstance)
		return E_FAIL;


	if (FAILED(gameInstance->InitializeEngine()))
		return E_FAIL;


	return S_OK;
}

Engine::_int MainApp::Tick(_double _timeDelta)
{
	return 0;
}

Engine::_int MainApp::LateTick(_double _timeDelta)
{
	return 0;
}

HRESULT MainApp::Render()
{
	return S_OK;
}

MainApp* MainApp::Create()
{
	MainApp* pInstance = new MainApp();
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating MainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void MainApp::Free()
{
	Safe_Release(gameInstance);
	GameInstance::ReleaseEngine();
}
