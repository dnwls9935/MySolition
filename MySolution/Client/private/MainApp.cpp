#include "stdafx.h"
#include "..\public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{	

	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::NativeConstruct()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hWnd, LEVEL_END, CGraphic_Device::MODE_WIN, g_iWinCX, g_iWinCY, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Component_Prototype()))
		return E_FAIL;
	if (FAILED(Ready_GameObject_Prototype()))
		return E_FAIL;

	if (FAILED(SetUp_StartLevel(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

_int CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return -1;

	m_pGameInstance->Tick_Engine(TimeDelta);



	return _int();
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Clear_BackBuffer_View(XMFLOAT4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
		return E_FAIL;	

	/* 내 게임을 구성하는객체들의 렌더함수르리 호출하낟. */
	if (FAILED(m_pRenderer->Draw_RenderGroup()))
		return E_FAIL;

	/* 내 게임내의 기타등ㄷ응을 렌더링하낟. */
	if (FAILED(m_pGameInstance->Render_Engine()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::SetUp_StartLevel(LEVEL eLevel)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	HRESULT		hr = 0;

	switch (eLevel)
	{
	case LEVEL_LOGO: case LEVEL_GAMEPLAY:
		hr = m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eLevel));
		break;

	default:
		return E_FAIL;
	}	

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Component_Prototype()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Rect.hlsl")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_GameObject_Prototype()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}	

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pRenderer);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);	
	
	CGameInstance::Release_Engine();
}

