#include "..\public\MeshContainer.h"


CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
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
}
