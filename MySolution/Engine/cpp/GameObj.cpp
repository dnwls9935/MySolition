#include "..\header\GameObj.h"



Engine::GameObj::GameObj(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext)
{
}

Engine::GameObj::GameObj(const GameObj& rhs)
{
}

HRESULT Engine::GameObj::NativeConstructProtoType()
{
	return S_OK;
}

HRESULT Engine::GameObj::NativeConstruct()
{
	return S_OK;
}

Engine::GameObj* Engine::GameObj::Clone(void* _arg)
{
	return nullptr;
}

void Engine::GameObj::Free()
{

}
