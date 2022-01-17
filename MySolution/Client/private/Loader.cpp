#include "stdafx.h"
#include "..\public\Loader.h"
#include "Terrain.h"
#include "Camera_Dynamic.h"
#include "Sky.h"
#include "Player.h"
#include "GameInstance.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

_uint APIENTRY Thread_Main(void* pArg)
{
	CLoader*		pLoader = (CLoader*)pArg;
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	EnterCriticalSection(&pLoader->Get_CriticalSection());

	HRESULT			hr = 0;

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		hr = pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		hr = pLoader->Loading_ForGamePlay();
		break;
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&pLoader->Get_CriticalSection());
	CoUninitialize();
	return 0;
}


HRESULT CLoader::NativeConstruct(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_CS);

	/* 스레드를 생성한다. -> 진입점함수를 정의한다. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogo()
{
	/* 컴포넌트 원형을 생성한다. */
	wsprintf(m_szLoading, TEXT("버퍼를 생성한다. "));



	wsprintf(m_szLoading, TEXT("텍스쳐를 생성한다. "));


	/* 객체원형을 생성한다. */
	


	wsprintf(m_szLoading, TEXT("로딩이 완료되었습니다. "));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	/* 컴포넌트 원형 생성한다. */
	wsprintf(m_szLoading, TEXT("버퍼를 생성한다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Terrain.hlsl"), 200, 200))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Cube.hlsl")))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("텍스쳐를 생성한다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Cube"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/burger%d.dds"), 4))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Snow_256_Dif.tga")))))
		return E_FAIL;


	_matrix			PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"), CModel::Create(m_pDevice, m_pDeviceContext, "../Bin/Resources/Meshes/Zero/", "ZeroHandTest.FBX", TEXT("../Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_ANIM))))
		return E_FAIL;



	/* 객체원형을 생성한다. */
	wsprintf(m_szLoading, TEXT("객체를 생성한다. "));	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera"), CCamera_Dynamic::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cube"), Sky::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	wsprintf(m_szLoading, TEXT("로딩이 완료되었습니다. "));

	m_isFinished = true;
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CLoader::Print_LoaderText()
{
	SetWindowText(g_hWnd, m_szLoading);
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, LEVEL eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))
	{
		MSGBOX("Failed to Creating CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);	
	DeleteCriticalSection(&m_CS);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);		

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
