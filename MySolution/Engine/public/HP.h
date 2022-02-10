#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL HP final : public CComponent
{ 
public:
	explicit HP(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit HP(const HP& rhs);
	virtual ~HP() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);

public:
	_int			Update(_vector Position, _float _Barpercent);
	HRESULT		Render();
	void				Picked(_bool _b) {
		m_Picked = _b;
	}

private:
	class CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	class CTexture*					m_pTextureCom = nullptr;
	class CTransform*			m_pTransformCom = nullptr;
private:
	_float		m_Barpercent = 1.f;
	_bool		m_Picked = FALSE;

private:
	virtual HRESULT SetUp_Components();

public:
	static HP*	Create(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext);
	virtual  HP* Clone(void* pArg);
	virtual void Free() override;
};

END