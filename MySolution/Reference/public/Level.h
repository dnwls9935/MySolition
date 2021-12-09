#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class ENGINE_DLL Level abstract : public SuperBase
{
protected:
	explicit Level(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual ~Level() = default;

public:
	virtual HRESULT			NativeConstruct();
	virtual	_int			Tick(_double _timeDelta);
	virtual	_int			LateTick(_double _timeDelta);
	virtual	HRESULT			Render();

	virtual _uint			GetLevelID() { return levelID; };

protected:
	ID3D11Device* dx11Device = nullptr;
	ID3D11DeviceContext* dx11DeviceContext = nullptr;
	_uint				levelID = 0;

public:
	virtual void Free() override;
};

END