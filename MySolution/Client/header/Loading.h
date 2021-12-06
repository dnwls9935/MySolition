#pragma once

#include "ClientDefine.h"
#include "Level.h"
#include "Loader.h"

BEGIN(Client)

class Loading final : public Level
{
public:
	explicit Loading(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext);
	virtual ~Loading() = default;

public:
	HRESULT NativeConstruct(LVL _nextLVL);
	_int Tick(_double _timeDelta) override;
	_int LateTick(_double _timeDelta) override;
	HRESULT Render() override;

private:
	HRESULT		OpenLevel();
	HRESULT		ReadyBackGround(const _tchar* _layerTag);
	
private:
	LVL			nextLVL = LVL::LVL_END;
	Loader*		loader = nullptr;

public:
	static	Loading* Create(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext, LVL _nextLVL);
	virtual void Free() override;

};

END