#pragma once
#include "GameObject.h"


BEGIN(Engine)
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
class CTransform;
END

class UIObject final : public CGameObject
{
private:
	explicit UIObject(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit UIObject(const UIObject& rhs);
	virtual ~UIObject() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

	_bool		PickCheck(POINT _PT);
	void			SetPickCheck(_bool _b) { 
		m_PickCheck = _b; 
	}

	UIOBJDESC							m_UIOBJDesc;

private:
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_matrix							m_ProjMatrix;

	_bool								m_PickCheck;
	_float		m_mouseSenitive = 0.00000001f;

private:
	virtual HRESULT SetUp_Components();

public:
	static UIObject*		Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void Free() override;

};

