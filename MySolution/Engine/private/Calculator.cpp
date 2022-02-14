#include "..\public\Calculator.h"

#include "GameInstance.h"

void Calculator::CalcMousePos(CALCDESC* _calDesc)
{
	POINT	pt = {};
	GetCursorPos(&pt);
	ScreenToClient(_calDesc->_hWnd, &pt);

	// 마우스 포스 가져옴(window 기준)
	_float3			mousePos;
	mousePos.x = pt.x / (_calDesc->_width * 0.5f) - 1.f;
	mousePos.y = pt.y / -(_calDesc->_height * 0.5f) + 1.f;
	mousePos.z = 0.f;

	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	// 투영에서 뷰 스페이스로 다운
	_fmatrix projMat = gameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION);
	_matrix projMatInv = XMMatrixInverse(nullptr, projMat);

	_vector viewMousePos = XMVector3TransformCoord(XMLoadFloat3(&mousePos), projMatInv);

	// 뷰에서 월드로 다운
	_vector RayPos, RayDir;
	RayPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	RayDir = viewMousePos - RayPos;

	_fmatrix viewMat = gameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	_matrix viewMatInv = XMMatrixInverse(nullptr, viewMat);

	RayPos = XMVector3TransformCoord(RayPos, viewMatInv);
	RayDir = XMVector3TransformNormal(RayDir, viewMatInv);


	_fmatrix worldMatInv = _calDesc->_transformCom->Get_WorldMatrixInverse();
	//
	RayPos = XMVector3TransformCoord(RayPos, worldMatInv);
	RayDir = XMVector3TransformNormal(RayDir, worldMatInv);
	RayDir = XMVector3Normalize(RayDir);

	_calDesc->_rayPos = RayPos;
	_calDesc->_rayDir = RayDir;

	RELEASE_INSTANCE(CGameInstance);
}

_vector Calculator::GetWindowPos(ID3D11DeviceContext* _DeviceContext, _float _WindowX, _float _WindowY, _float _PositionX, _float _PositionY)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	D3D11_VIEWPORT		Viewport;
	ZeroMemory(&Viewport, sizeof(D3D11_VIEWPORT));
	_uint NumViewport = 1;
	_DeviceContext->RSGetViewports(&NumViewport, &Viewport);

	_vector Position = XMVectorZero();

	Position = XMVectorSetX(Position, _PositionX / (Viewport.Width * 0.5f) - 1.f);
	Position = XMVectorSetY(Position, _PositionY / -(Viewport.Height * 0.5f) + 1.f);
	Position = XMVectorSetZ(Position, 0.f);
	Position = XMVectorSetW(Position, 1.f);

	_matrix		MatProj =XMMatrixOrthographicLH((_float)_WindowX, (_float)_WindowY, 0.f, 1.f);;
	MatProj = XMMatrixInverse(nullptr, MatProj);
	Position = XMVector3TransformCoord(Position, MatProj);

	_matrix		MatView = XMMatrixIdentity();
	MatView = XMMatrixInverse(nullptr, MatView);
	Position = XMVector3TransformCoord(Position, MatView);

	RELEASE_INSTANCE(CGameInstance);
	return Position;
}

_int Calculator::CalcRandom(_int _Input)
{
	_int Percent = _Input / 10;
	_int Random = (rand() % Percent) - Percent;

	return _Input += Random;
}

_float Calculator::Lerp(_float _From, _float _To, _float _DeltaTime, Lerp::EaseType _EaseType) noexcept
{
	_float time = GetEaseTime(_DeltaTime, _EaseType);
	return _From * (1 - time) + _To * time;
}

_float Calculator::GetEaseTime(_float _DeltaTime, Lerp::EaseType _EaseType) noexcept
{
	switch (_EaseType)
	{
	case Lerp::EaseType::EaseLiner:
		return EaseLiner(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInSine:
		return EaseInSine(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutSine:
		return EaseOutSine(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInQutSine:
		return EaseInOutSine(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInQuad:
		return EaseInQuad(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutQuad:
		return EaseOutQuad(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutQuad:
		return EaseInOutQuad(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInCubic:
		return EaseInCubic(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutCubic:
		return EaseOutCubic(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutCubic:
		return EaseInOutCubic(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInQuart:
		return EaseInQuart(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutQuart:
		return EaseOutQuart(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutQuart:
		return EaseInOutQuart(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInQuint:
		return EaseInQuint(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutQuint:
		return EaseOutQuint(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutQuint:
		return EaseInOutQuint(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInExpo:
		return EaseInExpo(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutExpo:
		return EaseOutExpo(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutExpo:
		return EaseInOutExpo(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInCirc:
		return EaseInCirc(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutCirc:
		return EaseOutCirc(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutCirc:
		return EaseInOutCirc(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInBack:
		return EaseInBack(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutBack:
		return EaseOutBack(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutBack:
		return EaseInOutBack(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInElastic:
		return EaseInElastic(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutElastic:
		return EaseOutElastic(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutElastic:
		return EaseInOutElastic(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInBounce:
		return EaseInBounce(_DeltaTime);
		break;

	case Lerp::EaseType::EaseOutBounce:
		return EaseOutBounce(_DeltaTime);
		break;

	case Lerp::EaseType::EaseInOutBounce:
		return EaseInOutBounce(_DeltaTime);
		break;

	default:
		return _DeltaTime;
		break;
	}
}

_float Calculator::EaseLiner(_float t) noexcept
{
	return t;
}

_float Calculator::EaseInSine(_float t) noexcept
{
	return 1 - cosf((t * XM_PI) / 2.f);
}

_float Calculator::EaseOutSine(_float t) noexcept
{
	return sinf((t * XM_PI) / 2.f);
}

_float Calculator::EaseInOutSine(_float t) noexcept
{
	return -(cosf(XM_PI * t) - 1) / 2.f;
}

_float Calculator::EaseInQuad(_float t) noexcept
{
	return t * t;
}

_float Calculator::EaseOutQuad(_float t) noexcept
{
	return 1 - (1 - t) * (1 - t);
}

_float Calculator::EaseInOutQuad(_float t) noexcept
{
	return t < 0.5f ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2.f;
}

_float Calculator::EaseInCubic(_float t) noexcept
{
	return t * t * t;
}

_float Calculator::EaseOutCubic(_float t) noexcept
{
	return 1 - powf(1 - t, 3);
}

_float Calculator::EaseInOutCubic(_float t) noexcept
{
	return t < 0.5f ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2.f;
}

_float Calculator::EaseInQuart(_float t) noexcept
{
	return t * t * t * t;
}

_float Calculator::EaseOutQuart(_float t) noexcept
{
	return 1 - powf(1 - t, 4);
}

_float Calculator::EaseInOutQuart(_float t) noexcept
{
	return t < 0.5f ? 8 * t * t * t * t : 1 - powf(-2 * t + 2, 4) / 2.f;
}

_float Calculator::EaseInQuint(_float t) noexcept
{
	return t * t * t * t * t;
}

_float Calculator::EaseOutQuint(_float t) noexcept
{
	return 1 - powf(1 - t, 5);
}

_float Calculator::EaseInOutQuint(_float t) noexcept
{
	return t < 0.5f ? 16 * t * t * t * t * t : 1 - powf(-2 * t + 2, 5) / 2.f;
}

_float Calculator::EaseInExpo(_float t) noexcept
{
	return t == 0 ? 0 : powf(2, 10 * t - 10);
}

_float Calculator::EaseOutExpo(_float t) noexcept
{
	return t == 1 ? 1 : 1 - powf(2, -10 * t);
}

_float Calculator::EaseInOutExpo(_float t) noexcept
{
	return t == 0
		? 0
		: t == 1
		? 1
		: t < 0.5f ? powf(2, 20 * t - 10) / 2.f
		: (2 - pow(2, -20 * t + 10)) / 2.f;
}

_float Calculator::EaseInCirc(_float t) noexcept
{
	return 1 - sqrtf(1 - powf(t, 2));
}

_float Calculator::EaseOutCirc(_float t) noexcept
{
	return sqrtf(1 - powf(t - 1, 2));
}

_float Calculator::EaseInOutCirc(_float t) noexcept
{
	return t < 0.5f
		? (1 - sqrtf(1 - powf(2 * t, 2))) / 2.f
		: (sqrtf(1 - powf(-2 * t + 2, 2)) + 1) / 2.f;
}

_float Calculator::EaseInBack(_float t) noexcept
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;

	return c3 * t * t * t - c1 * t * t;
}

_float Calculator::EaseOutBack(_float t) noexcept
{
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;

	return 1 + c3 * powf(t - 1, 3) + c1 * powf(t - 1, 2);
}

_float Calculator::EaseInOutBack(_float t) noexcept
{
	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;

	return t < 0.5f
		? (powf(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2.f
		: (powf(2 * t - 2, 2) * ((c2 + 1) * (t * 2 - 2) + c2) + 2) / 2.f;
}

_float Calculator::EaseInElastic(_float t) noexcept
{
	const float c4 = (2 * XM_PI) / 3.f;

	return t == 0
		? 0
		: t == 1
		? 1
		: -powf(2, 10 * t - 10) * sinf((t * 10 - 10.75f) * c4);
}

_float Calculator::EaseOutElastic(_float t) noexcept
{
	const float c4 = (2 * XM_PI) / 3.f;

	return t == 0
		? 0
		: t == 1
		? 1
		: powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1;
}

_float Calculator::EaseInOutElastic(_float t) noexcept
{
	const float c5 = (2 * XM_PI) / 4.5f;

	return t == 0
		? 0
		: t == 1
		? 1
		: t < 0.5f
		? -(powf(2, 20 * t - 10) * sinf((20 * t - 11.125f) * c5)) / 2.f
		: (powf(2, -20 * t + 10) * sinf((20 * t - 11.125f) * c5)) / 2.f + 1;
}

_float Calculator::EaseInBounce(_float t) noexcept
{
	return 1 - EaseOutBounce(1 - t);
}

_float Calculator::EaseOutBounce(_float t) noexcept
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (t < 1 / d1) {
		return n1 * t * t;
	}
	else if (t < 2 / d1) {
		return n1 * (t -= 1.5 / d1) * t + 0.75f;
	}
	else if (t < 2.5f / d1) {
		return n1 * (t -= 2.25f / d1) * t + 0.9375f;
	}
	else {
		return n1 * (t -= 2.625f / d1) * t + 0.984375f;
	}
}

_float Calculator::EaseInOutBounce(_float t) noexcept
{
	return t < 0.5f
		? (1 - EaseOutBounce(1 - 2 * t)) / 2.f
		: (1 + EaseOutBounce(2 * t - 1)) / 2.f;
}

