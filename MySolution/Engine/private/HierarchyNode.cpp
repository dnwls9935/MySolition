#include "..\public\HierarchyNode.h"
#include "Channel.h"

CHierarchyNode::CHierarchyNode(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CHierarchyNode::NativeConstruct(char* pBoneName, _fmatrix TransformationMatrix, _uint iDepth, CHierarchyNode* pParent)
{
	strcpy_s(m_szBoneName, pBoneName);
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(TransformationMatrix));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	m_iDepth = iDepth;

	m_pParent = pParent;
	Safe_AddRef(m_pParent);

	return S_OK;
}

/* 모든 뼈의 TransformationMatrix가 애니메이션에 맞게 셋팅된 이후에 호출된다. */
/* 현재 노드의 CombineTransformationMatrix를 셋팅해놓기위한 함수다. */
void CHierarchyNode::Update_CombinedTransformationMatrix(_uint iAnimationIndex)
{
	if (nullptr != m_Channels[iAnimationIndex])
	{
		XMStoreFloat4x4(&m_TransformationMatrix, m_Channels[iAnimationIndex]->Get_TransformMatrix());
	}

	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));

	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMMatrixIdentity());
}

CHierarchyNode * CHierarchyNode::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, char * pBoneName, _fmatrix TransformationMatrix, _uint iDepth, CHierarchyNode * pParent)
{
	CHierarchyNode*		pInstance = new CHierarchyNode(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pBoneName, TransformationMatrix, iDepth, pParent)))
	{
		MSGBOX("Failed to Creating CMeshContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{

	Safe_Release(m_pParent);

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();

}
