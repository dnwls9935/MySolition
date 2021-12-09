#include "stdafx.h"
#include "..\header\Logo.h"

Logo::Logo(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext)
	: Level(_dx11Direct, _dx11DirectContext)
{
}

HRESULT Client::Logo::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	levelID = (_uint)LVL::LVL_LOGO;

	return S_OK;
}

_int Client::Logo::Tick(_double _timeDelta)
{
	if (0 > (__super::Tick(_timeDelta)))
		return -1;

	return 0;
}

_int Client::Logo::LateTick(_double _timeDelta)
{
	if (0 > (__super::LateTick(_timeDelta)))
		return -1;

	return 0;
}

HRESULT Client::Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}


Logo* Logo::Create(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext)
{
	Logo* pInstance = new Logo(_dx11Direct,_dx11DirectContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Logo::Free()
{
	__super::Free();
}
