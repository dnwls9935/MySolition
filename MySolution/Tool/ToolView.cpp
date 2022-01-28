
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

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f));
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_PrimeBeast"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/PrimeBeast/", "PrimeBeast.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationY(XMConvertToRadians(90.0f));
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HandsomeJack"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "HandsomeJack.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HandsomeJackGoldenStatue"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "HandsomeJackGoldlStatue.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HyperionChest"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Static/", "HyperionChest.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh_Tool.hlsl"), PivotMatrix, CModel::TYPE_STATIC))))
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
	



	///* 객체원형을 생성한다. */
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), ToolTerrain::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dyanmic_Camera"), ToolCamera::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;


	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_BugMorph"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_PrimeBeast"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Zero"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HandsomeJack"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HandsomeJackGoldenStatue"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HyperionChest"), ToolObject::Create(dx11Device, dx11DeviceContext))))
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

	if (FAILED(ReadyEffectLayer(TEXT("Effect"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolView::ReadyTexture()
{
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/Terrain/Snow_256_Dif.tga")))))
		return E_FAIL;
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

	m_form->tapMap->m_objectListBox.AddString(TEXT("HandsomeJack"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("HyperionChest"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

	m_form->tapMap->m_objectListBox.AddString(TEXT("HandsomeJackGoldenStatue"));
	m_form->tapMap->m_BatchObjectEnumArr.push_back((_int)CGameObject::OBJTYPE_ID::ENVIRONMENT);

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

	return S_OK;
}

HRESULT CToolView::ReadyEffectLayer(const _tchar * _layerTag)
{
	return S_OK;
}
