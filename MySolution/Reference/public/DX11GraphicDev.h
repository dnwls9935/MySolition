#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class DX11GraphicDev final : public SuperBase
{
	DECLARE_SINGLETON(DX11GraphicDev)
public:
	enum class WINMODE {MODE_FULL, MODE_WIN, MODE_END};
private:
	DX11GraphicDev();
	virtual ~DX11GraphicDev() = default;

public:
	HRESULT				ReadyDX11Device();

private:
	//ID3D11Device*					dx11Device;
	//ID3D11DeviceContext*			dx11DeviceContext;

public:
	void Free() override;
};

END