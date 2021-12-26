
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�
HWND g_hWnd;
HINSTANCE g_hInst;
CToolView::CToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���

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
	/* ������Ʈ ���� �����Ѵ�. */
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/ShaderFiles/Shader_Terrain_Tool.hlsl"),200, 150))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Transform"), CTransform::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("ProtoType_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/ShaderFiles/Shader_Rect.hlsl")))))
		return E_FAIL;
	if (FAILED(ReadyTexture()))
		return E_FAIL;

	_matrix		pivotMat;
	pivotMat = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationX(XMConvertToRadians(90.0f)) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HyperionChest"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Kaelthas/", "HyperionChest.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh.hlsl"), pivotMat))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HandsomeJack"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Kaelthas/", "HandsomeJack.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh.hlsl"), pivotMat))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_HandsomeJackGoldlStatue"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Kaelthas/", "HandsomeJackGoldlStatue.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh.hlsl"), pivotMat))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Model_Kaelthas"), CModel::Create(dx11Device, dx11DeviceContext, "../Client/Bin/Resources/Meshes/Kaelthas/", "Kaelthas.FBX", TEXT("../Client/Bin/ShaderFiles/Shader_Mesh.hlsl"), pivotMat))))
		return E_FAIL;


	///* ��ü������ �����Ѵ�. */
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"), ToolTerrain::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dyanmic_Camera"), ToolCamera::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;

	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HyperionChest"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HandsomeJack"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_HandsomeJackGoldlStatue"), ToolObject::Create(dx11Device, dx11DeviceContext))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(TEXT("Prototype_GameObject_Model_Kaelthas"), ToolObject::Create(dx11Device, dx11DeviceContext))))
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

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
	if (TimerAcc >= 1.0 / 60.0f)
	{
		TimerAcc = 0.0;

		gameInstance->Update_TimeDelta(TEXT("Timer_60"));

		if (0 > gameInstance->Tick_Engine_For_Tool(gameInstance->Get_TimeDelta(TEXT("Timer_60"))))
			MSGBOX(L"Failed to Tick Engine");

		if (FAILED(RenderBegin()))
			MSGBOX(L"Failed to Render Begin");
		// ������ redering
		if (FAILED(m_pRenderer->Draw_RenderGroup()))
			MSGBOX(L"Failed to Draw RenderGroup");

		if (FAILED(RenderEnd()))
			MSGBOX(L"Failed to Render End");
	}


	Invalidate(FALSE);
}


// CToolView �μ�

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


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

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Terrain"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Textures/Terrain/Grass_%d.tga"), 2))))
		return E_FAIL;

	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Moon_Comp"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Menumap/Texture2D/Moon_Comp.tga")))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_Moon_Dif"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Menumap/Texture2D/Moon_Dif.tga")))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_MoonBase02_GRP"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Menumap/Texture2D/MoonBase02_GRP.tga")))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_MoonBase02a_Dif"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Menumap/Texture2D/MoonBase02a_Dif.tga")))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_MoonBase02a_Emis"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Menumap/Texture2D/MoonBase02a_Emis.tga")))))
		return E_FAIL;
	if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_Texture_MoonBase02a_Nrm"), CTexture::Create(dx11Device, dx11DeviceContext, TEXT("../Client/Bin/Resources/Menumap/Texture2D/MoonBase02a_Nrm.tga")))))
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
	ToolObject::TOOLOBJDESC objDesc;
	objDesc.m_BufferTag = TEXT("Prototype_Component_Model_HyperionChest");
	objDesc.m_Position = { 10.f,0.f,10.f };
	if (FAILED(gameInstance->Add_GameObjectToLayer(1, _layerTag, TEXT("Prototype_GameObject_Model_HyperionChest"), &objDesc)))
		return E_FAIL;

	objDesc.m_BufferTag = TEXT("Prototype_Component_Model_HandsomeJack");
	objDesc.m_Position = { 20.f,0.f,20.f };
	if (FAILED(gameInstance->Add_GameObjectToLayer(1, _layerTag, TEXT("Prototype_GameObject_Model_HandsomeJack"), &objDesc)))
		return E_FAIL;

	objDesc.m_BufferTag = TEXT("Prototype_Component_Model_HandsomeJackGoldlStatue");
	objDesc.m_Position = { 15.f,0.f,15.f };
	if (FAILED(gameInstance->Add_GameObjectToLayer(1, _layerTag, TEXT("Prototype_GameObject_Model_HandsomeJackGoldlStatue"), &objDesc)))
		return E_FAIL;

	objDesc.m_BufferTag = TEXT("Prototype_Component_Model_Kaelthas");
	objDesc.m_Position = { 0.f,0.f,0.f};
	if (FAILED(gameInstance->Add_GameObjectToLayer(1, _layerTag, TEXT("Prototype_GameObject_Model_Kaelthas"), &objDesc)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CToolView::ReadyEffectLayer(const _tchar * _layerTag)
{
	return S_OK;
}
