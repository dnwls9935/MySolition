#pragma once

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Timer_Manager.h"
#include "Object_Manager.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Calculator.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
public:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, HWND hWnd, _uint iNumLevel, CGraphic_Device::WINMODE eWinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	_int Tick_Engine(_double TimeDelta);
	_int Tick_Engine_For_Tool(_double TimeDelta);

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
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pOpenLevel, _uint nextLevelID);
	_uint GetNextLevelID();

public: /* For.Object_Manager*/
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	list<CGameObject*>& GetObjectList(_uint _levelIdx, const _tchar* _tag);
	//_uint		GetNumLevel();

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg);

public: /* For.PipeLine */
	_fmatrix Get_Transform(CPipeLine::TRANSFORMSTATEMATRIX eType);
	_fvector Get_CamPosition();
	void Set_Transform(CPipeLine::TRANSFORMSTATEMATRIX eType, _fmatrix TransformMatrix);

public: /* for.Input_Device */
	_byte Get_DIKeyState(_ubyte byKeyID) const;
	_long Get_MouseMoveState(CInput_Device::MOUSEMOVESTATE eMoveState) const;
	_byte Get_MouseButtonState(CInput_Device::MOUSEBUTTONSTATE eButtonState) const;

public: /* For.Calculator */
	void		CalcMousePos(Calculator::CALCDESC* _calDesc);
	
private:
	CGraphic_Device*			m_pGraphic_Device = nullptr;		
	CLevel_Manager*				m_pLevel_Manager = nullptr;
	CTimer_Manager*				m_pTimer_Manager = nullptr;
	CObject_Manager*			m_pObject_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	CInput_Device*				m_pInput_Device = nullptr;
public:

	static void Release_Engine();
	virtual void Free() override;
};

END

