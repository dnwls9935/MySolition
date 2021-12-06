#include "stdafx.h"
#include "..\header\MainApp.h"
#include "GameInstance.h"
#include "Loading.h"
#include "Backgound.h"

MainApp::MainApp()
	: gameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(gameInstance);
}

HRESULT MainApp::NativeConstruct()
{
	if (nullptr == gameInstance)
		return E_FAIL;

	if (FAILED(gameInstance->InitializeEngine(g_hWnd, (_int)LVL::LVL_END , DX11GraphicDev::WINMODE::MODE_WIN, g_WIN_WIDTH, g_WIN_HEIGHT, &dx11Device, &dx11DeviceContext)))
		return E_FAIL;

	if (FAILED(ReadyComponentProtoType()))
		return E_FAIL;

	if (FAILED(ReadyGameObjectProtoType()))
		return E_FAIL;


	if (FAILED(SetUpLVL(LVL::LVL_LOGO)))
		return E_FAIL;

	return S_OK;
}

Engine::_int MainApp::Tick(_double _timeDelta)
{
	if (nullptr == gameInstance)
		return -1;

	gameInstance->TickEngine(_timeDelta);

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

	if (FAILED(gameInstance->Render()))
		return E_FAIL;

	if (FAILED(gameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT MainApp::SetUpLVL(LVL _nextLevel)
{
	if (nullptr == gameInstance)
		return E_FAIL;

	HRESULT hr = 0;
	if (_nextLevel == LVL::LVL_LOGO ||
		_nextLevel == LVL::LVL_GAMEPLAY)
		hr = gameInstance->OpenLVL(Loading::Create(dx11Device, dx11DeviceContext, _nextLevel));

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT MainApp::ReadyComponentProtoType()
{
	return S_OK;
}

HRESULT MainApp::ReadyGameObjectProtoType()
{
	if (nullptr == gameInstance)
		return E_FAIL;

	if (FAILED(gameInstance->Add_ProtoObj(TEXT("Proto_Background"), Backgound::Create(dx11Device, dx11DeviceContext))))
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
