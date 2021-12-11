#include "..\header\ComponentManager.h"

#include "Component.h"

IMPLEMENT_SINGLETON(ComponentManager)

ComponentManager::ComponentManager()
{
}


HRESULT ComponentManager::ReserveManager(_uint _numLVL)
{
	if (nullptr != umapProtoContainer)
		return E_FAIL;

	umapProtoContainer = new COMPONENTS[_numLVL];

	numLVL = _numLVL;

	return S_OK;
}

HRESULT ComponentManager::AddProtoType(_uint _numLVL, const _tchar* _protoTag, class Component* _protoComponent)
{
	if (nullptr == umapProtoContainer ||
		nullptr != FindComponent(_numLVL, _protoTag) ||
		_numLVL >= numLVL)
		return E_FAIL;
	
	umapProtoContainer[_numLVL].insert(COMPONENTS::value_type(_protoTag, _protoComponent));

	return S_OK;
}

Component * ComponentManager::CloneComponent(_uint _numLVL, const _tchar * _tag, void* _arg)
{
	Component* protoTypeComponent = FindComponent(_numLVL, _tag);
	if (nullptr == protoTypeComponent)
		return nullptr;

	Component*  component = protoTypeComponent->Clone(_arg);
	if (nullptr == component)
		return nullptr;

	return component;
}

Component * ComponentManager::FindComponent(_uint _numLVL, const _tchar * _tag)
{
	auto iter = find_if(umapProtoContainer[_numLVL].begin(), umapProtoContainer[_numLVL].end(), CTag_Finder(_tag));
	if (umapProtoContainer[_numLVL].end() == iter)
		return nullptr;

	return iter->second;
}

void ComponentManager::Free()
{
	for (_uint i = 0; i < numLVL; i++)
	{
		for (auto& pair : umapProtoContainer[i])
			Safe_Release(pair.second);

		umapProtoContainer[i].clear();
	}

	Safe_Delete_Array(umapProtoContainer);
}
