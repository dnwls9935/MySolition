#include "..\public\LightManager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(LightManager);

LightManager::LightManager()
{
}

const LIGHTDESC * LightManager::GetLightDesc(_uint Index)
{
	auto iter = m_Lights.begin();
	for (_uint i = 0; i < Index; i++)
		iter++;

	return (*iter)->GetLightDesc();
}

HRESULT LightManager::AddLight(ID3D11Device * _Device, ID3D11DeviceContext * _DeviceContext, const LIGHTDESC & _LightDesc)
{
	Light*		pLight = Light::Create(_Device, _DeviceContext, _LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT LightManager::RenderLights()
{
	for (auto& pLight : m_Lights)
	{
		if (FAILED(pLight->Render()))
			return E_FAIL;
	}

	return S_OK;
}

void LightManager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
