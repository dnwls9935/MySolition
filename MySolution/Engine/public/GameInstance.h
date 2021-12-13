#pragma once

#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
public:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HWND hWnd, _uint iNumLevel, CGraphic_Device::WINMODE eWinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	_int Tick_Engine(_double TimeDelta);

	/* 엔진에 정의되어있는 매니져객체들을 통해 디버깅적 요소들을 출력한다.  */
	/* 게임의 구성요소들은 렌더러를 통해 렌더하낟. */
	HRESULT Render_Engine(); 
	HRESULT Clear_Engine(_uint iLevelIndex);


public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(XMFLOAT4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public: /* For.Timer_Manager */ 
	_double Get_TimeDelta(const _tchar* pTimerTag);
	void Update_TimeDelta(const _tchar* pTimerTag);
	HRESULT Ready_Timer(const _tchar* pTimerTag);

public: /* For.Level_Manager*/
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pOpenLevel);

public: /* For.Object_Manager*/
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	
public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);
	
private:
	CGraphic_Device*			m_pGraphic_Device = nullptr;		
	CLevel_Manager*				m_pLevel_Manager = nullptr;
	CTimer_Manager*				m_pTimer_Manager = nullptr;
	CObject_Manager*			m_pObject_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
public:

	static void Release_Engine();
	virtual void Free() override;
};

END

