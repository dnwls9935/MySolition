#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CVIBuffer
{
public:
	enum TYPE { TYPE_STATIC, TYPE_ANIM, TYPE_END };
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshContainer() {
		return (_uint)m_MeshContainers.size();
	}

	// Bone_ArmHand_R

	CHierarchyNode* Get_BoneMatrix(const char* pBoneName);
	_fmatrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}

public:
	HRESULT NativeConstruct_Prototype(const char * pMeshFilePath, const char * pMeshFileName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix, TYPE eMeshType);
	HRESULT NativeConstruct(void* pArg);
public:
	HRESULT SetUp_TextureOnShader(const char* pConstantName, _uint iMeshContainerIndex, aiTextureType eType);
	void SetUp_AnimationIndex(_uint iAnimationIndex) {
		m_iCurrentAnimation = iAnimationIndex;
	}
	HRESULT Bind_Buffers();
	HRESULT Update_CombinedTransformationMatrix(_double TimeDelta);
	HRESULT Render(_uint iMeshContainerIndex, _uint iPassIndex);

	TYPE GetMeshType()const { return m_eMeshType; }

	_bool	GetAnimationFinished();

private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;
private:
	char				m_szMeshFilePath[MAX_PATH] = "";
	FACEINDICES32*		m_pFaceIndices = nullptr;
	_float4x4			m_PivotMatrix;
	TYPE				m_eMeshType = TYPE_END;
	_uint				m_iCurrentAnimation = 0;

private:
	vector<class CMeshContainer*>			m_MeshContainers;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;

	vector<MESHMATERIAL*>				m_Materials;
	typedef vector<MESHMATERIAL*>		MESHMATERIALS;

	vector<class CHierarchyNode*>			m_HierarchyNodes;
	typedef vector<class CHierarchyNode*>	HIERARCHYNODES;

	vector<class CAnimation*>				m_Animations;
	typedef vector<class CAnimation*>		ANIMATIONS;


private:
	HRESULT Reserve_VertexIndexData();
	HRESULT Create_Materials();

	/* 모델을 구성하는 메시들의 정보를 구성한다. */
	HRESULT Create_MeshContainer();

	HRESULT Create_VertexIndexBuffer();

	HRESULT Compile_Shader(const _tchar* pShaderFilePath);

	HRESULT Create_HierarchyNode(aiNode* pNode, CHierarchyNode* pParent = nullptr, _uint iDepth = 0);

	HRESULT Create_SkinnedDesc();
	HRESULT Clone_SkinnedDesc();

	HRESULT Create_Animation();

private:
	CHierarchyNode* Find_HierarchyNode(const char* pName);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* pMeshFilePath, const char* pMeshFileName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix, TYPE eMeshType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END