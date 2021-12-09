#include "..\header\Rendering.h"

#include "GameObj.h"

Rendering::Rendering(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext)
	: Component(_dx11Device, _dx11DeviceContext)
{
}

HRESULT Rendering::NativeConstructProtoType()
{
	return S_OK;
}

HRESULT Rendering::NativeConstruct(void * _arg)
{
	return S_OK;
}

HRESULT Rendering::AddRenderingGroup(RENDERING_ID _id, GameObj * _obj)
{
	if (RENDERING_ID::RENDER_END <= _id ||
		nullptr == _obj)
		return E_FAIL;

	renderGroup[(_uint)_id].push_back(_obj);
	Safe_AddRef(_obj);

	return S_OK;
}

HRESULT Rendering::RenderingGroup()
{
	if (FAILED(RenderingPrioirty()))
	{
		MSGBOX("Failed to Component Rendering in RenderingGroup() function");
		return E_FAIL;
	}
	if (FAILED(RenderingNoalpha()))
	{
		MSGBOX("Failed to Component Rendering in RenderingNoalpha() function");
		return E_FAIL;
	}
	if (FAILED(RenderingAlpha()))
	{
		MSGBOX("Failed to Component Rendering in RenderingAlpha() function");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Rendering::RenderingPrioirty()
{
	for (auto& iter : renderGroup[(_uint)RENDERING_ID::RENDER_PRIORITY]){
		if (FAILED(iter->Render()))
			return E_FAIL;
		else
			Safe_Release(iter);
	}

	renderGroup[(_uint)RENDERING_ID::RENDER_PRIORITY].clear();
	return S_OK;
}

HRESULT Rendering::RenderingNoalpha()
{
	for (auto& iter : renderGroup[(_uint)RENDERING_ID::RENDER_NOALPHA]) {
		if (FAILED(iter->Render()))
			return E_FAIL;
		else
			Safe_Release(iter);
	}

	renderGroup[(_uint)RENDERING_ID::RENDER_NOALPHA].clear();
	return S_OK;
}

HRESULT Rendering::RenderingAlpha()
{
	for (auto& iter : renderGroup[(_uint)RENDERING_ID::RENDER_ALPHA]) {
		if (FAILED(iter->Render()))
			return E_FAIL;
		else
			Safe_Release(iter);
	}

	renderGroup[(_uint)RENDERING_ID::RENDER_ALPHA].clear();
	return S_OK;
}

Component * Rendering::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
{
	Rendering* pInstance = new Rendering(_dx11Device, _dx11DeviceContext);

	if (FAILED(pInstance->NativeConstructProtoType()))
	{
		MSGBOX("Failed to Create Component Proto Rendering ");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component * Rendering::Clone(void * _arg)
{
	AddRef();

	return this;
}

void Rendering::Free()
{
	__super::Free();

	for (_uint i = 0; i < (_uint)RENDERING_ID::RENDER_END; i++)
	{
		for (auto& iter : renderGroup[i])
			Safe_Release(iter);

		renderGroup[i].clear();
	}
}
