#include "..\header\Level.h"

Level::Level(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext)
	: dx11Device(_dx11Device)
	, dx11DeviceContext(_dx11DeviceContext)
{
	Safe_AddRef(dx11DeviceContext);
	Safe_AddRef(dx11Device);
}

HRESULT Engine::Level::NativeConstruct()
{
	return S_OK;
} 

_int Engine::Level::Tick(_double _timeDelta)
{
	return 0;
}

_int Engine::Level::LateTick(_double _timeDelta)
{
	return 0;
}

HRESULT Engine::Level::Render()
{
	return S_OK;
}

void Engine::Level::Free()
{
	Safe_Release(dx11Device);
	Safe_Release(dx11DeviceContext);
}