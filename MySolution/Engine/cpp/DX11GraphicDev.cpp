#include "..\header\DX11GraphicDev.h"

IMPLEMENT_SINGLETON(DX11GraphicDev)


DX11GraphicDev::DX11GraphicDev()
{
}


HRESULT DX11GraphicDev::ReadyDX11Device()
{
	return S_OK;
}

void Engine::DX11GraphicDev::Free()
{
}
