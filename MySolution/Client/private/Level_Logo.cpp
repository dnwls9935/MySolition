#include "stdafx.h"
#include "..\public\Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"


CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Logo::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	return S_OK;
}

_int CLevel_Logo::Tick(_double TimeDelta)
{
	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	if (GetKeyState(VK_SPACE) < 0)
	{
		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_GAMEPLAY), LEVEL_GAMEPLAY)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return -1;
		}

		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}

	

	return _int(0);
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLevel_Logo*	pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();


}
