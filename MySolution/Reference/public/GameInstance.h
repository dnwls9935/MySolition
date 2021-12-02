#pragma once

#include "DX11GraphicDev.h"
#include "LevelManager.h"
#include "TimeManager.h"

BEGIN(Engine)

class ENGINE_DLL GameInstance final : public SuperBase
{
	DECLARE_SINGLETON(GameInstance)
private:
	GameInstance();
	virtual ~GameInstance() = default;

public:
	HRESULT				InitializeEngine(HWND _hWnd, DX11GraphicDev::WINMODE _mode, _uint _width, _uint _height, ID3D11Device** _dx11DeviceOut, ID3D11DeviceContext** _dx11DeviceContextOut);
	_int				TickEngine(_double _timeDelta);
	_int				LateTick(_double _timeDelta);
	/* 디버그용 */
	HRESULT				Render();

	/* 장치초기화(DX11) */
	HRESULT				ClearBackBufferView(XMFLOAT4 _color);
	HRESULT				ClearDepthStencilView();
	HRESULT				Present();

	/* LVLManager */
	HRESULT		OpenLVL(class Level* pLVL);

	/* TimeManager */
	_double					GetTimeDelta(const _tchar* _timerTag);
	void					UpdateTimeDelta(const _tchar* _timerTag);
	HRESULT					ReadyTimer(const _tchar* _timerTag);



private:
	DX11GraphicDev*		dx11GraphicDev = nullptr;
	LevelManager*		levelManager = nullptr;
	TimeManager*		timeManager = nullptr;

public:
	static void ReleaseEngine();
	void Free() override;
};

END