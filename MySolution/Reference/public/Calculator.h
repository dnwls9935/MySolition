#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)
class Calculator
{
public:
	typedef struct tagCalculatorDesc {
		HWND _hWnd;
		_uint _width;
		_uint _height;
		_vector _rayPos = {0.f, 0.f, 0.f, 0.f};
		_vector _rayDir = { 0.f, 0.f, 0.f, 0.f };
		class CTransform* _transformCom;
	}CALCDESC;

private:
	Calculator();
	virtual ~Calculator() = default;

public:
	static void		CalcMousePos(CALCDESC* _calDesc);
};

END