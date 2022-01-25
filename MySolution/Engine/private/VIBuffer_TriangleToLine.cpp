#include "..\public\VIBuffer_TriangleToLine.h"

CVIBuffer_TriangleToLine::CVIBuffer_TriangleToLine(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_TriangleToLine::CVIBuffer_TriangleToLine(const CVIBuffer_TriangleToLine & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_TriangleToLine::NativeConstruct_Prototype(const _tchar* pShaderFilePath, _float3* Points)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	/* D3D11_BUFFER_DESC */
	m_iStride = sizeof(VTXCOL);
	m_iNumVertices = 3;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;


	/* D3D11_SUBRESOURCE_DATA */
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_pVertices = new VTXCOL[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXCOL) * m_iNumVertices);

	if (nullptr == Points)
	{
		((VTXCOL*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.0f, 0.f);
		((VTXCOL*)m_pVertices)[1].vPosition = _float3(0.0f, 1.f, 0.0f);
		((VTXCOL*)m_pVertices)[2].vPosition = _float3(0.5f, 0.0f, 0.f);
	}
	else {
		((VTXCOL*)m_pVertices)[0].vPosition = Points[0];
		((VTXCOL*)m_pVertices)[1].vPosition = Points[1];
		((VTXCOL*)m_pVertices)[2].vPosition = Points[2];
	}

	((VTXCOL*)m_pVertices)[0].vColor = _float4(0.f, 0.f, 0.f, 1.f);
	((VTXCOL*)m_pVertices)[1].vColor = _float4(0.f, 0.f, 0.f, 1.f);
	((VTXCOL*)m_pVertices)[2].vColor = _float4(0.f, 0.f, 0.f, 1.f);

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;


	/* D3D11_BUFFER_DESC */
	m_IndicesByteLength = sizeof(FACEINDICES16);
	m_iNumPrimitive = 3;
	m_iNumIndicesFigure = 2;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	m_eFormat = DXGI_FORMAT_R16_UINT;

	m_IBDesc.ByteWidth = m_IndicesByteLength * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;


	/* D3D11_SUBRESOURCE_DATA */
	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	LINEINDICES16*		pIndices = new LINEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(LINEINDICES16) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;

	pIndices[1]._0 = 1;
	pIndices[1]._1 = 2;

	pIndices[2]._0 = 2;
	pIndices[2]._1 = 0;

	m_IBSubresourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	D3D11_INPUT_ELEMENT_DESC		ElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_ShaderFiles(pShaderFilePath, ElementDescs, 2)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer_TriangleToLine::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_TriangleToLine * CVIBuffer_TriangleToLine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, _float3* _Points)
{
	CVIBuffer_TriangleToLine*		pInstance = new CVIBuffer_TriangleToLine(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, _Points)))
	{
		MSGBOX("Failed to Creating CVIBuffer_Rect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TriangleToLine::Clone(void * pArg)
{
	CVIBuffer_TriangleToLine*		pInstance = new CVIBuffer_TriangleToLine(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CVIBuffer_TriangleToLine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_TriangleToLine::Free()
{
	__super::Free();


}
