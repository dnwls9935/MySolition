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

private:
	HRESULT	SaveNavigation(HANDLE& hFile);
	HRESULT	SaveTerrainLayer(HANDLE& hFile);
	HRESULT SaveObjectLayer(HANDLE& hFile);

	HRESULT LoadTerrainLayer(HANDLE& hFile);
	HRESULT	LoadObjectLayer(HANDLE& hFile);
	HRESULT LoadNavigation(HANDLE& hFile);

public:
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButton1();
	afx_msg	void OnBnClieckRadio(UINT value);
	int m_radioValue;
	int m_brushRadius;
	afx_msg void OnEnChangeRadius();
	CEdit m_BrushRadiusText; 
	CButton m_CullMode;
	CListBox m_objectListBox;
	CButton m_BatchObject;
	afx_msg void OnBnClickedCheck2();
	CButton m_Modify;
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck4();
	CButton m_Navigation;
};
