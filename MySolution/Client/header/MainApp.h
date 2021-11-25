#pragma once

#include "ClientDefines.h"
#include "SuperBase.h"

BEGIN(Client)
class MainApp final : public SuperBase
{
private:
	MainApp();
	virtual ~MainApp();

public:
	HRESULT NativeConstruct();
	_int Tick(const _double _deltaTime);
	_int Process(const _double _deltaTime);
	HRESULT Render();

public:
	static MainApp* Create();
	void Free() override;
};

END