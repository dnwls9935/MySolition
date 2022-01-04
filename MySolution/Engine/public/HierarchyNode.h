#pragma once

#include "Base.h"

BEGIN(Engine)
class HierarchyNode final : public CBase
{
public:
	typedef struct tagHierarchyNodeDesc {
		char					m_BoneName[MAX_PATH] = "";

		HierarchyNode*			m_Parent = nullptr;
		_uint					m_Depth = 0;

		_float4x4				m_OffsetMatrix;
		_float4x4				m_TransformationMatrix;
		_float4x4				m_CombinedTrasformationMatrix;
	}HIERARCHY_DESE;

public:
	explicit HierarchyNode(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual ~HierarchyNode() = default;

	_uint GetDepth() const { return m_HierarchyDesc.m_Depth; };
	const char* GetBoneName()const { return m_HierarchyDesc.m_BoneName; }

private:
	HRESULT	NativeConstruct(HierarchyNode::HIERARCHY_DESE _HierarchyDesc);

private:
	ID3D11Device*			dx11Device;
	ID3D11DeviceContext*	dx11DeviceContext;

private:
	HIERARCHY_DESE	m_HierarchyDesc;

public:
	static HierarchyNode* Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext,HierarchyNode::HIERARCHY_DESE _HierarchyDesc);
	virtual void Free() override;
};

END