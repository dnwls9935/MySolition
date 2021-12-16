#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)
public:
	enum MOUSEMOVESTATE { MMS_X, MMS_Y, MMS_WHEEL, MMS_END };
	enum MOUSEBUTTONSTATE { MBS_LBUTTON, MBS_RBUTTON, MBS_WHEEL, MBS_END };
private:
	CInput_Device();
	virtual ~CInput_Device() = default;
public:
	_byte Get_DIKState(_ubyte byKeyID) const {
		return m_KeyBoardState[byKeyID];
	}

	_long Get_MouseMoveState(MOUSEMOVESTATE eMoveState) const {
		return ((_long*)&m_MouseState)[eMoveState];
	}

	_byte Get_MouseButtonState(MOUSEBUTTONSTATE eButtonState) const {
		return m_MouseState.rgbButtons[eButtonState];
	}
	
public:
	HRESULT Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	
	/* 입력장치의 현재 상태를 얻어온다. */
	HRESULT SetUp_InputDeviceState();

private:
	IDirectInput8*			m_pInputSDK = nullptr;
	IDirectInputDevice8*	m_pKeyBoard = nullptr;
	IDirectInputDevice8*	m_pMouse = nullptr;

private:	
	_byte				m_KeyBoardState[256];
	DIMOUSESTATE		m_MouseState;
public:
	virtual void Free() override;
};

END