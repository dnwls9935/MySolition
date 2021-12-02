#include "stdafx.h"
#include "..\header\Loading.h"
#include "Loader.h"
#include "Logo.h"
#include "GameInstance.h"

Client::Loading::Loading(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext)
	: Level(_dx11Direct, _dx11DirectContext)
{
}

HRESULT Client::Loading::NativeConstruct(LVL _nextLVL)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;


	nextLVL = _nextLVL;
	
	loader = Loader::Create(dx11Device, dx11DeviceContext, nextLVL);
	if (nullptr == loader)
		return E_FAIL;

	return S_OK;
}

Engine::_int Client::Loading::Tick(_double _timeDelta)
{
	if (nullptr == loader)
		return -1;

	if (0 > __super::Tick(_timeDelta))
		return -1;

	if (loader->GetThreadFinished())
	{
		if (GetKeyState(VK_SPACE) < 0)  
		{
			if (FAILED(OpenLevel()))
				return -1;
			return 0;
		}
	}
	return 0;
}

Engine::_int Client::Loading::LateTick(_double _timeDelta)
{
	return 0;
}

HRESULT Client::Loading::Render()
{
	return S_OK;
}

HRESULT Client::Loading::OpenLevel()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Level* pLevel = nullptr;

	if (nullptr == pGameInstance)
		return E_FAIL;

	switch (nextLVL)
	{
	case LVL::LVL_LOGO:
		pLevel = Logo::Create(dx11Device, dx11DeviceContext);
		break;
	}

	if (FAILED(pGameInstance->OpenLVL(pLevel)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

Client::Loading* Client::Loading::Create(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext,LVL _nextLVL)
{
	Loading* pInstance = new Loading(_dx11Direct, _dx11DirectContext);
	if (FAILED(pInstance->NativeConstruct(_nextLVL)))
	{
		MSGBOX("Failed Create Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Loading::Free()
{
	__super::Free();
	Safe_Release(loader);
}
