#pragma once


#include "GameInstance.h"
#include "ToolDefines.h"
#include "ToolTerrain.h"
#include "ToolCamera.h"
#include "afxwin.h"
// TapMap ��ȭ �����Դϴ�.

class TapMap : public CDialogEx
{
	DECLARE_DYNAMIC(TapMap)

public:
	TapMap(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~TapMap();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAPMAP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:

public:
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CEdit m_VerticesX;
	CEdit m_VerticesZ;
	afx_msg void OnBnClickedButton1();
};
