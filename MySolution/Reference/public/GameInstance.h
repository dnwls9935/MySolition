#pragma once

#include "DX11GraphicDev.h"

BEGIN(Engine)

class ENGINE_DLL GameInstance final : public SuperBase
{
	DECLARE_SINGLETON(GameInstance)
private:
	GameInstance();
	virtual ~GameInstance() = default;

public:
	HRESULT				InitializeEngine(HWND _hWnd, DX11GraphicDev::WINMODE _mode, _uint _width, _uint _height);
	_int				TickEngine(_double _timeDelta);
	_int				LateTick(_double _timeDelta);

	/* 장치초기화(DX11) */
	HRESULT				ClearBackBufferView(XMFLOAT4 _color);
	HRESULT				ClearDepthStencilView();
	HRESULT				Present();



private:
	DX11GraphicDev*		dx11GraphicDev = nullptr;

public:
	static void ReleaseEngine();
	void Free() override;
};

END