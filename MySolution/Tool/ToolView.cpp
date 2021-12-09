
// ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�

CToolView::CToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	//RELEASE_INSTANCE(GameInstance);
}

CToolView::~CToolView()
{

}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���

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

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
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

	if (FAILED(SettingBasic()))
		return;



	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}
