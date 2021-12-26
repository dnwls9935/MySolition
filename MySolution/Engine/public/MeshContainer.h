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
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_MeshDesc.iMaterialIndex;
	}

public:
	HRESULT NativeConstruct(const MESHDESC& MeshDesc);
	HRESULT Render();
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	MESHDESC				m_MeshDesc;

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const MESHDESC& MeshDesc);
	virtual void Free() override;
};

END