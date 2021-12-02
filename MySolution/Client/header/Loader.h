#pragma once

#include "ClientDefine.h"
#include "SuperBase.h"

BEGIN(Client)

class Loader final : public SuperBase
{
private:
	Loader(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext);
	virtual ~Loader() = default;

public:
	HRESULT		NativeConstruct(LVL _nextLVL);
	HRESULT		LoadingLogo();
	HRESULT		LoadingGamePlaye();

	CRITICAL_SECTION		GetCriticalSection() { return criticalSection; }
	_bool					GetThreadFinished() { return threadFinished; }
	LVL						GetNextLVL() { return nextLVL; }

private:
	ID3D11Device*				dx11Device = nullptr;
	ID3D11DeviceContext*		dx11DirectContext = nullptr;

	HANDLE						threadHandle;
	CRITICAL_SECTION			criticalSection;
	_bool						threadFinished;
	_tchar						loadingBuffer[MAX_PATH] = TEXT("");

	LVL							nextLVL = LVL::LVL_END;

public:
	static Loader*		Create(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext, LVL _nextLVL);
	virtual void Free() override;
};

END