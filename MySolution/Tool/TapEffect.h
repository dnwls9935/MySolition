#pragma once


// TapEffect ��ȭ �����Դϴ�.

class TapEffect : public CDialogEx
{
	DECLARE_DYNAMIC(TapEffect)

public:
	TapEffect(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~TapEffect();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAPEFFECT };
#endif

protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
