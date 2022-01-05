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
	_matrix		pTransformationMatrix = m_Channels[_animationIndex]->GetTransformationMatrix();
	// 여기부터 짜야함
}

HRESULT HierarchyNode::NativeConstruct(HierarchyNode::HIERARCHY_DESE _HierarchyDesc)
{
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

	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
