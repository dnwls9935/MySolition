#include "..\header\GameObj.h"
 
#include "GameInstance.h"

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

HRESULT GameObj::Render()
{
	return S_OK;
}

HRESULT GameObj::AddComponent(const _tchar * _protoTag, const _tchar * _tag, Component** _out, void* _arg)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	// gameInstance->CloneComponent(protoTag, default (void*)) 안에서 클론 처리함
	Component* component = gameInstance->CloneComponent(_protoTag, _arg);

	RELEASE_INSTANCE(GameInstance);

	if (nullptr == component)
		return E_FAIL;

	*_out = component;


	return S_OK;
}


void Engine::GameObj::Free()
{
	Safe_Release(dx11Device);
	Safe_Release(dx11DeviceContext);
}
