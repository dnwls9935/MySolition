#include "..\public\HierarchyNode.h"

#include "Channel.h"

HierarchyNode::HierarchyNode(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: dx11Device(_dx11Device)
	, dx11DeviceContext(_dx11DeviceContext)
{
	Safe_AddRef(dx11Device);
	Safe_AddRef(dx11DeviceContext);
}

void HierarchyNode::UpdateCombinedTransformationMatrix(_uint _animationIndex)
{
	if (nullptr != m_Channels[_animationIndex])
		XMStoreFloat4x4(&m_HierarchyDesc.m_TransformationMatrix, m_Channels[_animationIndex]->GetTransformationMatrix());

	if (nullptr != m_HierarchyDesc.m_Parent) {
		XMStoreFloat4x4(&m_HierarchyDesc.m_CombinedTrasformationMatrix,
			XMLoadFloat4x4(&m_HierarchyDesc.m_TransformationMatrix)* XMLoadFloat4x4(&m_HierarchyDesc.m_Parent->m_HierarchyDesc.m_CombinedTrasformationMatrix));
	}
	else {
		XMStoreFloat4x4(&m_HierarchyDesc.m_CombinedTrasformationMatrix,
			XMLoadFloat4x4(&m_HierarchyDesc.m_TransformationMatrix)* XMMatrixIdentity() );
	}
}

HRESULT HierarchyNode::NativeConstruct(HierarchyNode::HIERARCHY_DESE _HierarchyDesc)
{
	XMStoreFloat4x4(&_HierarchyDesc.m_TransformationMatrix, XMMatrixTranspose( XMLoadFloat4x4(&_HierarchyDesc.m_TransformationMatrix)));
	XMStoreFloat4x4(&_HierarchyDesc.m_CombinedTrasformationMatrix, XMMatrixIdentity());

	m_HierarchyDesc = _HierarchyDesc;
	Safe_AddRef(m_HierarchyDesc.m_Parent);
	return S_OK;
}

HierarchyNode * HierarchyNode::Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext, HierarchyNode::HIERARCHY_DESE _HierarchyDesc)
{
	HierarchyNode* pInstance = new HierarchyNode(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstruct(_HierarchyDesc)))
	{
		MSGBOX("Failed to Create HierachyNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void HierarchyNode::Free()
{
	Safe_Release(dx11Device);
	Safe_Release(dx11DeviceContext);

	Safe_Release(m_HierarchyDesc.m_Parent);

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
