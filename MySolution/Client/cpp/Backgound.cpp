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

	if (FAILED(__super::AddComponent(TEXT("Component_Rendering_Proto"), TEXT("Rendering"), (Component**)&renderingCom)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(TEXT("Component_RectBuffer_Proto"), TEXT("RectBuffer"), (Component**)&bufferCom)))
		return E_FAIL;

	return S_OK;
}

Engine::_int Backgound::Tick(_double _timeDelta)
{
	if (FAILED(__super::Tick(_timeDelta)))
		return E_FAIL;

	return 0;
}

Engine::_int Backgound::LateTick(_double _timeDelta)
{
	if (FAILED(__super::LateTick(_timeDelta)))
		return E_FAIL;

	if (nullptr != renderingCom)
		renderingCom->AddRenderingGroup(Rendering::RENDERING_ID::RENDER_PRIORITY, this);

	return 0;
}

HRESULT Backgound::Render()
{/*
	bufferCom->SetUpValueOnShader("worldMatrix", &XMMatrixIdentity(), sizeof(_float)*16);
	bufferCom->SetUpValueOnShader("viewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	bufferCom->SetUpValueOnShader("projMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);*/
	bufferCom->SetUpValueOnShader("g_WorldMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	bufferCom->SetUpValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	bufferCom->SetUpValueOnShader("g_ProjMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);

	bufferCom->Render(0);

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

	Safe_Release(renderingCom);
	Safe_Release(bufferCom);
}
