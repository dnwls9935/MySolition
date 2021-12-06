#include "stdafx.h"
#include "..\header\Backgound.h"
#include "GameInstance.h"

Backgound::Backgound(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: GameObj(_dx11Device, _dx11DeviceContext)
{
}

Backgound::Backgound(const Backgound & rhs)
	: GameObj(rhs)
{
}

HRESULT Backgound::NativeConstructProtoType()
{
	if (FAILED(__super::NativeConstructProtoType()))
		return E_FAIL;

	return S_OK;
}

HRESULT Backgound::NativeConstruct(void* _arg)
{
	if (FAILED(__super::NativeConstruct(_arg)))
		return E_FAIL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	return S_OK;
}

Engine::_int Backgound::Tick(_double _timeDelta)
{
	return 0;
}

Engine::_int Backgound::LateTick(_double _timeDelta)
{
	return 0;
}

HRESULT Backgound::AddComponent()
{
	return S_OK;
}

Backgound * Backgound::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
{
	Backgound* pInstance = new Backgound(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstructProtoType()))
	{
		MSGBOX("Failed To Create Background");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

Backgound* Backgound::Clone(void* _arg)
{
	Backgound* pInstance = new Backgound(*this);
	if (FAILED(pInstance->NativeConstruct(_arg)))
	{
		MSGBOX("Failed To Clone Background");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Backgound::Free()
{
	__super::Free();
}
