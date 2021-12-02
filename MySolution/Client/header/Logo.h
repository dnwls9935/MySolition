#pragma once

#include "ClientDefine.h"
#include "Level.h"

BEGIN(Client)

class Logo final : public Level
{
public:
	explicit Logo(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext);
	virtual	 ~Logo() = default;

public:
	HRESULT NativeConstruct() override;
	_int Tick(_double _timeDelta) override;
	_int LateTick(_double _timeDelta) override;
	HRESULT Render() override;

public:
	static	Logo* Create(ID3D11Device* _dx11Direct, ID3D11DeviceContext* _dx11DirectContext);
	void Free() override;
};

END