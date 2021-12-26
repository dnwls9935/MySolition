#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"


CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CModel::CModel(const CModel & rhs)
	: CVIBuffer(rhs)
	, m_pFaceIndices(rhs.m_pFaceIndices)
	, m_pScene(rhs.m_pScene)
	, m_MeshContainers(rhs.m_MeshContainers)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
{
	for (auto& pMeshContainer : m_MeshContainers)
		Safe_AddRef(pMeshContainer);

	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(pMaterial->pMeshTexture[i]);
	}
}

HRESULT CModel::NativeConstruct_Prototype(const char * pMeshFilePath, const char * pMeshFileName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	strcpy_s(m_szMeshFilePath, pMeshFilePath);

	char		szFullPath[MAX_PATH] = "";
	strcpy_s(szFullPath, pMeshFilePath);
	strcat_s(szFullPath, pMeshFileName);

	m_pScene = m_Importer.ReadFile(szFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (nullptr == m_pScene)
		return E_FAIL;

	if (FAILED(Reserve_VertexIndexData()))
		return E_FAIL;

	if (FAILED(Create_MeshContainer()))
		return E_FAIL;

	if (FAILED(Create_Materials()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffer()))
		return E_FAIL;

	if (FAILED(Compile_Shader(pShaderFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::NatvieConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CModel::SetUp_TextureOnShader(const char * pConstantName, _uint iMeshContainerIndex, aiTextureType eType)
{
	if (FAILED(__super::SetUp_TextureOnShader(pConstantName, m_Materials[m_MeshContainers[iMeshContainerIndex]->Get_MaterialIndex()]->pMeshTexture[eType])))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Bind_Buffers()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB
	};

	_uint				iStrides[] = {
		m_iStride
	};

	_uint				iOffsets[] = {
		0
	};

	/* 그려야할 버텍싀버퍼들을 장치에 바인드한다. */
	m_pDeviceContext->IASetVertexBuffers(0, 1, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshContainerIndex, _uint iPassIndex)
{
	m_pDeviceContext->IASetInputLayout(m_EffectDescs[iPassIndex]->pInputLayout);

	if (FAILED(m_EffectDescs[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_MeshContainers[iMeshContainerIndex]->Render();

	return S_OK;
}

HRESULT CModel::Reserve_VertexIndexData()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		m_iNumVertices += m_pScene->mMeshes[i]->mNumVertices;
		m_iNumPrimitive += m_pScene->mMeshes[i]->mNumFaces;
	}

	m_pVertices = new VTXMESH[m_iNumVertices];
	m_pFaceIndices = new FACEINDICES32[m_iNumPrimitive];

	return S_OK;
}

HRESULT CModel::Create_Materials()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_Materials.reserve(m_pScene->mNumMaterials);

	char		szMeshFilePath[MAX_PATH] = "";

	for (_uint i = 0; i < m_pScene->mNumMaterials; ++i)
	{
		aiMaterial*		pMaterial = m_pScene->mMaterials[i];

		MESHMATERIAL*		pMeshMaterial = new MESHMATERIAL;
		ZeroMemory(pMeshMaterial, sizeof(MESHMATERIAL));

		for (_uint j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString	strFilePath;

			if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &strFilePath)))
				continue;

			char	szFileName[MAX_PATH] = "";
			char	szExt[MAX_PATH] = "";
			_splitpath_s(strFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szMeshFilePath, m_szMeshFilePath);
			strcat_s(szMeshFilePath, szFileName);
			strcat_s(szMeshFilePath, szExt);

			_tchar		szFullName[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szMeshFilePath, strlen(szMeshFilePath), szFullName, MAX_PATH);

			pMeshMaterial->pMeshTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, szFullName);
			if (nullptr == pMeshMaterial->pMeshTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(pMeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Create_MeshContainer()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	_uint	iVertexIndex = 0;
	_uint	iFaceIndex = 0;

	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];

		CMeshContainer::MESHDESC		MeshDesc;
		ZeroMemory(&MeshDesc, sizeof(MeshDesc));

		MeshDesc.iNumFaces = pMesh->mNumFaces;
		MeshDesc.iStartVertexIndex = iVertexIndex;
		MeshDesc.iStartFaceIndex = iFaceIndex;
		MeshDesc.iMaterialIndex = pMesh->mMaterialIndex;

		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, MeshDesc);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_MeshContainers.push_back(pMeshContainer);

		for (_uint j = 0; j < pMesh->mNumVertices; ++j)
		{
			_vector		vPosition;
			memcpy(&vPosition, &pMesh->mVertices[j], sizeof(_float3));

			vPosition = XMVector3TransformCoord(vPosition, XMLoadFloat4x4(&m_PivotMatrix));

			XMStoreFloat3(&((VTXMESH*)m_pVertices)[iVertexIndex].vPosition, vPosition);



			memcpy(&((VTXMESH*)m_pVertices)[iVertexIndex].vNormal, &pMesh->mNormals[j], sizeof(_float3));
			memcpy(&((VTXMESH*)m_pVertices)[iVertexIndex].vTexUV, &pMesh->mTextureCoords[0][j], sizeof(_float2));
			memcpy(&((VTXMESH*)m_pVertices)[iVertexIndex].vTangent, &pMesh->mTangents[j], sizeof(_float3));
			++iVertexIndex;
		}

		for (_uint j = 0; j < pMesh->mNumFaces; ++j)
		{
			m_pFaceIndices[iFaceIndex]._0 = pMesh->mFaces[j].mIndices[0];
			m_pFaceIndices[iFaceIndex]._1 = pMesh->mFaces[j].mIndices[1];
			m_pFaceIndices[iFaceIndex]._2 = pMesh->mFaces[j].mIndices[2];
			++iFaceIndex;
		}
	}
	return S_OK;
}

HRESULT CModel::Create_VertexIndexBuffer()
{
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	/* D3D11_BUFFER_DESC */
	m_iStride = sizeof(VTXMESH);
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;

	/* D3D11_BUFFER_DESC */
	m_IndicesByteLength = sizeof(FACEINDICES32);
	m_iNumIndicesFigure = 3;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eFormat = DXGI_FORMAT_R32_UINT;

	m_IBDesc.ByteWidth = m_IndicesByteLength * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pFaceIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Compile_Shader(const _tchar * pShaderFilePath)
{
	D3D11_INPUT_ELEMENT_DESC		ElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(__super::Compile_ShaderFiles(pShaderFilePath, ElementDescs, 4)))
		return E_FAIL;

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * pMeshFilePath, const char * pMeshFileName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pMeshFilePath, pMeshFileName, pShaderFilePath, PivotMatrix)))
	{
		MSGBOX("Failed to Creating CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pFaceIndices);
	}

	for (auto& pMeshContainer : m_MeshContainers)
		Safe_Release(pMeshContainer);
	m_MeshContainers.clear();

	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(pMaterial->pMeshTexture[i]);

		Safe_Delete(pMaterial);
	}

	m_Materials.clear();
}
