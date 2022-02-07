
// ToolView.cpp : CToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "GameInstance.h"
#include "ToolDefines.h"
#include "Engine_Defines.h"

#include "ToolDoc.h"
#include "ToolView.h"
#include "ToolTerrain.h"
#include "ToolCamera.h"
#include "ToolRect.h"
#include "ToolObject.h"
#include "TapMap.h"
#include "TapUI.h"
#include "ToolUI.h"
#include "UIObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CToolView 생성/소멸
HWND g_hWnd;
HINSTANCE g_hInst;
CToolView::CToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CToolView::~CToolView()
{
	Safe_Release(dx11Device);
	Safe_Release(dx11DeviceContext);
	RELEASE_INSTANCE(CGameInstance);
	CGameInstance::Release_Engine();
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CToolView 그리기

HRESULT CToolView::RenderBegin()
{
	if (nullptr == gameInstance)
		return E_FAIL;

	if (FAILED(gameInstance->Clear_BackBuffer_View(XMFLOAT4(0.f, 0.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(gameInstance->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolView::RenderEnd()
{
	if (FAILED(gameInstance->Present()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolView::ReadyProtoAll()
{
	/* 컴포넌트 원형 생성한다. */
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/ShaderFiles/Shader_Terrain_Tool.hlsl"),200, 200))))
		return E_FAIL;
	/* For.Prototype_Component_Transform */
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Transform"), CTransform::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/ShaderFiles/Shader_Rect.hlsl")))))
		return E_FAIL;
	/* For.Prototype_Component_Navigation */
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Navigation"), Navigation::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	/* For.Prototype_Component_Collider */
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Collider_Sphere"), CCollider::Create(dx11Device, dx11DeviceContext, CCollider::TYPE::TYPE_SPHERE))))
		return E_FAIL;
	
	
	if (FAILED(ReadyTexture()))
		return E_FAIL;

	_matrix			PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f));
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Zero"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Zero/", "Zero.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f));
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BugMorph"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/BugMorph/", "BugMorph.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;

	//PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(90.f)); // ANIM상태
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f)); // STATIC상태
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_PrimeBeast"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/PrimeBeast/", "PrimeBeast.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	PivotMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationX(XMConvertToRadians(90.f)); // STATIC상태
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BossPrimeBeast"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/PrimeBeast/", "PrimeBeast.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Dahl_WeaponLocker"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/ItemBox/", "Dahl_WeaponLocker.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_DahlEpicCrate"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/ItemBox/", "DahlEpicCrate.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.f));
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HyperionChest"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/ItemBox/", "HyperionChest.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;


	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HandsomeJack"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "HandsomeJack.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HandsomeJackGoldenStatue"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "HandsomeJackGoldlStatue.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_SnowDrift"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "GlacialTest.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_SnowPile0"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Snowpile00.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_SnowPile1"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Snowpile01.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_SnowPile2"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Snowpile02.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_SnowPile3"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Snowpile03.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_SnowPile4"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Snowpile04.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_GlacialFlow_Straight0"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "GlacialFlow_Straight0.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_GlacialFlow_Straight1"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "GlacialFlow_Straight1.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL; 
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_ArenaSign"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "ArenaSign.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_AutoJunk"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "AutoJunk.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_AutoJunk2"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "AutoJunk2.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BanditFlag"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BanditFlag.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BanditHouse"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BanditHouse.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BanditHouseDestory"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BanditHouseDestory.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BrokenCrapTrapA"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BrokenCrapTrapA.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BrokenCrapTrapB"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BrokenCrapTrapB.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BrokenCrapTrapC"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BrokenCrapTrapC.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BrokenCrapTrapD"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BrokenCrapTrapD.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BrokenCrapTrapE"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BrokenCrapTrapE.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_BrokenCrapTrapF"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "BrokenCrapTrapF.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_DoorWayLR"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "DoorWayLR.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_FlagPole"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "FlagPole.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_FlagPoleOptimizer"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "FlagPoleOptimizer.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Furace"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Furace.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_IceBlockLarge"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "IceBlockLarge.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_SkyBoxMountain"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "SkyBoxMountain.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_SnowDriftStatic"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "SnowDrift.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Tire"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Tire.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_TurbinBase"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "TurbinBase.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_TurbinFan"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "TurbinFan.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;

	/*2차*/
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_ComShip"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "ComShip.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_ComShipBrace"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "ComShipBrace.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_DeadMaleVillagerBody"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "DeadMaleVillagerBody.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_DeadMaleVillagerBody1"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "DeadMaleVillagerBody1.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_DeathBandit0"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "DeathBandit0.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_DeathBandit1"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "DeathBandit1.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_FrozenWaterFall"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "FrozenWaterFall.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_GlacialFlowWall"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "GlacialFlowWall.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_IceFalls"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "IceFalls.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Wire0"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Wire0.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Wire1"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Wire1.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Wire2"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Wire2.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Wire3"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "Wire3.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;


	///* 객체원형을 생성한다. */
	/*		Static	*/
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), ToolTerrain::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI"), ToolUI::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIObject"), UIObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dyanmic_Camera"), ToolCamera::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BugMorph"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_PrimeBeast"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BossPrimeBeast"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Zero"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Dahl_WeaponLocker"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_DahlEpicCrate"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HyperionChest"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;


	/*		Dynamic		*/
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HandsomeJack"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HandsomeJackGoldenStatue"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SnowDrift"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SnowPile0"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SnowPile1"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SnowPile2"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SnowPile3"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SnowPile4"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_GlacialFlow_Straight0"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_GlacialFlow_Straight1"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;

	/* Static */
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_ArenaSign"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_AutoJunk"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_AutoJunk2"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BanditFlag"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BanditHouse"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BanditHouseDestory"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BrokenCrapTrapA"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BrokenCrapTrapB"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BrokenCrapTrapC"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BrokenCrapTrapD"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BrokenCrapTrapE"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BrokenCrapTrapF"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_DoorWayLR"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_FlagPole"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_FlagPoleOptimizer"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Furace"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_IceBlockLarge"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SkyBoxMountain"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_SnowDriftStatic"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Tire"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_TurbinBase"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_TurbinFan"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	//
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_ComShip"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_ComShipBrace"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_DeadMaleVillagerBody"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;																				
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_DeadMaleVillagerBody1"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_DeathBandit0"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_DeathBandit1"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_FrozenWaterFall"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_GlacialFlowWall"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_IceFalls"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Wire0"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Wire1"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Wire2"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Wire3"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;



	/* UI 관련 */


	return S_OK;
}

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	gameInstance->Update_TimeDelta(TEXT("Timer_Default"));
	TimerAcc += gameInstance->Get_TimeDelta(TEXT("Timer_Default"));

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
	if (TimerAcc >= 1.0 / 60.0f)
	{
		TimerAcc = 0.0;

		gameInstance->Update_TimeDelta(TEXT("Timer_60"));

		if (0 > gameInstance->Tick_Engine_For_Tool(gameInstance->Get_TimeDelta(TEXT("Timer_60"))))
			MSGBOX(L"Failed to Tick Engine");

		if (FAILED(RenderBegin()))
			MSGBOX(L"Failed to Render Begin");
		// 렌더러 redering
		if (FAILED(m_pRenderer->Draw_RenderGroup()))
			MSGBOX(L"Failed to Draw RenderGroup");

		if (FAILED(RenderEnd()))
			MSGBOX(L"Failed to Render End");
	}


	Invalidate(FALSE);
}


// CToolView 인쇄

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CToolView 진단

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView 메시지 처리기


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));

	RECT rcMain = {};
	m_mainFrm->GetWindowRect(&rcMain);

	SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);
	RECT rcView{};
	GetClientRect(&rcView);

	int iGapX = rcMain.right - rcView.right;
	int iGapY = rcMain.bottom - rcView.bottom;
	m_mainFrm->SetWindowPos(nullptr, 0, 0, g_WIN_WIDHT + iGapX, g_WIN_HEIGHT + iGapY, SWP_NOMOVE);

	g_hWnd = m_hWnd;
	g_hInst = AfxGetInstanceHandle();

	gameInstance = GET_INSTANCE(CGameInstance);

	gameInstance->Initialize_Engine(g_hInst, g_hWnd, 3, CGraphic_Device::WINMODE::MODE_WIN, g_WIN_WIDHT, g_WIN_HEIGHT, &dx11Device, &dx11DeviceContext);


	if (FAILED(gameInstance->Ready_Timer(TEXT("Timer_Default"))))
	{
		MSGBOX(L"Failed to Ready Timer");
	}
	if (FAILED(gameInstance->Ready_Timer(TEXT("Timer_60"))))
	{
		MSGBOX(L"Failed to Ready Timer");
	}

	if (FAILED(ReadyProtoAll()))
		MSGBOX(L"Failed to ReadyProtoAll");
	/* For.Prototype_Component_Renderer */
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(dx11Device, dx11DeviceContext))))
	{
		MSGBOX(L"Failed to Ready Renderer Component");
	}

	if (FAILED(ReadyLayer()))
	{
		MSGBOX(L"Failed to Ready Layer");
	}

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

HRESULT CToolView::ReadyLayer()
{
	if (FAILED(ReadyCameraLayer(TEXT("Camera"))))
		return E_FAIL;
	
	if (FAILED(ReadyTerrainLayer(TEXT("Terrain"))))
		return E_FAIL;

	if (FAILED(ReadyObjectLayer(TEXT("Object"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CToolView::ReadyTexture()
{
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/Terrain/Snow_256_Dif.tga")))))
		return E_FAIL;


	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Ammo"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/Ammo.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_Ammo"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_GrenadeBar"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/GrenadeBar.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_GrenadeBar"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HeartCross"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HeartCross.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HeartCross"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_I2CD"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_I2CD.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_I2CD"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_I2D3"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_I2D3.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_I2D3"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_I3AD"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_I3AD.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_I3AD"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_I3B3"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_I3B3.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_I3B3"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_I3B9"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_I3B9.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_I3B9"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_I15D"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_I15D.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_I15D"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_I101"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_I101.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_I101"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_I316"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_I316.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_I316"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_IB8"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_IB8.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_IB8"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_IB8_B"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_IB8_B.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_IB8_B"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_HUD_IF8"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/HUD_IF8.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_HUD_IF8"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Large"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/Large.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_Large"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Sheld"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/Sheld.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_Sheld"));

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Small"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/UI/UI_HUD/Small.png")))))
		return E_FAIL;
	m_form->tapUI->UIListBox.AddString(TEXT("Prototype_Component_Texture_Small"));


	return S_OK;
}

HRESULT CToolView::ReadyCameraLayer(const _tchar * _layerTag)
{
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.vEye = _float3(0.0f, 10.0f, -7.0f);
	CameraDesc.vAt = _float3(0.0f, 0.0f, 0.0f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(60.f);
	CameraDesc.fAspect = _float(g_WIN_WIDHT) / g_WIN_HEIGHT;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 300.f;

	CameraDesc.TransformDesc.fSpeedPerSec = 10.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	if (FAILED(gameInstance->Add_GameObjectToLayer(1, _layerTag, TEXT("Prototype_GameObject_Dyanmic_Camera"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolView::ReadyTerrainLayer(const _tchar* _layerTag)
{
	if (FAILED(gameInstance->Add_GameObjectToLayer(1, _layerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	if (FAILED(gameInstance->Add_GameObjectToLayer(1, _layerTag, TEXT("Prototype_GameObject_UI"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolView::ReadyObjectLayer(const _tchar * _layerTag)
{
	m_form->tapMap->m_objectListBox.AddString(TEXT("Zero"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::PLAYER);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BugMorph"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENEMY);

	m_form->tapMap->m_objectListBox.AddString(TEXT("PrimeBeast"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENEMY);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BossPrimeBeast"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::BOSS);

	m_form->tapMap->m_objectListBox.AddString(TEXT("Dahl_WeaponLocker"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::INTERACTION);

	m_form->tapMap->m_objectListBox.AddString(TEXT("DahlEpicCrate"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::INTERACTION);

	m_form->tapMap->m_objectListBox.AddString(TEXT("HyperionChest"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::INTERACTION);

	m_form->tapMap->m_objectListBox.AddString(TEXT("SnowDrift"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("SnowPile0"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("SnowPile1"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("SnowPile2"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("SnowPile3"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("SnowPile4")); 
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("GlacialFlow_Straight0"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("GlacialFlow_Straight1"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BrokenCrapTrapA"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BrokenCrapTrapB"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BrokenCrapTrapD"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BrokenCrapTrapE"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BrokenCrapTrapF"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("IceBlockLarge"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("SkyBoxMountain"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("SnowDriftStatic"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("FlagPole"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("FlagPoleOptimizer"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("ArenaSign"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("AutoJunk"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("AutoJunk2"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BanditFlag"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BanditHouse"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("BanditHouseDestory"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("HandsomeJack"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("HandsomeJackGoldenStatue"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("Tire"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("TurbinBase"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("TurbinFan"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("ComShip"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("ComShipBrace"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("DeadMaleVillagerBody"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("DeadMaleVillagerBody1"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("DeathBandit0"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("DeathBandit1"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("FrozenWaterFall"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("GlacialFlowWall"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("IceFalls"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("Wire0"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("Wire1"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("Wire2"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("Wire3"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::COLLISION_ENVIORNMENT);

	return S_OK;
}
