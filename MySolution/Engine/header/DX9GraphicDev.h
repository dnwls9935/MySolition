#pragma once

#include "EngineInclude.h"
#include "SuperBase.h"

BEGIN(Engine)

class DLL_MODE DX9GraphicDev : public SuperBase
{
	DECLARE_SINGLETON(DX9GraphicDev)
private:
	DX9GraphicDev();
	virtual ~DX9GraphicDev();

public:
	HRESULT		ReadyDirectX9Device(HWND _hWnd, WINMODE _mode, const _ulong& _sizeX, const _ulong& _sizeY, LPDIRECT3DDEVICE9* _directX9Device);
	void		DirectX9RenderBegin(D3DXCOLOR _color);
	void		DirectX9RenderEnd();

private:
	LPDIRECT3DDEVICE9			directX9Device;


	LPDIRECT3D9			m_pSDK;
	LPDIRECT3DDEVICE9	m_pGraphicDev;

public:
	void Free() override;
};

END