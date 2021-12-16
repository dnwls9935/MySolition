
// ToolView.h : CToolView 클래스의 인터페이스
//

#pragma once
#include "GameInstance.h"

#include "MainFrm.h"
#include "Form.h"

class CToolDoc;
class CToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// 특성입니다.
public:
	CToolDoc* GetDocument() const;

// 작업입니다.
public:
	ID3D11Device*						dx11Device = nullptr;
	ID3D11DeviceContext*			dx11DeviceContext = nullptr;
	CGameInstance*						gameInstance = nullptr;

	class CRenderer* m_pRenderer = nullptr;

	class CMainFrame*		m_mainFrm = nullptr;
	class Form*					m_form = nullptr;


	_double		TimerAcc = 0.0; // 메인꺼

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

	

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // ToolView.cpp의 디버그 버전
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

