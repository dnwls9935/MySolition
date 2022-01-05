#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CVIBuffer
{
public:		enum class TYPE {TYPE_STATIC, TYPE_DYNAMIC, TYPE_END};
public:		typedef struct tagModelDesc {
		const char*		mMeshFilePath = "";
		const char*		mMeshFileName = "";
		const _tchar*	mShaderFilePath = TEXT("");
		_matrix			mPivotMatrix;
		TYPE				mMeshType = TYPE::TYPE_END;
	}MODELDESC;

private:		explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
private:		explicit CModel(const CModel& rhs);
private:		virtual ~CModel() = default;

public:		_uint Get_NumMeshContainer() {	return (_uint)m_MeshContainers.size(); }

public:		HRESULT	NativeConstruct_Prototype(MODELDESC _modelDesc);
public:		HRESULT	NatvieConstruct(void* pArg);
public:		HRESULT	SetUp_TextureOnShader(const char* pConstantName, _uint iMeshContainerIndex, aiTextureType eType);
public:		HRESULT	Bind_Buffers();
public:		HRESULT	Render(_uint iMeshContainerIndex, _uint iPassIndex);

private:		HRESULT	UpdateCombinedTrasnformationMatrix(_double _timeDelta);

private:		const aiScene*			m_pScene = nullptr;
private:		Assimp::Importer		m_Importer;
private:		char							m_szMeshFilePath[MAX_PATH] = "";
private:		FACEINDICES32*		m_pFaceIndices = nullptr;
private:		_float4x4					m_PivotMatrix;
private:		TYPE						m_Type;
private:		_uint							m_CurrentAnimation = 0;

private:		vector<class CMeshContainer*>				m_MeshContainers;
private:		typedef vector<class CMeshContainer*>	MESHCONTAINERS;

private:		vector<MESHMATERIAL*>					m_Materials;
private:		typedef vector<MESHMATERIAL*>		MESHMATERIALS;

private:		vector<class HierarchyNode*>				m_HierarchyNodes;
private:		typedef vector<class HierarchyNode*>	HIERARCHYNODES;

private:		vector<class Animation*>					m_Animations;
private:		typedef vector<class Animation*>	ANIMATIONS;


private:		HRESULT Reserve_VertexIndexData();
private:		HRESULT Create_Materials();
private:		HRESULT Create_MeshContainer();
private:		HRESULT Create_VertexIndexBuffer();
private:		HRESULT Compile_Shader(const _tchar* pShaderFilePath);
private:		HRESULT Create_HierachyNode(aiNode* _node, class HierarchyNode* _parent = nullptr, _uint _depth = 0);
private:		HRESULT	Create_SkinnedDesc();
private:		HRESULT Create_Animation();

private:		HierarchyNode* Find_HierarchyNode(char* _name);

public:		static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, MODELDESC _modelDesc);
public:		virtual CComponent* Clone(void* pArg) override;
public:		virtual void Free() override;
};

END