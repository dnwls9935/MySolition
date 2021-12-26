#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CVIBuffer
{
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshContainer() {
		return (_uint)m_MeshContainers.size();
	}
public:
	HRESULT NativeConstruct_Prototype(const char * pMeshFilePath, const char * pMeshFileName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix);
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
	/* 모든 메시들의 정점정보를 하나의 배열에 모아둔다. */
	/* 정점버퍼를 생성한다. */
	//VTXMESH*			m_pVertices = nullptr;


	FACEINDICES32*		m_pFaceIndices = nullptr;


private:
	vector<class CMeshContainer*>			m_MeshContainers;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;

	vector<MESHMATERIAL*>				m_Materials;
	typedef vector<MESHMATERIAL*>		MESHMATERIALS;

private:
	_float4x4							m_PivotMatrix;

private:
	HRESULT Reserve_VertexIndexData();

	HRESULT Create_Materials();

	/* 모델을 구성하는 메시들의 정보를 구성한다. */
	HRESULT Create_MeshContainer();

	HRESULT Create_VertexIndexBuffer();

	HRESULT Compile_Shader(const _tchar* pShaderFilePath);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const char* pMeshFilePath, const char* pMeshFileName, const _tchar* pShaderFilePath, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END