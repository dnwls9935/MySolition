
// ToolView.h : CToolView Ŭ������ �������̽�
//

#pragma once
#include "EngineIncludeh.h"
#include "GameInstance.h"

class CToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// Ư���Դϴ�.
public:
	class CToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:
	class CMainFrame*		mainFrame = nullptr;;
	class Form*				form = nullptr;

	class GameInstance*			gameInstance = nullptr;
	ID3D11Device*			dx11Device = nullptr;
	ID3D11DeviceContext*			dx11DeviceContext = nullptr;

private:
	HRESULT		SettingBasic();
	HRESULT		SettingDevice();

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // ToolView.cpp�� ����� ����
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

