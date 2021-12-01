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
	HRESULT				ReadyDX11Device(HWND _hWnd, WINMODE _mode, _uint _width, _uint _height);
	HRESULT				ClearBackBufferView(XMFLOAT4 _color);
	HRESULT				ClearDepthStencilView();
	HRESULT				Present();

private:
	HRESULT				ReadySwapChain(HWND _hWnd, WINMODE _mode, _uint _width, _uint _height);
	HRESULT				ReadyBackBufferView();
	HRESULT				ReadyDepthStencilView(_uint _width, _uint _height);

private:
	ID3D11Device*					dx11Device = nullptr;
	ID3D11DeviceContext*			dx11DeviceContext = nullptr;

	IDXGISwapChain*					dx11SwapChain = nullptr;
	ID3D11RenderTargetView*			dx11RenderTargetView = nullptr;
	ID3D11DepthStencilView*			dx11DepthStencilView = nullptr;

	

public:
	void Free() override;
};

END