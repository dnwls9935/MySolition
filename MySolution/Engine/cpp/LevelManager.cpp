#include "..\header\LevelManager.h"
#include "Level.h"
#include "GameObjManager.h"

IMPLEMENT_SINGLETON(LevelManager)

Engine::LevelManager::LevelManager()
{

}

HRESULT Engine::LevelManager::OpenLVL(class Level* pLVL)
{
	if (nullptr == pLVL)
		return E_FAIL;

	GameObjManager* gameObjManager = GET_INSTANCE(GameObjManager);

	if(nullptr != currentLVL) gameObjManager->ClearObj(currentLVL->GetLevelID());

	Safe_Release(currentLVL);
	currentLVL = pLVL;
	RELEASE_INSTANCE(GameObjManager);

	return S_OK;
}

Engine::_int Engine::LevelManager::Tick(_double _timeDelta)
{
	if (nullptr == currentLVL)
		return -1;

	return currentLVL->Tick(_timeDelta);
}

Engine::_int Engine::LevelManager::LateTick(_double _timeDelta)
{
	if (nullptr == currentLVL)
		return -1;

	return currentLVL->LateTick(_timeDelta);
}

HRESULT Engine::LevelManager::Render()
{
	if (nullptr == currentLVL)
		return -1;

	return currentLVL->Render();
}

void Engine::LevelManager::Free()
{
	Safe_Release(currentLVL);
}
