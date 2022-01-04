#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CVIBuffer
{
public:
	enum class TYPE {TYPE_STATIC, TYPE_DYNAMIC, TYPE_END};
public:
	typedef struct tagModelDesc {
		const char*		mMeshFilePath = "";
		const char*		mMeshFileName = "";
		const _tchar*	mShaderFilePath = TEXT("");
		_matrix			mPivotMatrix;
		TYPE				mMeshType = TYPE::TYPE_END;
	}MODELDESC;
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshContainer() {
		return (_uint)m_MeshContainers.size();
	}
public:
	HRESULT NativeConstruct_Prototype(MODELDESC _modelDesc);
	HRESULT NatvieConstruct(void* pArg);
public:
	HRESULT SetUp_TextureOnShader(const char* pConstantName, _uint iMeshContainerIndex, aiTextureType eType);
	HRESULT Bind_Buffers();
	HRESULT Render(_uint iMeshContainerIndex, _uint iPassIndex);
private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;

private:
	char				m_szMeshFilePath[MAX_PATH] = "";

private:
	FACEINDICES32*		m_pFaceIndices = nullptr;

private:
	vector<class CMeshContainer*>				m_MeshContainers;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;

	vector<MESHMATERIAL*>					m_Materials;
	typedef vector<MESHMATERIAL*>		MESHMATERIALS;

	vector<class HierarchyNode*>				m_HierarchyNodes;
	typedef vector<class HierarchyNode*>	HIERARCHYNODES;

	vector<class Animation*>					m_Animations;
	typedef vector<class Animation*>	ANIMATIONS;

private:
	_float4x4							m_PivotMatrix;

private:
	TYPE		m_Type;

private:
	HRESULT Reserve_VertexIndexData();

	HRESULT Create_Materials();

	/* 모델을 구성하는 메시들의 정보를 구성한다. */
	HRESULT Create_MeshContainer();

	HRESULT Create_VertexIndexBuffer();

	HRESULT Compile_Shader(const _tchar* pShaderFilePath);

	HRESULT Create_HierachyNode(aiNode* _node, class HierarchyNode* _parent = nullptr, _uint _depth = 0);

	HRESULT	Create_SkinnedDesc();

	HRESULT Create_Animation();

private:
	HierarchyNode* Find_HierarchyNode(char* _name);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELDESC _modelDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END