#pragma once

#include "Base.h"

BEGIN(Engine)

class Light final : public CBase
{
private:
	explicit Light(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext);
	virtual ~Light() = default;

public:
	const LIGHTDESC*	GetLightDesc() {
		return &m_LightDesc;
	};

public:
	HRESULT		NativeConstruct(const LIGHTDESC&	_LightDesc);
	HRESULT		Render();

private:
	ID3D11Device*						m_Device = nullptr;
	ID3D11DeviceContext*		m_DeviceContext = nullptr;

private:
	LIGHTDESC											m_LightDesc;
	class CVIBuffer_RectViewPort*		m_VIBuffer = nullptr;

public:
	static Light*	Create(ID3D11Device* _Device, ID3D11DeviceContext* _DeviceContext, const LIGHTDESC& _LightDesc);
	virtual void Free() override;
};

END