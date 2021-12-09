#pragma once

#include "ClientDefine.h"
#include "SuperBase.h"

BEGIN(Engine)
class GameInstance;
class Rendering;
END

class MainApp final : public SuperBase
{
private:
	MainApp();
	virtual ~MainApp() = default;

public:
	HRESULT				NativeConstruct();
	_int				Tick(_double _timeDelta);
	HRESULT				Render();

private:
	HRESULT				SetUpLVL(LVL _nextLevel);
	HRESULT				ReadyComponentProtoType();
	HRESULT				ReadyGameObjectProtoType();

private:
	GameInstance*			gameInstance = nullptr;
	ID3D11Device*			dx11Device = nullptr;
	ID3D11DeviceContext*	dx11DeviceContext = nullptr;
	Rendering*				rendering = nullptr;


public:
	static MainApp* Create();
	void Free() override;
};

