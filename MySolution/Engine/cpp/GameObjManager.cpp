#include "..\header\GameObjManager.h"
#include "Layer.h"
#include "GameObj.h"

IMPLEMENT_SINGLETON(GameObjManager)

HRESULT GameObjManager::ReserveManager(_uint _lvlNum)
{
	umapGameObjContainer = new LAYERS[_lvlNum];

	levelNum = _lvlNum;

	return S_OK;
}

HRESULT GameObjManager::Add_ProtoObj(const _tchar * _tag, GameObj* _obj)
{
	if (nullptr == _obj || nullptr != FindProto(_tag))
		return E_FAIL;

	umapProtoContainer.insert(PROTOTYPE::value_type(_tag, _obj));

	return S_OK;
}

HRESULT GameObjManager::Add_GameObj(_uint _lvl, const _tchar * _layerTag, const _tchar * _protoTag, void* _arg)
{
	if (_lvl >= levelNum)
		return E_FAIL;

	GameObj* protoObj = FindProto(_protoTag);
	if (nullptr == protoObj)
		return E_FAIL;

	GameObj* cloneProtoObj = protoObj->Clone(_arg);
	if (nullptr == cloneProtoObj)
		return E_FAIL;

	auto pair = find_if(umapGameObjContainer->begin(), umapGameObjContainer->end(), CTag_Finder(_layerTag));
	if (umapGameObjContainer->end() == pair)
	{
		Layer* pLayer = Layer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->AddObj(cloneProtoObj)))
			return E_FAIL;

		umapGameObjContainer->insert(LAYERS::value_type(_layerTag, pLayer));
	}
	else {
		if (FAILED(pair->second->AddObj(cloneProtoObj)))
			return E_FAIL;
	}

	return S_OK;
}

_int GameObjManager::Tick(_double _timeDelta)
{
	_int pProgress = 0;
	for (_uint i = 0; i < levelNum; i++)
	{
		for (auto& pair : umapGameObjContainer[i])
			pProgress = pair.second->Tick(_timeDelta);

		if (0 > pProgress)
			return -1;
	}

	return _int(0);
}

_int GameObjManager::LateTick(_double _timeDelta)
{
	_int pProgress = 0;
	for (_uint i = 0; i < levelNum; i++)
	{
		for (auto& pair : umapGameObjContainer[i])
			pProgress = pair.second->LateTick(_timeDelta);

		if (0 > pProgress)
			return -1;
	}

	return _int(0);
}

GameObj * GameObjManager::FindProto(const _tchar * _protoTag)
{
	auto pair = find_if(umapProtoContainer.begin(), umapProtoContainer.end(), CTag_Finder(_protoTag));

	if (umapProtoContainer.end() == pair)
		return nullptr;

	return pair->second;
}

void GameObjManager::Free()
{
	for (_uint i = 0; i < levelNum; i++)
	{
		for (auto pair : umapGameObjContainer[i])
			Safe_Release(pair.second);

		umapGameObjContainer[i].clear();
	}
	Safe_Delete_Array(umapGameObjContainer);

	for (auto& pair : umapProtoContainer)
		Safe_Release(pair.second);

	umapProtoContainer.clear();
}
