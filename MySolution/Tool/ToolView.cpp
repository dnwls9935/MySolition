
// ToolView.cpp : CToolView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrm.h"
#include "Form.h"
#include "ClientDefine.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolView
HWND	g_hWnd;
HINSTANCE	g_hInst;
IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CToolView 생성/소멸

CToolView::CToolView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	//RELEASE_INSTANCE(GameInstance);
}

CToolView::~CToolView()
{

}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CToolView 그리기

HRESULT CToolView::SettingBasic()
{
	mainFrame = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	if (nullptr == mainFrame)
	{
		MSGBOX(L"Failed to Get MainFrame");
		return E_FAIL;
	}

	form = dynamic_cast<Form*>(mainFrame->mainSpliiter.GetPane(0, 0));
	if (nullptr == form)
	{
		MSGBOX(L"Failed to Get Form");
		return E_FAIL;
	}

	RECT mainRect = {};
	mainFrame->GetWindowRect(&mainRect);

	SetRect(&mainRect, 0, 0, mainRect.right - mainRect.left, mainRect.bottom - mainRect.top);
	RECT viewRect = {};
	GetClientRect(&viewRect);

	int width = mainRect.right - viewRect.right;
	int height = mainRect.bottom - viewRect.bottom;
	mainFrame->SetWindowPos(nullptr, 0, 0, g_WIN_WIDTH + width, g_WIN_HEIGHT + height, SWP_NOMOVE);

	g_hWnd = m_hWnd;
	g_hInst = AfxGetInstanceHandle();

	return S_OK;
}

HRESULT CToolView::SettingDevice()
{
	gameInstance = GET_INSTANCE(GameInstance);

	if (FAILED(gameInstance->InitializeEngineForTool(g_hWnd, DX11GraphicDev::WINMODE::MODE_WIN, g_WIN_WIDTH, g_WIN_HEIGHT, &dx11Device, &dx11DeviceContext)))
		return E_FAIL;

	return S_OK;
}

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
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

	if (FAILED(SettingBasic()))
		return;



	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}
