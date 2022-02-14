#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

namespace Lerp {
	enum class EaseType {
		EaseLiner,
		EaseInSine,
		EaseOutSine,
		EaseInQutSine,
		EaseInQuad,
		EaseOutQuad,
		EaseInOutQuad,
		EaseInCubic,
		EaseOutCubic,
		EaseInOutCubic,
		EaseInQuart,
		EaseOutQuart,
		EaseInOutQuart,
		EaseInQuint,
		EaseOutQuint,
		EaseInOutQuint,
		EaseInExpo,
		EaseOutExpo,
		EaseInOutExpo,
		EaseInCirc,
		EaseOutCirc,
		EaseInOutCirc,
		EaseInBack,
		EaseOutBack,
		EaseInOutBack,
		EaseInElastic,
		EaseOutElastic,
		EaseInOutElastic,
		EaseInBounce,
		EaseOutBounce,
		EaseInOutBounce
	};
};

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
	static _vector	GetWindowPos(ID3D11DeviceContext* _DeviceContext , _float _WindowX, _float _WindowY, _float _PositionX, _float _PositionY);
	static	_int		CalcRandom(_int _Input);

	static _float Lerp(_float _From, _float _To, _float _DeltaTime, Lerp::EaseType _EaseType = Lerp::EaseType::EaseLiner) noexcept;

private:
	static _float GetEaseTime(_float _DeltaTime, Lerp::EaseType _EaseType) noexcept;

	static _float	EaseLiner(_float t) noexcept;
	static _float	EaseInSine(_float t) noexcept;
	static _float	EaseOutSine(_float t) noexcept;
	static _float	EaseInOutSine(_float t) noexcept;
	static _float	EaseInQuad(_float t) noexcept;
	static _float	EaseOutQuad(_float t) noexcept;
	static _float	EaseInOutQuad(_float t) noexcept;
	static _float	EaseInCubic(_float t) noexcept;
	static _float	EaseOutCubic(_float t) noexcept;
	static _float	EaseInOutCubic(_float t) noexcept;
	static _float	EaseInQuart(_float t) noexcept;
	static _float	EaseOutQuart(_float t) noexcept;
	static _float	EaseInOutQuart(_float t) noexcept;
	static _float	EaseInQuint(_float t) noexcept;
	static _float	EaseOutQuint(_float t) noexcept;
	static _float	EaseInOutQuint(_float t) noexcept;
	static _float	EaseInExpo(_float t) noexcept;
	static _float	EaseOutExpo(_float t) noexcept;
	static _float	EaseInOutExpo(_float t) noexcept;
	static _float	EaseInCirc(_float t) noexcept;
	static _float	EaseOutCirc(_float t) noexcept;
	static _float	EaseInOutCirc(_float t) noexcept;
	static _float	EaseInBack(_float t) noexcept;
	static _float	EaseOutBack(_float t) noexcept;
	static _float	EaseInOutBack(_float t) noexcept;
	static _float	EaseInElastic(_float t) noexcept;
	static _float	EaseOutElastic(_float t) noexcept;
	static _float	EaseInOutElastic(_float t) noexcept;
	static _float	EaseInBounce(_float t) noexcept;
	static _float	EaseOutBounce(_float t) noexcept;
	static _float	EaseInOutBounce(_float t) noexcept;

private:

};


END