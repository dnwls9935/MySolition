#pragma once


// TapEffect 대화 상자입니다.

class TapEffect : public CDialogEx
{
	DECLARE_DYNAMIC(TapEffect)

public:
	TapEffect(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~TapEffect();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TAPEFFECT };
#endif

protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
