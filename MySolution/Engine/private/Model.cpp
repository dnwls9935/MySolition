#include "..\public\Model.h"
#include "MeshContainer.h"
#include "HierarchyNode.h"
#include "Texture.h"
#include "Channel.h"
#include "Animation.h"


CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CModel::CModel(const CModel & rhs)
	: CVIBuffer(rhs)
	, m_pFaceIndices(rhs.m_pFaceIndices)
	, m_pScene(rhs.m_pScene)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iCurrentAnimation(rhs.m_iCurrentAnimation)
	, m_eMeshType(rhs.m_eMeshType)
{
	for (auto& pMeshContainer : rhs.m_MeshContainers)
	{
		m_MeshContainers.push_back(pMeshContainer->Clone());
	}

	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(pMaterial->pMeshTexture[i]);
	}
}

CHierarchyNode* CModel::Get_BoneMatrix(const char * pBoneName)
{
	return Find_HierarchyNode(pBoneName);
}

HRESULT CModel::NativeConstruct_Prototype(const char * pMeshFilePath, const char * pMeshFileName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix, TYPE eMeshType)
{
	m_eMeshType = eMeshType;

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

	if (FAILED(Compile_Shader(pShaderFilePath)))
		return E_FAIL;

	if (TYPE_STATIC == m_eMeshType)
	{
		if (FAILED(Create_VertexIndexBuffer()))
			return E_FAIL;

		return S_OK;
	}

	if (FAILED(Create_HierarchyNode(m_pScene->mRootNode)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(Create_SkinnedDesc()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffer()))
		return E_FAIL;

	if (FAILED(Create_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	if (TYPE_STATIC == m_eMeshType)
		return S_OK;

	if (FAILED(Create_HierarchyNode(m_pScene->mRootNode)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	if (FAILED(Clone_SkinnedDesc()))
		return E_FAIL;

	if (FAILED(Create_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::SetUp_TextureOnShader(const char * pConstantName, _uint iMeshContainerIndex, aiTextureType eType)
{
	if (FAILED(__super::SetUp_TextureOnShader(pConstantName, m_Materials[m_MeshContainers[iMeshContainerIndex]->Get_MaterialIndex()]->pMeshTexture[eType])))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::SetUp_TextureOnShader(const char * pConstantName, CTexture * _TextureCom, _int _TextureIndex)
{
	if (FAILED(__super::SetUp_TextureOnShader(pConstantName, _TextureCom, _TextureIndex)))
		return E_FAIL;

	return S_OK;
}

void CModel::SetUp_AnimationIndex(_uint iAnimationIndex)
{
//	m_Animations[m_iCurrentAnimation]->SetTrackPositionAcc(0.0);
	m_iCurrentAnimation = iAnimationIndex;
}

_int CModel::GetCurrentAnimationFrame()
{
	return (_int)m_Animations[m_iCurrentAnimation]->GetTrackPositionAcc();
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

/* 매 프레임마다 호출. */
HRESULT CModel::Update_CombinedTransformationMatrix(_double TimeDelta)
{
	/* 현재 애니메이션 재생시간에 따른 뼈들의 TransformationMatrix를 갱신한다. */
	m_Animations[m_iCurrentAnimation]->Update_TransformationMatrix(TimeDelta);

	/* 렌더링해야할 CombinedTransfromkationMatrix를 만든다. */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimation);
	}

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshContainerIndex, _uint iPassIndex)
{
	m_pDeviceContext->IASetInputLayout(m_EffectDescs[iPassIndex]->pInputLayout);


	_matrix		BoneMatrices[128];
	ZeroMemory(BoneMatrices, sizeof(_matrix) * 128);

	m_MeshContainers[iMeshContainerIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

	if (FAILED(SetUp_ValueOnShader("g_BoneMatrices", BoneMatrices, sizeof(_matrix) * 128)))
		return E_FAIL;

	if (FAILED(m_EffectDescs[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;


	m_MeshContainers[iMeshContainerIndex]->Render();

	return S_OK;
}

_bool CModel::GetAnimationFinished()
{
	return  m_Animations[m_iCurrentAnimation]->GetFinished();
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
	ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pFaceIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pFaceIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

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

				if (j == aiTextureType_OPACITY)
				{
					pMeshMaterial->pMeshTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Burned_Mask.png"));
					if (nullptr == pMeshMaterial->pMeshTexture[j])
						return E_FAIL;
				}

			if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &strFilePath)))
				continue;

			char	szFileName[MAX_PATH] = "";
			char	szExt[MAX_PATH] = "";
			_splitpath_s(strFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szMeshFilePath, m_szMeshFilePath);
			strcat_s(szMeshFilePath, szFileName);
			strcat_s(szMeshFilePath, szExt);

			_tchar		szFullName[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szMeshFilePath, (_int)strlen(szMeshFilePath), szFullName, MAX_PATH);

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

			if (TYPE_STATIC == m_eMeshType)
				vPosition = XMVector3TransformCoord(vPosition, XMLoadFloat4x4(&m_PivotMatrix));

			XMStoreFloat3(&((VTXMESH*)m_pVertices)[iVertexIndex].vPosition, vPosition);

			_vector			vNormal;
			memcpy(&vNormal, &pMesh->mNormals[j], sizeof(_float3));
			vNormal = XMVectorSetW(vNormal, 0.f);
			vNormal = XMVector3Transform(vNormal, XMLoadFloat4x4(&m_PivotMatrix));
			XMStoreFloat3(&((VTXMESH*)m_pVertices)[iVertexIndex].vNormal, vNormal);

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
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	if (FAILED(__super::Compile_ShaderFiles(pShaderFilePath, ElementDescs, 6)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Create_HierarchyNode(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	_matrix			TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));

	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(m_pDevice, m_pDeviceContext, pNode->mName.data, TransformationMatrix, iDepth, pParent);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	pHierarchyNode->Reserve_Channel(m_pScene->mNumAnimations);

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
		Create_HierarchyNode(pNode->mChildren[i], pHierarchyNode, iDepth + 1);

	return S_OK;
}

HRESULT CModel::Create_SkinnedDesc()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];
		CMeshContainer*	pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			aiBone*		pBone = pMesh->mBones[j];

			BONEDESC*	pBoneDesc = new BONEDESC;
			_matrix		OffSetMatrix;

			OffSetMatrix = XMMatrixTranspose(_matrix(pBone->mOffsetMatrix[0]));
			XMStoreFloat4x4(&pBoneDesc->OffsetMatrix, OffSetMatrix);

			pBoneDesc->pNode = Find_HierarchyNode(pBone->mName.data);
			if (nullptr == pBoneDesc->pNode)
				return E_FAIL;

			pBoneDesc->pNode->Set_OffsetMatrix(OffSetMatrix * XMLoadFloat4x4(&m_PivotMatrix));

			pMeshContainer->Add_BoneDesc(pBoneDesc);

			/* 몇개 정점에 영향을 주고 있는지?! */
			for (_uint k = 0; k < pBone->mNumWeights; ++k)
			{
				CMeshContainer::MESHDESC	MeshDesc = pMeshContainer->Get_MeshContainerDesc();

				_uint	iVertexIndex = MeshDesc.iStartVertexIndex + pBone->mWeights[k].mVertexId;

				if (0.0f == ((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.x)
				{
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendIndex.x = j;
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.x = pBone->mWeights[k].mWeight;
				}
				else if (0.0f == ((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.y)
				{
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendIndex.y = j;
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.y = pBone->mWeights[k].mWeight;
				}

				else if (0.0f == ((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.z)
				{
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendIndex.z = j;
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.z = pBone->mWeights[k].mWeight;
				}
				else
				{
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendIndex.w = j;
					((VTXMESH*)m_pVertices)[iVertexIndex].vBlendWeight.w = pBone->mWeights[k].mWeight;
				}
			}
		}
	}

	return S_OK;
}

HRESULT CModel::Clone_SkinnedDesc()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];
		CMeshContainer*	pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pMesh->mNumBones; ++j)
		{
			aiBone*		pBone = pMesh->mBones[j];

			BONEDESC*	pBoneDesc = new BONEDESC;
			_matrix		OffSetMatrix;

			OffSetMatrix = XMMatrixTranspose(_matrix(pBone->mOffsetMatrix[0]));
			XMStoreFloat4x4(&pBoneDesc->OffsetMatrix, OffSetMatrix);

			pBoneDesc->pNode = Find_HierarchyNode(pBone->mName.data);
			if (nullptr == pBoneDesc->pNode)
				return E_FAIL;

			pBoneDesc->pNode->Set_OffsetMatrix(OffSetMatrix * XMLoadFloat4x4(&m_PivotMatrix));

			pMeshContainer->Add_BoneDesc(pBoneDesc);
		}
	}

	return S_OK;
}

HRESULT CModel::Create_Animation()
{
	for (_uint i = 0; i < m_pScene->mNumAnimations; ++i)
	{
		aiAnimation*	pAnim = m_pScene->mAnimations[i];

		CAnimation*		pAnimation = CAnimation::Create(pAnim->mName.data, pAnim->mDuration, pAnim->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;

		/* 현재 애니메이션 영햐을 주는 뼈대들 */
		for (_uint j = 0; j < pAnim->mNumChannels; ++j)
		{
			aiNodeAnim*		pAnimChannel = pAnim->mChannels[j];
			CHierarchyNode*	pHierarchyNode = Find_HierarchyNode(pAnimChannel->mNodeName.data);
			CChannel*		pChannel = CChannel::Create(pAnimChannel->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;

			_uint iNumKeyframes = max(pAnimChannel->mNumPositionKeys, pAnimChannel->mNumRotationKeys);
			iNumKeyframes = max(iNumKeyframes, pAnimChannel->mNumScalingKeys);

			_vector			vScale, vRotation, vPosition;
			ZeroMemory(&vScale, sizeof(_vector));
			ZeroMemory(&vRotation, sizeof(_vector));
			ZeroMemory(&vPosition, sizeof(_vector));

			for (_uint k = 0; k < iNumKeyframes; ++k)
			{
				KEYFRAME*			pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				if (pAnimChannel->mNumPositionKeys > k)
				{
					memcpy(&vPosition, &pAnimChannel->mPositionKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pAnimChannel->mPositionKeys[k].mTime;
					vPosition = XMVectorSetW(vPosition, 1.f);
				}

				if (pAnimChannel->mNumScalingKeys > k)
				{
					memcpy(&vScale, &pAnimChannel->mScalingKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pAnimChannel->mScalingKeys[k].mTime;
				}

				if (pAnimChannel->mNumRotationKeys > k)
				{
					// memcpy(&pKeyFrame->vRotation, &pAnimChannel->mRotationKeys[k].mValue, sizeof(_float4));
					vRotation = XMVectorSetX(vRotation, pAnimChannel->mRotationKeys[k].mValue.x);
					vRotation = XMVectorSetY(vRotation, pAnimChannel->mRotationKeys[k].mValue.y);
					vRotation = XMVectorSetZ(vRotation, pAnimChannel->mRotationKeys[k].mValue.z);
					vRotation = XMVectorSetW(vRotation, pAnimChannel->mRotationKeys[k].mValue.w);

					pKeyFrame->Time = pAnimChannel->mRotationKeys[k].mTime;
				}

				memcpy(&pKeyFrame->vScale, &vScale, sizeof(_float3));
				memcpy(&pKeyFrame->vRotation, &vRotation, sizeof(_float4));
				memcpy(&pKeyFrame->vPosition, &vPosition, sizeof(_float3));

				pChannel->Add_KeyFrame(pKeyFrame);

			}
			pHierarchyNode->Add_Channel(i, pChannel);
			pAnimation->Add_Channel(pChannel);
		}
		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

CHierarchyNode * CModel::Find_HierarchyNode(const char * pName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
	{
		return !strcmp(pNode->Get_Name(), pName);
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return (*iter);
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const char * pMeshFilePath, const char * pMeshFileName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix, TYPE eMeshType)
{
	CModel*		pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pMeshFilePath, pMeshFileName, pShaderFilePath, PivotMatrix, eMeshType)))
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

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pNode : m_HierarchyNodes)
		Safe_Release(pNode);

	m_HierarchyNodes.clear();

	for (auto& pMeshContainer : m_MeshContainers)
		Safe_Release(pMeshContainer);
	m_MeshContainers.clear();

	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(pMaterial->pMeshTexture[i]);
		if (false == m_isCloned)
			Safe_Delete(pMaterial);
	}

	m_Materials.clear();
}
