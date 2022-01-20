#include "..\public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_Objects.push_back(pGameObject);

	return S_OK;
}

_int CLayer::Tick(_double TimeDelta)
{
	_int		iProgress = 0;

	for (auto& pGameObject : m_Objects)
	{
		if (nullptr != pGameObject)
			iProgress = pGameObject->Tick(TimeDelta);			

		if (0 > iProgress)
			return -1;
	}	
	return _int(0);
}

_int CLayer::LateTick(_double TimeDelta)
{
	_int		iProgress = 0;

	auto& pGameObject = m_Objects.begin();

	for (; pGameObject != m_Objects.end();)
	{
		_int deadChk = (*pGameObject)->LateTick(TimeDelta);
		if (deadChk)
		{
			Safe_Release(*pGameObject);
			pGameObject = m_Objects.erase(pGameObject);
		}
		else {
			pGameObject++;
		}
	}
/*
	for (auto& pGameObject : m_Objects)
	{
		if (nullptr != pGameObject)
			iProgress = pGameObject->LateTick(TimeDelta);

		if (0 > iProgress)
			return -1;
	}*/
	return _int(0);
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();
	
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_Objects)
		Safe_Release(pGameObject);

	m_Objects.clear();
}
