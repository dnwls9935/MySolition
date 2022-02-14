#include "..\public\LightManager.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{

}

const LIGHTDESC* CLight_Manager::Get_LightDesc(_uint iIndex)
{
	auto	iter = m_Lights.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC & LightDesc, class CLight** _OutLight)
{
	CLight*		pLight = CLight::Create(pDevice, pDeviceContext, LightDesc);
	if (nullptr == pLight)
		return E_FAIL;
	
	*_OutLight = pLight;

	m_Lights.push_back(pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render_Lights()
{
	for (auto& pLight : m_Lights)
		pLight->Render();

	return S_OK;
}

HRESULT CLight_Manager::ReleaseLight()
{
	for (auto& Light : m_Lights)
		Safe_Release(Light);

	return S_OK;
}

void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight);

	m_Lights.clear();
}
