#include "..\public\Cell.h"

#include "VIBuffer_TriangleToLine.h"
#include "PipeLine.h"

Cell::Cell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT Cell::NativeConstruct(_float3* _Point, _uint _Index, const _tchar* _ShaderFilePath)
{
	m_Index = _Index;

	ZeroMemory(m_Neighbor, sizeof(Cell*) * (_uint)LINE::LINE_END);

	memcpy(m_Point, _Point, sizeof(_float3) * (_uint)POINT::POINT_END);

	XMStoreFloat3(&m_Line[(_uint)LINE::LINE_12], XMLoadFloat3(&m_Point[(_uint)POINT::POINT_2nd]) - XMLoadFloat3(&m_Point[(_uint)POINT::POINT_1st]));
	XMStoreFloat3(&m_Line[(_uint)LINE::LINE_23], XMLoadFloat3(&m_Point[(_uint)POINT::POINT_3rd]) - XMLoadFloat3(&m_Point[(_uint)POINT::POINT_2nd]));
	XMStoreFloat3(&m_Line[(_uint)LINE::LINE_31], XMLoadFloat3(&m_Point[(_uint)POINT::POINT_1st]) - XMLoadFloat3(&m_Point[(_uint)POINT::POINT_3rd]));

	m_Normal[(_uint)LINE::LINE_12] = _float3(m_Line[(_uint)LINE::LINE_12].z * -1.f, m_Line[(_uint)LINE::LINE_12].y, m_Line[(_uint)LINE::LINE_12].x);
	m_Normal[(_uint)LINE::LINE_23] = _float3(m_Line[(_uint)LINE::LINE_23].z * -1.f, m_Line[(_uint)LINE::LINE_23].y, m_Line[(_uint)LINE::LINE_23].x);
	m_Normal[(_uint)LINE::LINE_31] = _float3(m_Line[(_uint)LINE::LINE_31].z * -1.f, m_Line[(_uint)LINE::LINE_31].y, m_Line[(_uint)LINE::LINE_31].x);

	for (_uint i = 0; i < (_uint)LINE::LINE_END; i++) {
		_vector normal = XMVector3Normalize(XMLoadFloat3(&m_Normal[i]));
		XMStoreFloat3(&m_Normal[i], normal);
	}

#ifdef _DEBUG
	if (FAILED(ReadyDebugBuffer(_ShaderFilePath)))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

_bool Cell::Compare(_fvector _SourPoint, _fvector _DestPoint)
{
	if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_1st]), _SourPoint))
	{
		if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_2nd]), _DestPoint))
			return TRUE;
		if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_3rd]), _DestPoint))
			return TRUE;
	}
	if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_2nd]), _SourPoint))
	{
		if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_3rd]), _DestPoint))
			return TRUE;
		if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_1st]), _DestPoint))
			return TRUE;
	}
	if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_3rd]), _SourPoint))
	{
		if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_1st]), _DestPoint))
			return TRUE;
		if (TRUE == XMVector3Equal(XMLoadFloat3(&m_Point[(_uint)POINT::POINT_2nd]), _DestPoint))
			return TRUE;
	}
	return FALSE;
}

_bool Cell::IsIn(_fvector _Position, Cell ** _OutNeighbor)
{
	for (_uint i = 0; i < (_uint)LINE::LINE_END; i++)
	{
		_vector pDir = _Position - XMLoadFloat3(&m_Point[i]);

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(pDir), XMLoadFloat3(&m_Normal[i]))))
		{
			*_OutNeighbor = m_Neighbor[i];
			return FALSE;
		}
	}
	return TRUE;
}
#ifdef _DEBUG
HRESULT Cell::ReadyDebugBuffer(const _tchar* _ShaderFilePath)
{
	m_VIBuffer = CVIBuffer_TriangleToLine::Create(m_pDevice, m_pDeviceContext, _ShaderFilePath, m_Point);

	if (nullptr == m_VIBuffer)
		return E_FAIL;
	return S_OK;
}

HRESULT Cell::Render(_fmatrix _WorldMatrix, _uint _CurrentIndex)
{
	if (nullptr == m_VIBuffer)
		return E_FAIL;

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_VIBuffer->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(_WorldMatrix), sizeof(_float4x4));
	m_VIBuffer->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pPipeLine->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_float4x4));
	m_VIBuffer->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pPipeLine->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_float4x4));

	if (_CurrentIndex == m_Index)
		m_VIBuffer->SetUp_ValueOnShader("g_vCellColor", &XMVectorSet(1.f, 0.f, 0.f, 1.f), sizeof(_float4));
	else
		m_VIBuffer->SetUp_ValueOnShader("g_vCellColor", &XMVectorSet(0.f, 1.f, 0.f, 1.f), sizeof(_float4));

	m_VIBuffer->Render(0);
	RELEASE_INSTANCE(CPipeLine);
	return S_OK;
}
#endif // _DEBUG

Cell * Cell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float3* _Point, _uint _Index, const _tchar* _ShaderFilePath)
{
	Cell*		pInstance = new Cell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(_Point, _Index, _ShaderFilePath)))
	{
		MSGBOX("Failed to Creating Cell");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Cell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
