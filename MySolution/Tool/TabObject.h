#pragma once


// TabObject ��ȭ �����Դϴ�.

class TabObject : public CDialogEx
{
	DECLARE_DYNAMIC(TabObject)

public:
	TabObject(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~TabObject();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TABOBJECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
