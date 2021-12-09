#include "..\header\ComponentManager.h"

#include "Component.h"

IMPLEMENT_SINGLETON(ComponentManager)

ComponentManager::ComponentManager()
{
}


HRESULT ComponentManager::AddProtoType(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext)
{
	/* 원본을 알아서 다 넣어주고 클라이언트에선 다 뽑아가자 */
	/* Component_Rendering_Proto = Rendering Component */
	umapProtoContainer.insert( COMPONENTS::value_type(TEXT("Component_Rendering_Proto"), Rendering::Create(_dx11Device, _dx11DeviceContext)));

	umapProtoContainer.insert(COMPONENTS::value_type(TEXT("Component_RectBuffer_Proto"), RectBuffer::Create(_dx11Device, _dx11DeviceContext, L"../../Client/bin/Shader/ShaderRect.hlsl")));


	return S_OK;
}

Component * ComponentManager::CloneComponent(const _tchar * _tag, void* _arg)
{
	Component* protoTypeComponent = FindComponent(_tag);
	if (nullptr == protoTypeComponent)
		return nullptr;

	Component*  component = protoTypeComponent->Clone(_arg);
	if (nullptr == component)
		return nullptr;

	return component;
}

Component * ComponentManager::FindComponent(const _tchar * _tag)
{
	auto iter = find_if(umapProtoContainer.begin(), umapProtoContainer.end(), CTag_Finder(_tag));
	if (umapProtoContainer.end() == iter)
		return nullptr;

	return iter->second;
}

void ComponentManager::Free()
{
	for (auto& pair : umapProtoContainer)
		Safe_Release(pair.second);
	
	umapProtoContainer.clear();

	//Safe_Delete_Array(umapProtoContainer);
}
