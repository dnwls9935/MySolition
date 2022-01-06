#pragma once

#include "Base.h"

BEGIN(Engine)
class HierarchyNode final : public CBase
{

public: typedef struct tagHierarchyNodeDesc {
		char					m_BoneName[MAX_PATH] = "";

		HierarchyNode*			m_Parent = nullptr;
		_uint					m_Depth = 0;

		_float4x4				m_OffsetMatrix;
		_float4x4				m_TransformationMatrix;
		_float4x4				m_CombinedTrasformationMatrix;
	}HIERARCHY_DESE;

public:		explicit HierarchyNode(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
public:		virtual ~HierarchyNode() = default;

public:		_uint					GetDepth() const { return m_HierarchyDesc.m_Depth; };
public:		const char*		GetBoneName()const { return m_HierarchyDesc.m_BoneName; }
public:		void					ReserveChannel(_uint _numAnimation) { m_Channels.resize(_numAnimation); }
public:		HRESULT			AddChannel(class Channel* _channel) { m_Channels.push_back(_channel); return S_OK; };
public:		void					UpdateCombinedTransformationMatrix(_uint _animationIndex);
public:		_matrix			GetCombinedMatrix() const { return XMLoadFloat4x4(&m_HierarchyDesc.m_CombinedTrasformationMatrix); };

private:		HRESULT	NativeConstruct(HierarchyNode::HIERARCHY_DESE _HierarchyDesc);


private:		ID3D11Device*					dx11Device;
private:		ID3D11DeviceContext*		dx11DeviceContext;
private:		HIERARCHY_DESE			m_HierarchyDesc;
private:		vector<class Channel*>	m_Channels;

public:		static HierarchyNode* Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext,HierarchyNode::HIERARCHY_DESE _HierarchyDesc);
public:		virtual void Free() override;
};

END