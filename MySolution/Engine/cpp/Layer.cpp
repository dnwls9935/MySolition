#include "..\header\Layer.h"
#include "GameObj.h"

Layer::Layer()
{
}

HRESULT Layer::AddObj(GameObj * _obj)
{
	if (nullptr == _obj)
		return E_FAIL;

	listObj.push_back(_obj);

	return S_OK;
}

_int Layer::Tick(_double _timeDelta)
{
	_int pProgress = 0;

	for (auto iter : listObj)
	{
		if (nullptr != iter)
			pProgress = iter->Tick(_timeDelta);

		if (0 > pProgress)
			return -1;
	}
		
	return pProgress;
}

_int Layer::LateTick(_double _timeDelta)
{
	_int pProgress = 0;

	for (auto iter : listObj)
	{
		if (nullptr != iter)
			pProgress = iter->LateTick(_timeDelta);

		if (0 > pProgress)
			return -1;
	}

	return pProgress;
}

Layer * Layer::Create()
{
	Layer* pInstance = new Layer();
	return pInstance;
}

void Layer::Free()
{
	for (auto& iter : listObj)
		Safe_Release(iter);

	listObj.clear();
}
