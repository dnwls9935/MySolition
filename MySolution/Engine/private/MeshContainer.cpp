#include "..\public\MeshContainer.h"
#include "HierarchyNode.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

void CMeshContainer::GetBoneMatrices(_matrix * _boneMatrices)
{
	_uint pIndex = 0;

	for (auto& pBone : m_Bones)
		_boneMatrices[pIndex++] = XMMatrixTranspose(XMLoadFloat4x4(&pBone->m_OffsetMatrix) * pBone->m_Node->GetCombinedMatrix());

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

HRESULT CMeshContainer::AddBoneDesc(BONEDESC * _boneDesc)
{
	m_Bones.push_back(_boneDesc);

	return S_OK;
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


void CMeshContainer::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	for (auto& pBone : m_Bones)
		Safe_Delete(pBone);

	m_Bones.clear();
}
