#include "..\public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Channel.h"


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
	, m_CurrentAnimation(rhs.m_CurrentAnimation)
	, m_Animations(rhs.m_Animations)
	, m_HierarchyNodes(rhs.m_HierarchyNodes)
{
	for (auto& pMeshContainer : m_MeshContainers)
		Safe_AddRef(pMeshContainer);

	for (auto& pMaterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(pMaterial->pMeshTexture[i]);
	}

	for (auto& pAnim : m_Animations)
		Safe_AddRef(pAnim);
	for (auto& pNode : m_HierarchyNodes)
		Safe_AddRef(pNode);
}

HRESULT CModel::NativeConstruct_Prototype(MODELDESC _modelDesc)
{
	XMStoreFloat4x4(&m_PivotMatrix, _modelDesc.mPivotMatrix);
	m_Type = _modelDesc.mMeshType;
	
	strcpy_s(m_szMeshFilePath, _modelDesc.mMeshFilePath);

	char		szFullPath[MAX_PATH] = "";
	strcpy_s(szFullPath, _modelDesc.mMeshFilePath);
	strcat_s(szFullPath, _modelDesc.mMeshFileName);

	m_pScene = m_Importer.ReadFile(szFullPath, aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);
	if (nullptr == m_pScene)
		return E_FAIL;

	if (FAILED(Reserve_VertexIndexData()))
		return E_FAIL;

	if (FAILED(Create_MeshContainer()))
		return E_FAIL;

	if (FAILED(Create_Materials()))
		return E_FAIL;

	if (FAILED(Compile_Shader(_modelDesc.mShaderFilePath)))
		return E_FAIL;

	if (TYPE::TYPE_STATIC == m_Type)
	{
		if (FAILED(Create_VertexIndexBuffer()))
			return E_FAIL;

		return S_OK;
	}

	if (FAILED(Create_HierachyNode(m_pScene->mRootNode)))
		return E_FAIL;

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](HierarchyNode* _sour, HierarchyNode* _dest) {
		return _sour->GetDepth() < _dest->GetDepth();
	});

	if (FAILED(Create_SkinnedDesc()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffer()))
		return E_FAIL;

	if (FAILED(Create_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::NatvieConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CModel::SetUp_TextureOnShader(const char * pConstantName, _uint iMeshContainerIndex, aiTextureType eType)
{
	if (FAILED(__super::SetUp_TextureOnShader(pConstantName, m_Materials[m_MeshContainers[iMeshContainerIndex]->GetMeshDesc().iMaterialIndex]->pMeshTexture[eType])))
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

	_matrix	BoneMatrices[128];
	ZeroMemory(&BoneMatrices, sizeof(_matrix) * 128);


	m_MeshContainers[iMeshContainerIndex]->GetBoneMatrices(BoneMatrices);

	if (FAILED(SetUp_ValueOnShader(("g_BoneMatrices"), BoneMatrices, sizeof(_matrix) * 128)))
		return E_FAIL;
	
	if (FAILED(m_EffectDescs[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_MeshContainers[iMeshContainerIndex]->Render();

	return S_OK;
}

HRESULT CModel::UpdateCombinedTrasnformationMatrix(_double _timeDelta)
{
	m_Animations[m_CurrentAnimation]->UpdateTransformationMatrix(_timeDelta);
	
	for (auto& pHierachyNode : m_HierarchyNodes)
		pHierachyNode->UpdateCombinedTransformationMatrix(m_CurrentAnimation);

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
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(__super::Compile_ShaderFiles(pShaderFilePath, ElementDescs, 6)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Create_HierachyNode(aiNode * _node, HierarchyNode * _parent, _uint _depth)
{
	_matrix pTransformationMatrix;
	memcpy(&pTransformationMatrix, &_node->mTransformation, sizeof(_matrix));

	HierarchyNode::HIERARCHY_DESE pHierarchyNodeDesc;
	strcpy_s(pHierarchyNodeDesc.m_BoneName, /*strlen(_node->mName.data)*/MAX_PATH, _node->mName.data);
	XMStoreFloat4x4(&pHierarchyNodeDesc.m_CombinedTrasformationMatrix, pTransformationMatrix);

	pHierarchyNodeDesc.m_Depth = _depth;
	pHierarchyNodeDesc.m_Parent = _parent;

	HierarchyNode* pHierarchyNode = HierarchyNode::Create(m_pDevice, m_pDeviceContext, pHierarchyNodeDesc);
	if (nullptr == pHierarchyNode)
		return E_FAIL;


	pHierarchyNode->ReserveChannel(m_pScene->mNumAnimations);
	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < _node->mNumChildren; i++)
		Create_HierachyNode(_node->mChildren[i], pHierarchyNode, _depth +1);

	return S_OK;
}

HRESULT CModel::Create_SkinnedDesc()
{
	for (_uint i = 0; i < m_pScene->mNumMeshes; i++) 
	{
		aiMesh* pMesh = m_pScene->mMeshes[i];
		CMeshContainer* pMeshContainer = m_MeshContainers[i];

		for (_uint j = 0; j < pMesh->mNumBones; j++)
		{
			aiBone* pBone = pMesh->mBones[j];

			CMeshContainer::BONEDESC* pBoneDesc = new CMeshContainer::BONEDESC;
			_matrix		offsetMatrix;

			memcpy(&offsetMatrix, &pBone->mOffsetMatrix, sizeof(_float4x4));
			XMStoreFloat4x4(&pBoneDesc->m_OffsetMatrix, XMMatrixTranspose(offsetMatrix));

			pBoneDesc->m_Node = Find_HierarchyNode(pBone->mName.data);
			if (nullptr == pBoneDesc->m_Node)
				return E_FAIL;

			pMeshContainer->AddBoneDesc(pBoneDesc) ;
			
			for (_uint k = 0; k < pBone->mNumWeights; k++)
			{
				CMeshContainer::MESHDESC		pMeshDesc = pMeshContainer->GetMeshDesc();
				_uint pVertexIdx = pMeshDesc.iStartVertexIndex + pBone->mWeights[k].mVertexId;

				if (0.0f == ((VTXMESH*)m_pVertices)[pVertexIdx].vBlendWeight.x)
				{
					((VTXMESH*)m_pVertices)[pVertexIdx].vBlendIndex.x = j;
					((VTXMESH*)m_pVertices)[pVertexIdx].vBlendWeight.x = pBone->mWeights[k].mWeight;
				}
				else if (0.0f == ((VTXMESH*)m_pVertices)[pVertexIdx].vBlendWeight.y)
				{
					((VTXMESH*)m_pVertices)[pVertexIdx].vBlendIndex.y = j;
					((VTXMESH*)m_pVertices)[pVertexIdx].vBlendWeight.y = pBone->mWeights[k].mWeight;
				}
				else if (0.0f == ((VTXMESH*)m_pVertices)[pVertexIdx].vBlendWeight.z)
				{
					((VTXMESH*)m_pVertices)[pVertexIdx].vBlendIndex.z = j;
					((VTXMESH*)m_pVertices)[pVertexIdx].vBlendWeight.z = pBone->mWeights[k].mWeight;
				}
				else if (0.0f == ((VTXMESH*)m_pVertices)[pVertexIdx].vBlendWeight.w)
				{
					((VTXMESH*)m_pVertices)[pVertexIdx].vBlendIndex.w = j;
					((VTXMESH*)m_pVertices)[pVertexIdx].vBlendWeight.w = pBone->mWeights[k].mWeight;
				}

			}

		}
	}
	return S_OK;
}

HRESULT CModel::Create_Animation()
{
	for (_uint i = 0; i < m_pScene->mNumAnimations; i++)
	{
		aiAnimation* pAiAnim = m_pScene->mAnimations[i];
		Animation::ANIMDESC pAnimDesc;
		ZeroMemory(&pAnimDesc, sizeof(pAnimDesc));

		strcpy_s(pAnimDesc.m_AnimationName, pAiAnim->mName.data);
		pAnimDesc.m_Duration = pAiAnim->mDuration;
		pAnimDesc.m_TrackPlaySpeed = pAiAnim->mTicksPerSecond;

		Animation*	pAnimation = Animation::Create(pAnimDesc);
		if (nullptr == pAnimation)
			return E_FAIL;

		for (_uint j = 0; j < pAiAnim->mNumChannels; j++)
		{
			aiNodeAnim* pAiAnimChannel = pAiAnim->mChannels[j];
			
			Channel* pChannel = Channel::Create(pAiAnimChannel->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;
			HierarchyNode*		pHierarchyNode = Find_HierarchyNode(pAiAnimChannel->mNodeName.data);
			if (nullptr == pHierarchyNode)
				return E_FAIL;

			_uint pNumKeyFrames = max(pAiAnimChannel->mNumPositionKeys, pAiAnimChannel->mNumRotationKeys);
			pNumKeyFrames = max(pNumKeyFrames, pAiAnimChannel->mNumScalingKeys);

			for (_uint k = 0; k < pNumKeyFrames; k++)
			{
				Channel::KEYFRAME pKeyFrame;
				ZeroMemory(&pKeyFrame, sizeof(pKeyFrame));

				if (pAiAnimChannel->mNumPositionKeys > k)
				{
					memcpy(&pKeyFrame.m_Position, &pAiAnimChannel->mPositionKeys[k].mValue , sizeof(_float3));
					pKeyFrame.m_Time = pAiAnimChannel->mPositionKeys[k].mTime;
				}
				if (pAiAnimChannel->mNumRotationKeys > k)
				{
					pKeyFrame.m_Rotation.x = pAiAnimChannel->mRotationKeys[k].mValue.x;
					pKeyFrame.m_Rotation.y = pAiAnimChannel->mRotationKeys[k].mValue.y;
					pKeyFrame.m_Rotation.z = pAiAnimChannel->mRotationKeys[k].mValue.z;
					pKeyFrame.m_Rotation.w = pAiAnimChannel->mRotationKeys[k].mValue.w;

					pKeyFrame.m_Time = pAiAnimChannel->mRotationKeys[k].mTime;
				}
				if (pAiAnimChannel->mNumScalingKeys > k)
				{
					memcpy(&pKeyFrame.m_Scale, &pAiAnimChannel->mScalingKeys[k].mValue, sizeof(_float3));
					pKeyFrame.m_Time = pAiAnimChannel->mScalingKeys[k].mTime;
				}

				pChannel->AddKeyFrame(pKeyFrame);
			}

			pHierarchyNode->AddChannel(pChannel);
			pAnimation->AddChannel(pChannel);
		}

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}

HierarchyNode * CModel::Find_HierarchyNode(char * _name)
{
	auto iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](HierarchyNode* _node) {
		return !strcmp(_node->GetBoneName(), _name);
	});

	if (m_HierarchyNodes.end() == iter)
		return nullptr;

	return (*iter);
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, MODELDESC _modelDesc)
{
	CModel*		pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(_modelDesc)))
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

	for (auto& pAnim : m_Animations)
		Safe_Release(pAnim);
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
