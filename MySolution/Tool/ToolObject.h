#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CModel;
class CVIBuffer_ThreeWaySystem;
END
class ToolObject final : public CGameObject
{
public:
	typedef struct tagToolObjectDesc {
		_tchar	m_BufferTag[MAX_PATH] = TEXT("");
		_tchar  m_ObjTag[MAX_PATH] = TEXT("");
		_float3			m_Position = { 0.f, 0.f, 0.f };
		_float4x4		m_pTransformMatrix = _float4x4();
		_bool			loadCheck = FALSE;
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

	_bool		GetPickCheckState() { return m_PickChecking; };
	TOOLOBJDESC GetObjectDesc() { return m_ToolObjDesc; }
	_matrix GetTransformMatrix()const { return m_pTransformCom->Get_WorldMatrix(); }

private:
	void	CheckButton();
	_bool	PickingObject();
	void	RenderThreeWaySystem();

private:
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CVIBuffer_ThreeWaySystem*	m_pThreeWaySystem = nullptr;

	TOOLOBJDESC			m_ToolObjDesc;
	bool				m_PickChecking = FALSE;
	_float		m_mouseSenitive = 0.05f;

private:
	virtual HRESULT SetUp_Components();
	
public:
	static ToolObject*		Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void Free() override;
};

