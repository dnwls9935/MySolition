#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "Camera_Dynamic.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Player.h"
#include "Terrain.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_GamePlay::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;
	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_Terrain"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_SkyBox(TEXT("Layer_SkyBox"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	HANDLE hFile = CreateFile(L"../Bin/Data/Finally.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;
	if (FAILED(LoadData(hFile)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	CloseHandle(hFile);
	return S_OK;
}

_int CLevel_GamePlay::Tick(_double TimeDelta)
{
	POINT pt{g_iWinCX/ 2, g_iWinCY/2};
	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);


	if (0 > (__super::Tick(TimeDelta)))
		return -1;

	return _int(0);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC			LightDesc;
	/*ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_POINT;

	LightDesc.vPosition = _float3(5.f, 3.f, 5.f);
	LightDesc.fRange = 100.f;

	LightDesc.vDiffuse = _float4(1.f, 0.0f, 0.0f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;*/


	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_POINT;

	LightDesc.vPosition = _float3(100.f, 10.f, 100.f);
	LightDesc.fRange = 150.f;

	LightDesc.vDiffuse = _float4(1.f, 1.f, 0.9f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;

	/*LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float3(1.f, -1.f, 1.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Enemy(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_MonsterTest"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_SkyBox(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Cube"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.0f, 3.0f, -2.5f);
	CameraDesc.vAt = _float3(0.0f, 0.0f, 5.0f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(60.f);
	CameraDesc.fAspect = _float(g_iWinCX) / g_iWinCY;
	CameraDesc.fNear = 0.01f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);


	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera"), &CameraDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI_PlayerHP"));


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::LoadData(HANDLE& hFile)
{
	if (FAILED(LoadNavigation(hFile)))
		return E_FAIL;

	if (FAILED(LoadTerrain(hFile)))
		return E_FAIL;

	if (FAILED(LoadEnvironment(hFile)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::LoadTerrain(HANDLE & hFile)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	list<CGameObject*>& pGameObjectList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"));
	CTerrain* pTerrain = static_cast<CTerrain*>(pGameObjectList.front());
	

	if (nullptr == pTerrain)
		return E_FAIL;

	_uint x = static_cast<CVIBuffer_Terrain*>(pTerrain->GetComponent(TEXT("Com_VIBuffer")))->GetVerticesX();
	_uint z = static_cast<CVIBuffer_Terrain*>(pTerrain->GetComponent(TEXT("Com_VIBuffer")))->GetVerticesZ();
	_float y = 0.f;
	DWORD dwByte;
	for (_uint i = 0; i < z; i++)
	{
		for (_uint j = 0; j < x; j++)
		{
			_int		iIndex = i * x + j;
			ReadFile(hFile, &y, sizeof(_float), &dwByte, nullptr);
			//pTerrain->SetVerticeY(iIndex, y);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::LoadNavigation(HANDLE & hFile)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	DWORD dwByte = 0;
	_int size = 0;
	ReadFile(hFile, &size, sizeof(_int), &dwByte, nullptr);

	for (_int i = 0; i < size; i++)
	{
		_float3 Points[3];
		_float3 Point;
		ReadFile(hFile, &Point, sizeof(_float3), &dwByte, nullptr);
		Points[0] = Point;

		ReadFile(hFile, &Point, sizeof(_float3), &dwByte, nullptr);
		Points[1] = Point;

		ReadFile(hFile, &Point, sizeof(_float3), &dwByte, nullptr);
		Points[2] = Point;
		static_cast<Navigation*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Terrain")).front()->GetComponent(TEXT("Com_Navigation")))->AddCell(Points, TEXT("../Bin/ShaderFiles/Shader_TriangleToLine.hlsl"));
	}
	static_cast<Navigation*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Terrain")).front()->GetComponent(TEXT("Com_Navigation")))->SetUpNeighbor();
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::LoadEnvironment(HANDLE & hFile)
{
	DWORD dwByte = 0;

	while (TRUE)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CGameObject::TOOLOBJDESC pToolObjDesc;
		ZeroMemory(&pToolObjDesc, sizeof(pToolObjDesc));
		ReadFile(hFile, &pToolObjDesc, sizeof(CGameObject::TOOLOBJDESC), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		switch (pToolObjDesc.m_Type)
		{
		case CGameObject::OBJTYPE_ID::ENVIRONMENT:
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Envrionment"), TEXT("Prototype_GameObject_Model_EnvironmentObject"), &pToolObjDesc)))
				return E_FAIL;
		}
			break;
		case CGameObject::OBJTYPE_ID::INTERACTION:
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interact"), pToolObjDesc.m_ObjTag, &pToolObjDesc)))
				return E_FAIL;
		}
			break;
		case CGameObject::OBJTYPE_ID::PLAYER:
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Model_Player"), &pToolObjDesc)))
				return E_FAIL;
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Model_SMG"))))
				return E_FAIL;
		}
			break;
		case CGameObject::OBJTYPE_ID::ENEMY:
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), pToolObjDesc.m_ObjTag, &pToolObjDesc)))
				return E_FAIL;
		}
			break;
		case CGameObject::OBJTYPE_ID::BOSS:
		{
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), pToolObjDesc.m_ObjTag, &pToolObjDesc)))
				return E_FAIL;
		}
			break;
		case CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT:
		{
			if (0 == lstrcmp(pToolObjDesc.m_ObjTag, TEXT("Prototype_GameObject_Model_GlacialFlowWall")))
			{
				if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Wall"), pToolObjDesc.m_ObjTag, &pToolObjDesc)))
					return E_FAIL;
			}
			else {
				if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Envrionment"), TEXT("Prototype_GameObject_Model_EnvironmentObject"), &pToolObjDesc)))
					return E_FAIL;
			}
		}
			break;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLevel_GamePlay*	pInstance = new CLevel_GamePlay(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Creating CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();


}
