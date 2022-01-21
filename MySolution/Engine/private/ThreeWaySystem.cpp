#include "../public/ThreeWaySystem.h"

CVIBuffer_ThreeWaySystem::CVIBuffer_ThreeWaySystem(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_ThreeWaySystem::CVIBuffer_ThreeWaySystem(const CVIBuffer_ThreeWaySystem & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_ThreeWaySystem::NativeConstruct_Prototype(const _tchar* pShaderFilePath)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	/* D3D11_BUFFER_DESC */
	m_iStride = sizeof(VTXCOL);
	m_iNumVertices = 4;

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

	((VTXTEX*)m_pVertices)[0].vPosition = _float3(0.0f, 0.0f, 0.f);

	((VTXTEX*)m_pVertices)[1].vPosition = _float3(0.0f, 0.0f, 3.0f);

	((VTXTEX*)m_pVertices)[2].vPosition = _float3(0.0f, 3.0f, 0.0f);

	((VTXTEX*)m_pVertices)[3].vPosition = _float3(3.0f, 0.0f, 0.0f);


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

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;

	pIndices[2]._0 = 0;
	pIndices[2]._1 = 3;

	m_IBSubresourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	D3D11_INPUT_ELEMENT_DESC		ElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(Compile_ShaderFiles(pShaderFilePath, ElementDescs, 1)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_ThreeWaySystem::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_ThreeWaySystem * CVIBuffer_ThreeWaySystem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath)
{
	CVIBuffer_ThreeWaySystem*		pInstance = new CVIBuffer_ThreeWaySystem(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath)))
	{
		MSGBOX("Failed to Creating CVIBuffer_ThreeWaySystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_ThreeWaySystem::Clone(void * pArg)
{
	CVIBuffer_ThreeWaySystem*		pInstance = new CVIBuffer_ThreeWaySystem(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CVIBuffer_ThreeWaySystem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_ThreeWaySystem::Free()
{
	__super::Free();


}
