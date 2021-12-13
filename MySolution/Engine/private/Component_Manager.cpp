#include "..\public\Component_Manager.h"
#include "Component.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{

}

HRESULT CComponent_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pComponents)
		return E_FAIL;

	m_pComponents = new COMPONENTS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == pPrototype ||
		nullptr != Find_Prototype_Component(iLevelIndex, pPrototypeTag) || 
		iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	m_pComponents[iLevelIndex].insert(COMPONENTS::value_type(pPrototypeTag, pPrototype));
	
	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	CComponent*	pPrototype = Find_Prototype_Component(iLevelIndex, pPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	CComponent*	pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}



CComponent * CComponent_Manager::Find_Prototype_Component(_uint iLevelIndex, const _tchar * pComponentTag)
{
	auto	iter = find_if(m_pComponents[iLevelIndex].begin(), m_pComponents[iLevelIndex].end(), CTag_Finder(pComponentTag));
	if (iter == m_pComponents[iLevelIndex].end())
		return nullptr;

	return iter->second;	
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pComponents[i])
			Safe_Release(Pair.second);

		m_pComponents[i].clear();
	}

	Safe_Delete_Array(m_pComponents);

}
