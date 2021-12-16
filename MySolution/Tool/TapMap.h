#pragma once


#include "GameInstance.h"
#include "ToolDefines.h"
#include "ToolTerrain.h"
#include "ToolCamera.h"
// TapMap 대화 상자입니다.

class TapMap : public CDialogEx
{
	DECLARE_DYNAMIC(TapMap)

public:
	TapMap(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~TapMap();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAPMAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

public:
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
