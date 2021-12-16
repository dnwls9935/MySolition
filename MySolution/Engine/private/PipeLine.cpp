#include "..\public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{

}

void CPipeLine::Update_PipeLine()
{
	/* 카메라의 위치를 계산해 놓는다ㅏ. */
	_matrix		CamWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[D3DTS_VIEW]));

	XMStoreFloat4(&m_vWorldCamPosition, CamWorldMatrix.r[3]);
}

void CPipeLine::Free()
{
}
