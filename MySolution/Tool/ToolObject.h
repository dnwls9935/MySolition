#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END
class ToolObject final : public CGameObject
{
public:
	typedef struct tagToolObjectDesc {
		const _tchar*	m_BufferTag = TEXT("");
		_float3			m_Position = { 0.f, 0.f, 0.f };
	}TOOLOBJDESC;

private:
	explicit ToolObject(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit ToolObject(const ToolObject& rhs);
	virtual ~ToolObject() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	TOOLOBJDESC			m_ToolObjDesc;

private:
	virtual HRESULT SetUp_Components();
	
public:
	static ToolObject*		Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void Free() override;
};

