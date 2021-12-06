#include "..\header\GameObj.h"
 


Engine::GameObj::GameObj(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext)
	: dx11Device(_dx11Device)
	, dx11DeviceContext(_dx11DeviceContext)
{
	Safe_AddRef(dx11Device);
	Safe_AddRef(dx11DeviceContext);
}

Engine::GameObj::GameObj(const GameObj& rhs)
	: dx11Device(rhs.dx11Device)
	, dx11DeviceContext(rhs.dx11DeviceContext)
{
	Safe_AddRef(dx11Device);
	Safe_AddRef(dx11DeviceContext);
}

HRESULT Engine::GameObj::NativeConstructProtoType()
{
	return S_OK;
}

HRESULT Engine::GameObj::NativeConstruct(void* _arg)
{
	return S_OK;
}

_int GameObj::Tick(_double _timeDelta)
{
	return _int();
}

_int GameObj::LateTick(_double _timeDelta)
{
	return _int();
}

void Engine::GameObj::Free()
{
	Safe_Release(dx11Device);
	Safe_Release(dx11DeviceContext);
}
