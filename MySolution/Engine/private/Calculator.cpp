#include "..\public\Calculator.h"

#include "GameInstance.h"

void Calculator::CalcMousePos(CALCDESC* _calDesc)
{
	POINT	pt = {};
	GetCursorPos(&pt);
	ScreenToClient(_calDesc->_hWnd, &pt);

	// ���콺 ���� ������(window ����)
	_float3			mousePos;
	mousePos.x = pt.x / (_calDesc->_width * 0.5f) - 1.f;
	mousePos.y = pt.y / -(_calDesc->_height * 0.5f) + 1.f;
	mousePos.z = 0.f;

	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	// �������� �� �����̽��� �ٿ�
	_fmatrix projMat = gameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION);
	_matrix projMatInv = XMMatrixInverse(nullptr, projMat);

	_vector viewMousePos = XMVector3TransformCoord(XMLoadFloat3(&mousePos), projMatInv);

	// �信�� ����� �ٿ�
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

