
// ToolView.h : CToolView Ŭ������ �������̽�
//

#pragma once
#include "GameInstance.h"

#include "MainFrm.h"
#include "Form.h"

class CToolDoc;
class CToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// Ư���Դϴ�.
public:
	CToolDoc* GetDocument() const;

// �۾��Դϴ�.
public:
	ID3D11Device*						dx11Device = nullptr;
	ID3D11DeviceContext*			dx11DeviceContext = nullptr;
	CGameInstance*						gameInstance = nullptr;

	class CRenderer* m_pRenderer = nullptr;

	class CMainFrame*		m_mainFrm = nullptr;
	class Form*					m_form = nullptr;


	_double		TimerAcc = 0.0; // ���β�

public:
	HRESULT		RenderBegin();
	HRESULT		RenderEnd();
	HRESULT		ReadyProtoAll();	

private:
	HRESULT	ReadyLayer();
	HRESULT	ReadyTexture();

	HRESULT	ReadyCameraLayer(const _tchar* _layerTag);
	HRESULT ReadyTerrainLayer(const _tchar* _layerTag);
	HRESULT ReadyObjectLayer(const _tchar* _layerTag);
	HRESULT ReadyEffectLayer(const _tchar* _layerTag);

	

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

