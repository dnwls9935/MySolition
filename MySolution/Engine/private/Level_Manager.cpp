#include "..\public\Level_Manager.h"

#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel * pOpenLevel, _uint nextLevel)
{
	if (nullptr == pOpenLevel)
		return E_FAIL;

	if (0 != m_iCurrentLevelID)
	{
		if (FAILED(Clear_Managers()))
			return E_FAIL;
	}

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pOpenLevel;	

	m_iCurrentLevelID = iLevelIndex;

	m_iNextLevelID = nextLevel;

	return S_OK;
}

_int CLevel_Manager::Tick(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return -1;

	return m_pCurrentLevel->Tick(TimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

_uint CLevel_Manager::GetNextLevelID()
{
	if (nullptr == m_pCurrentLevel)
		return 99999999;

	return m_iNextLevelID;
}

HRESULT CLevel_Manager::Clear_Managers()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(pGameInstance->Clear_Engine(m_iCurrentLevelID)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
