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

