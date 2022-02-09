#pragma once

#include "Component.h"

/* 객체들을 그리는 순서가 중요하기 때문. */

BEGIN(Engine)

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDER { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };
	enum RENDERCOM {RENDERCOM_HP, RENDERCOM_COLLIDER, RENDERCOM_END};
private:
	explicit CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
public:
	HRESULT Add_RenderGroup(RENDER eRenderID, class CGameObject* pGameObject);
	HRESULT Add_RenderComGroup(RENDERCOM eRenderID, class CComponent* pComponent);
	HRESULT Draw_RenderGroup();
private:
	list<class CGameObject*>			m_RenderGroup[RENDER_END];
	typedef list<class CGameObject*>	RENDERGROUP;

	list<class CComponent*>			m_RenderComGroup[RENDERCOM::RENDERCOM_END];
	typedef list<class CComponent*>		RENDERCOMGROUP;

private:
	class CTarget_Manager*					m_pTarget_Manager = nullptr;
	class CVIBuffer_RectViewPort*			m_pVIBuffer = nullptr;

private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlpha();
	HRESULT Render_Alpha();
	HRESULT Render_UI();
private:
	HRESULT Render_LightAcc();
	HRESULT Render_Blend();
private:
	HRESULT Render_Components();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END