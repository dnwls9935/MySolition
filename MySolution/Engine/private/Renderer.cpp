#include "..\public\Renderer.h"
#include "GameObject.h"
#include "RenderTargetManager.h"
#include "LightManager.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)	
	, m_RenderTargetManager(RenderTargetManager::GetInstance())
{
	Safe_AddRef(m_RenderTargetManager);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (nullptr == m_RenderTargetManager)
		return E_FAIL;

	_uint		ViewPortIndex = 1;
	D3D11_VIEWPORT ViewPortDesc;
	
	m_pDeviceContext->RSGetViewports(&ViewPortIndex, &ViewPortDesc);

	RenderTargetManager::RTDESC RTDesc;
	ZeroMemory(&RTDesc, sizeof(RenderTargetManager::RTDESC));
	RTDesc.Width = (_uint)ViewPortDesc.Width;
	RTDesc.Height = (_uint)ViewPortDesc.Height;
	RTDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	RTDesc.Color = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(m_RenderTargetManager->AddRenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), RTDesc)))
		return E_FAIL;

	RTDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RTDesc.Color = _float4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_RenderTargetManager->AddRenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), RTDesc)))
		return E_FAIL;
/*
	RTDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	RTDesc.Color = _float4(0.f, 1.f, 0.f, 1.f);
	if (FAILED(m_RenderTargetManager->AddRenderTarget(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), RTDesc)))
		return E_FAIL;*/
/*
	if (FAILED(m_RenderTargetManager->AddMRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_RenderTargetManager->AddMRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;*/
	/*if (FAILED(m_RenderTargetManager->AddMRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;*/


#ifdef _DEBUG
	//RenderTargetManager::DBDESC DBDesc;
	//ZeroMemory(&DBDesc, sizeof(RenderTargetManager::DBDESC));
	//DBDesc.X = 0.f;
	//DBDesc.Y = 0.f;
	//DBDesc.SizeX = 200.f;
	//DBDesc.SizeY = 200.f;
	//if (FAILED(m_RenderTargetManager->ReadyDebugBuffer(TEXT("Target_Diffuse"), DBDesc)))
	//	return E_FAIL;

	//ZeroMemory(&DBDesc, sizeof(RenderTargetManager::DBDESC));
	//DBDesc.X = 0.f;
	//DBDesc.Y = 200.f;
	//DBDesc.SizeX = 200.f;
	//DBDesc.SizeY = 200.f;
	//if (FAILED(m_RenderTargetManager->ReadyDebugBuffer(TEXT("Target_Normal"), DBDesc)))
	//	return E_FAIL;

	/*ZeroMemory(&DBDesc, sizeof(RenderTargetManager::DBDESC));
	DBDesc.X = 0.f;
	DBDesc.Y = 400.f;
	DBDesc.SizeX = 200.f;
	DBDesc.SizeY = 200.f;
	if (FAILED(m_RenderTargetManager->ReadyDebugBuffer(TEXT("Target_Shade"), DBDesc)))
		return E_FAIL;*/
#endif // _DEBUG



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
	if (FAILED(RenderPriority()))
		return E_FAIL;
	if (FAILED(RenderNonAlpha()))
		return E_FAIL;

	if (FAILED(RenderLightAcc()))
		return E_FAIL;

	if (FAILED(RenderAlpha()))
		return E_FAIL;
	if (FAILED(RenderUI()))
		return E_FAIL;

#ifdef _DEBUG
	//if (FAILED(m_RenderTargetManager->RenderDebugBuffer(TEXT("MRT_Deferred"))))
	//	return E_FAIL;
	//if (FAILED(m_RenderTargetManager->RenderDebugBuffer(TEXT("MRT_LightAcc"))))
	//	return E_FAIL;
#endif // _DEBUG


	return S_OK;
}

HRESULT CRenderer::RenderPriority()
{
	for (auto& pGameObject : m_RenderGroup[RENDER::RENDER_PRIORITY])
	{
		if(nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER::RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::RenderNonAlpha()
{
	//if (nullptr == m_RenderTargetManager)
	//	return E_FAIL;

	//if (FAILED(m_RenderTargetManager->BeginMRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
	//	return E_FAIL;

	for (auto& pGameObject : m_RenderGroup[RENDER::RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER::RENDER_NONALPHA].clear();

	//if (FAILED(m_RenderTargetManager->EndMRT(m_pDeviceContext)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::RenderAlpha()
{
	for (auto& pGameObject : m_RenderGroup[RENDER::RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER::RENDER_ALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::RenderUI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER::RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER::RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::RenderLightAcc()
{
	//LightManager*		LightMGR = GET_INSTANCE(LightManager);

	//if (FAILED(m_RenderTargetManager->BeginMRT(m_pDeviceContext, TEXT("MRT_LightAcc"))))
	//	return E_FAIL;

	//if (FAILED(LightMGR->RenderLights()))
	//	return E_FAIL;

	//if (FAILED(m_RenderTargetManager->EndMRT(m_pDeviceContext)))
	//	return E_FAIL;


	//RELEASE_INSTANCE(LightManager);
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

	Safe_Release(m_RenderTargetManager);

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}
}
