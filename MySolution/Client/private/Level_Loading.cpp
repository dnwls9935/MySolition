#include "stdafx.h"
#include "..\public\Level_Loading.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{

}

HRESULT CLevel_Loading::NativeConstruct(LEVEL eNextLevel)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

_int CLevel_Loading::Tick(_double TimeDelta)
{
	if (nullptr == m_pLoader)
		return -1;

	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (true == m_pLoader->Get_Finished())
	{
		if (GetKeyState(VK_RETURN) < 0)
		{
			if (FAILED(Open_Level()))
				return -1;

			return 0;
		}		
	}

	return _int(0);
}

HRESULT CLevel_Loading::Render()
{
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pLoader->Print_LoaderText();


	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.GameObject_BackGround */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_LOADING, pLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Loading::Open_Level()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CLevel*		pLevel = nullptr;
	LEVEL			nextLevel = LEVEL::LEVEL_END;
	switch (m_eNextLevel)
	{
	case LEVEL_LOGO: 
 		pLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
		nextLevel = LEVEL::LEVEL_GAMEPLAY;
		break;
	case LEVEL_GAMEPLAY:
		pLevel = CLevel_GamePlay::Create(m_pDevice, m_pDeviceContext);
		nextLevel = LEVEL::LEVEL_LOGO;
		break;
	}

	if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pLevel, nextLevel)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, LEVEL eNextLevel)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSGBOX("Failed to Creating CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	

	Safe_Release(m_pLoader);
}
