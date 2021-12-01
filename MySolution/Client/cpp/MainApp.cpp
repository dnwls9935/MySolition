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

	if (FAILED(gameInstance->InitializeEngine(g_hWnd, DX11GraphicDev::WINMODE::MODE_WIN, g_WIN_WIDTH, g_WIN_HEIGHT)))
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
	if (nullptr == gameInstance)
		return E_FAIL;

	if (FAILED(gameInstance->ClearBackBufferView(XMFLOAT4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(gameInstance->ClearDepthStencilView()))
		return E_FAIL;




	if (FAILED(gameInstance->Present()))
		return E_FAIL;

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
