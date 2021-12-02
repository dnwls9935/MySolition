#include "..\header\GameInstance.h"

IMPLEMENT_SINGLETON(GameInstance)

GameInstance::GameInstance()
	: dx11GraphicDev(DX11GraphicDev::GetInstance())
	, levelManager(LevelManager::GetInstance())
	, timeManager(TimeManager::GetInstance())
{
	Safe_AddRef(dx11GraphicDev);
	Safe_AddRef(levelManager);
	Safe_AddRef(timeManager);
}

HRESULT GameInstance::InitializeEngine(HWND _hWnd, DX11GraphicDev::WINMODE _mode, _uint _width, _uint _height, ID3D11Device** _dx11DeviceOut, ID3D11DeviceContext** _dx11DeviceContextOut)
{
	if (nullptr == dx11GraphicDev)
		return E_FAIL;

	if (FAILED(dx11GraphicDev->ReadyDX11Device(_hWnd, _mode, _width, _height, _dx11DeviceOut, _dx11DeviceContextOut)))
		return E_FAIL;

	return S_OK;
}

_int GameInstance::TickEngine(_double _timeDelta)
{
	if (nullptr == levelManager)
		return E_FAIL;

	return levelManager->Tick(_timeDelta);
}

_int GameInstance::LateTick(_double _timeDelta)
{
	if (nullptr == levelManager)
		return E_FAIL;

	return levelManager->LateTick(_timeDelta);
}

HRESULT GameInstance::Render()
{
	if (nullptr == levelManager)
		return E_FAIL;

	levelManager->Render();

	return S_OK;
}

HRESULT GameInstance::ClearBackBufferView(XMFLOAT4 _color)
{
	if (nullptr == dx11GraphicDev)
		return E_FAIL;

	return dx11GraphicDev->ClearBackBufferView(_color);
}

HRESULT GameInstance::ClearDepthStencilView()
{
	if (nullptr == dx11GraphicDev)
		return E_FAIL;

	return dx11GraphicDev->ClearDepthStencilView();
}

HRESULT GameInstance::Present()
{
	if (nullptr == dx11GraphicDev)
		return E_FAIL;

	return dx11GraphicDev->Present();
}

void GameInstance::ReleaseEngine()
{
	if (0 != TimeManager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release TimeManager");

	if (0 != GameInstance::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release GameInstance");

	if (0 != LevelManager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release LevelManager");

	if(0!=DX11GraphicDev::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release DX11GraphicDevice");

}

HRESULT Engine::GameInstance::OpenLVL(class Level* pLVL)
{
	if (nullptr == levelManager)
		return E_FAIL;

	return levelManager->OpenLVL(pLVL);
}

_double GameInstance::GetTimeDelta(const _tchar * _timerTag)
{
	if (nullptr == timeManager)
		return 0.0;

	return timeManager->GetTimeDelta(_timerTag);
}

void GameInstance::UpdateTimeDelta(const _tchar * _timerTag)
{
	if (nullptr == timeManager)
		return;

	timeManager->UpdateTimeDelta(_timerTag);
}

HRESULT GameInstance::ReadyTimer(const _tchar * _timerTag)
{
	if (nullptr == timeManager)
		return E_FAIL;

	return timeManager->ReadyTimer(_timerTag);
}


void GameInstance::Free()
{
	Safe_Release(timeManager);
	Safe_Release(levelManager);
	Safe_Release(dx11GraphicDev);
}
