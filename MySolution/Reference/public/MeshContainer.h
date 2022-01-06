#pragma once

#include "Base.h"

BEGIN(Engine)

class CMeshContainer final : public CBase
{
public:		typedef struct tagBoneDesc  
			{
				_float4x4				m_OffsetMatrix;
				class HierarchyNode*	m_Node;
			}BONEDESC;


public:		typedef struct tagMeshContainderDesc
			{
				_uint	iMaterialIndex = 0;
				_uint	iStartVertexIndex = 0;
				_uint	iStartFaceIndex = 0;
				_uint	iNumFaces = 0;
			}MESHDESC;

public:		explicit CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
public:		virtual ~CMeshContainer() = default;

public:		const MESHDESC& GetMeshDesc() const { return m_MeshDesc; }
public:		void	GetBoneMatrices(_matrix* _boneMatrices);

public:		HRESULT NativeConstruct(const MESHDESC& MeshDesc);
public:		HRESULT Render();


public:		HRESULT			AddBoneDesc(BONEDESC* _boneDesc);

private:	ID3D11Device*							m_pDevice = nullptr;
private:	ID3D11DeviceContext*				m_pDeviceContext = nullptr;
private:	MESHDESC									m_MeshDesc;
private:	vector<BONEDESC*>					m_Bones;
private:	typedef vector<BONEDESC*>		BONES;



public:		static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const MESHDESC& MeshDesc);
public:		virtual void Free() override;
};

END