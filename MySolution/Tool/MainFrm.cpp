
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "ToolView.h"
#include "Form.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetMenu(NULL);
	//ModifyStyle(WS_CAPTION, NULL);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.hMenu = NULL;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기




BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	m_MainSpliiter.CreateStatic(this, 1, 2); //몇개로 분할할지 16x16 초과할 수 없다, 1행의 2열
											 //m_tSecondSplitter.CreateStatic(&m_tMainSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_tMainSplitter.IdFromRowCol(0, 0));

											 //##3번째 인자에서 MFCToolView* pView = new CMFCToolView //CMFCToolView : public부분이 protected라서 사용불가
	m_MainSpliiter.CreateView(0, 0, RUNTIME_CLASS(Form), CSize(400, 300), pContext);
	m_MainSpliiter.CreateView(0, 1, RUNTIME_CLASS(CToolView), CSize(300, 600), pContext);

	//m_tMainSplitter 크기를 지정안해줘서 창이 안보인다.
	m_MainSpliiter.SetColumnInfo(0, 400, 100); //가로 크기를 재조정 해주는 함수.

	return TRUE;
}