#include "..\public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"


IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{

}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype || 
		nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototype.insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));	

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTag_Finder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
	{
		CLayer*		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else
	{
		iter->second->Add_GameObject(pGameObject);
	}

	return S_OK;
}

_int CObject_Manager::Tick(_double TimeDelta)
{
	_int		iProgress = 0;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			iProgress = Pair.second->Tick(TimeDelta);

		if (0 > iProgress)
			return -1;
	}

	return _int();
}

_int CObject_Manager::LateTick(_double TimeDelta)
{
	_int		iProgress = 0;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			iProgress = Pair.second->LateTick(TimeDelta);

		if (0 > iProgress)
			return -1;
	}

	return _int();
}

HRESULT CObject_Manager::Clear_Object_Manager(_uint iLevelIndex)
{
	if (iLevelIndex > m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

CGameObject* CObject_Manager::FindObject(const _tchar * _tag)
{
	auto	iter = find_if(m_Prototype.begin(), m_Prototype.end(), CTag_Finder(_tag));
	if (iter == m_Prototype.end())
		return nullptr;

	return iter->second;
}

list<CGameObject*>& CObject_Manager::GetObjectList(_uint _levelIdx, const _tchar* _tag)
{
	auto	iter = find_if(m_pLayers[_levelIdx].begin(), m_pLayers[_levelIdx].end(), CTag_Finder(_tag));

	if (iter == m_pLayers[_levelIdx].end())
		return list<CGameObject*>();

	return iter->second->GetObjectList();
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototype.begin(), m_Prototype.end(), CTag_Finder(pPrototypeTag));
	if(iter == m_Prototype.end())		
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototype)
		Safe_Release(Pair.second);

	m_Prototype.clear();	
}
