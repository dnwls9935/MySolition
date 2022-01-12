#include "..\public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar* pShaderFilePath, _uint x, _uint z)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iNumVerticesX = x;
	m_iNumVerticesZ = z;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	/* D3D11_BUFFER_DESC */
	m_iStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;


	/* D3D11_SUBRESOURCE_DATA */
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			((VTXNORTEX*)m_pVertices)[iIndex].vPosition = _float3((_float)j, 0.f, (_float)i);
			((VTXNORTEX*)m_pVertices)[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			((VTXNORTEX*)m_pVertices)[iIndex].vTexUV = _float2(j / _float((m_iNumVerticesX - 1)), i / _float((m_iNumVerticesZ - 1)));
		}
	}

	m_VBSubresourceData.pSysMem = m_pVertices;



	/* D3D11_BUFFER_DESC */
	m_IndicesByteLength = sizeof(FACEINDICES32);
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndicesFigure = 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eFormat = DXGI_FORMAT_R32_UINT;

	m_IBDesc.ByteWidth = m_IndicesByteLength * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;


	/* D3D11_SUBRESOURCE_DATA */
	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint	iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			_uint	iIndices[] = {
				{ iIndex + m_iNumVerticesX },
				{ iIndex + m_iNumVerticesX + 1 },
				{ iIndex + 1 },
				{ iIndex }
			};

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;

			vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vPosition);

			vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);
			vNormal = XMVector3Normalize(vNormal);

			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[2];
			pIndices[iNumPrimitive]._2 = iIndices[3];

			vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vPosition);

			vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);
			vNormal = XMVector3Normalize(vNormal);

			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;
		}
	}

	m_IBSubresourceData.pSysMem = pIndices;


	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	D3D11_INPUT_ELEMENT_DESC		ElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_ShaderFiles(pShaderFilePath, ElementDescs, 3)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Terrain::SetVerticeY(_float4 _mousePos, _float _y, _int _scale, _uint _type)
{
	D3D11_MAPPED_SUBRESOURCE data;
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	_float x = 0;
	_float z = 0;
	_int dist = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);
	_uint	iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			_uint	iIndices[] = {
				{ iIndex + m_iNumVerticesX },
				{ iIndex + m_iNumVerticesX + 1 },
				{ iIndex + 1 },
				{ iIndex }
			};

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;


			switch (_type)
			{
			case 1:
				x = _mousePos.x - ((VTXNORTEX*)m_pVertices)[iIndex].vPosition.x;
				z = _mousePos.z - ((VTXNORTEX*)m_pVertices)[iIndex].vPosition.z;

				dist = (_uint)(sqrt(x * x + z * z));

				if (_scale > dist)
				{
					((VTXNORTEX*)m_pVertices)[iIndex].vPosition.y += _y;

					vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
						XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vPosition);

					vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
						XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition);

					vNormal = XMVector3Cross(vSour, vDest);
					vNormal = XMVector3Normalize(vNormal);

					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

					++iNumPrimitive;

					pIndices[iNumPrimitive]._0 = iIndices[0];
					pIndices[iNumPrimitive]._1 = iIndices[2];
					pIndices[iNumPrimitive]._2 = iIndices[3];

					vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
						XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vPosition);

					vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
						XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition);

					vNormal = XMVector3Cross(vSour, vDest);
					vNormal = XMVector3Normalize(vNormal);

					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

					++iNumPrimitive;
					((VTXNORTEX*)(data.pData))[iIndex] = ((VTXNORTEX*)m_pVertices)[iIndex];
				}
				break;
			case 2:
				if (_mousePos.x - _scale <= ((VTXNORTEX*)m_pVertices)[iIndex].vPosition.x &&
					_mousePos.x + _scale >= ((VTXNORTEX*)m_pVertices)[iIndex].vPosition.x &&
					_mousePos.z - _scale <= ((VTXNORTEX*)m_pVertices)[iIndex].vPosition.z &&
					_mousePos.z + _scale >= ((VTXNORTEX*)m_pVertices)[iIndex].vPosition.z
					)
				{
					((VTXNORTEX*)m_pVertices)[iIndex].vPosition.y += _y;

					vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
						XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vPosition);

					vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
						XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition);

					vNormal = XMVector3Cross(vSour, vDest);
					vNormal = XMVector3Normalize(vNormal);

					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

					++iNumPrimitive;

					pIndices[iNumPrimitive]._0 = iIndices[0];
					pIndices[iNumPrimitive]._1 = iIndices[2];
					pIndices[iNumPrimitive]._2 = iIndices[3];

					vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
						XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vPosition);

					vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
						XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition);

					vNormal = XMVector3Cross(vSour, vDest);
					vNormal = XMVector3Normalize(vNormal);

					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
					XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal,
						XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

					++iNumPrimitive;
					((VTXNORTEX*)(data.pData))[iIndex] = ((VTXNORTEX*)m_pVertices)[iIndex];
				}
				break;
			default:
				break;
			}
		}
	}

	Safe_Delete_Array(pIndices);

	m_pDeviceContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Terrain::SetVerticeY(_int _index, _float _y)
{
	D3D11_MAPPED_SUBRESOURCE data;
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	((VTXNORTEX*)m_pVertices)[_index].vPosition.y = _y;
	((VTXNORTEX*)(data.pData))[_index] = ((VTXNORTEX*)m_pVertices)[_index];

	m_pDeviceContext->Unmap(m_pVB, 0);
}

void CVIBuffer_Terrain::SetNormalVector()
{
	D3D11_MAPPED_SUBRESOURCE data;
	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	/* //////////////////////////////////////////// */
	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint	iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = i * m_iNumVerticesX + j;

			_uint	iIndices[] = {
				{ iIndex + m_iNumVerticesX },
				{ iIndex + m_iNumVerticesX + 1 },
				{ iIndex + 1 },
				{ iIndex }
			};

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;

			vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vPosition);

			vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);
			vNormal = XMVector3Normalize(vNormal);

			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[2];
			pIndices[iNumPrimitive]._2 = iIndices[3];

			vSour = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vPosition);

			vDest = XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vPosition);

			vNormal = XMVector3Cross(vSour, vDest);
			vNormal = XMVector3Normalize(vNormal);

			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._0].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._1].vNormal) + vNormal));
			XMStoreFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)m_pVertices)[pIndices[iNumPrimitive]._2].vNormal) + vNormal));

			++iNumPrimitive;
			((VTXNORTEX*)(data.pData))[iIndex] = ((VTXNORTEX*)m_pVertices)[iIndex];
		}
	}
	/* //////////////////////////////////////////// */

	Safe_Delete_Array(pIndices);
	m_pDeviceContext->Unmap(m_pVB, 0);
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, _uint x, _uint z)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath,x,z)))
	{
		MSGBOX("Failed to Creating CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*		pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
