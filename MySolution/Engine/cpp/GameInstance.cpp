#include "..\header\GameInstance.h"

IMPLEMENT_SINGLETON(GameInstance)

GameInstance::GameInstance()
	: dx11GraphicDev(DX11GraphicDev::GetInstance())
{
	Safe_AddRef(dx11GraphicDev);
}

HRESULT GameInstance::InitializeEngine(HWND _hWnd, DX11GraphicDev::WINMODE _mode, _uint _width, _uint _height)
{
	if (nullptr == dx11GraphicDev)
		return E_FAIL;

	if (FAILED(dx11GraphicDev->ReadyDX11Device(_hWnd, _mode, _width, _height)))
		return E_FAIL;

	return S_OK;
}

_int GameInstance::TickEngine(_double _timeDelta)
{
	return 0;
}

_int GameInstance::LateTick(_double _timeDelta)
{
	return 0;
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
	if (0 != GameInstance::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release GameInstance");

	if(0!=DX11GraphicDev::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release DX11GraphicDevice");

}

void GameInstance::Free()
{
	Safe_Release(dx11GraphicDev);
}
