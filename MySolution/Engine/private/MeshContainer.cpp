#include "..\public\MeshContainer.h"
#include "HierarchyNode.h"


CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_MeshDesc(rhs.m_MeshDesc)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CMeshContainer::NativeConstruct(const MESHDESC & MeshDesc)
{
	memcpy(&m_MeshDesc, &MeshDesc, sizeof(MESHDESC));

	return S_OK;
}

HRESULT CMeshContainer::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->DrawIndexed(m_MeshDesc.iNumFaces * 3, m_MeshDesc.iStartFaceIndex * 3, m_MeshDesc.iStartVertexIndex);

	return S_OK;
}


HRESULT CMeshContainer::Add_BoneDesc(BONEDESC * pBoneDesc)
{
	m_Bones.push_back(pBoneDesc);

	return S_OK;
}

void CMeshContainer::SetUp_BoneMatrices(_matrix * pBoneMatrices, _fmatrix _pivotMatrix)
{
	_uint		iIndex = 0;

	for (auto& pBone : m_Bones)
	{
		pBoneMatrices[iIndex++] = _pivotMatrix * XMMatrixTranspose(XMLoadFloat4x4(&pBone->OffsetMatrix) * pBone->pNode->Get_CombinedMatrix());
	}
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const MESHDESC& MeshDesc)
{
	CMeshContainer*		pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(MeshDesc)))
	{
		MSGBOX("Failed to Creating CMeshContainer");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CMeshContainer * CMeshContainer::Clone()
{
	return new CMeshContainer(*this);
}


void CMeshContainer::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	for (auto& pBone : m_Bones)
		Safe_Delete(pBone);

	m_Bones.clear();
}
