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

HRESULT GameObj::AddComponent(_uint _numLVL, const _tchar * _protoTag, const _tchar * _tag, Component** _out, void* _arg)
{
	auto iter = find_if(umapComponent.begin(), umapComponent.end(), CTag_Finder(_tag));
	if (umapComponent.end() != iter)
		return E_FAIL;
	
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	Component* pComponent = gameInstance->CloneComponent(_numLVL, _protoTag, _arg);
	if (nullptr == pComponent)
		return E_FAIL;

	umapComponent.emplace(_tag, pComponent);

	*_out = pComponent;

	Safe_AddRef(pComponent);

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}


void Engine::GameObj::Free()
{
	for (auto& pair : umapComponent)
		Safe_Release(pair.second);

	umapComponent.clear();

	Safe_Release(dx11Device);
	Safe_Release(dx11DeviceContext);
}
