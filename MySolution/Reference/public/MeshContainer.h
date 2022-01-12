#pragma once

#include "Base.h"

BEGIN(Engine)

class CMeshContainer final : public CBase
{
public:
	typedef struct tagMeshContainderDesc
	{
		_uint	iMaterialIndex = 0;
		_uint	iStartVertexIndex = 0;
		_uint	iStartFaceIndex = 0;
		_uint	iNumFaces = 0;
	}MESHDESC;
public:
	explicit CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_MeshDesc.iMaterialIndex;
	}

	const MESHDESC& Get_MeshContainerDesc() const {
		return m_MeshDesc;
	}

public:
	HRESULT NativeConstruct(const MESHDESC& MeshDesc);
	HRESULT Render();


public:
	HRESULT Add_BoneDesc(BONEDESC* pBoneDesc);
	void SetUp_BoneMatrices(_matrix* pBoneMatrices);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	MESHDESC				m_MeshDesc;

private:
	vector<BONEDESC*>			m_Bones;
	typedef vector<BONEDESC*>	BONES;

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const MESHDESC& MeshDesc);
	CMeshContainer* Clone();
	virtual void Free() override;
};

END