#include "..\public\Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)	
{

}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDER eRenderID, CGameObject* pGameObject)
{
	if (nullptr == pGameObject || 
		eRenderID >= RENDER_END)
		return E_FAIL;

	m_RenderGroup[eRenderID].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroup[i])
		{
			if (nullptr != pGameObject)
				pGameObject->Render();

			Safe_Release(pGameObject);
		}
		m_RenderGroup[i].clear();
	}

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CRenderer");
		Safe_Release(pInstance);
	}		

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}
}
