#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class Layer : public SuperBase
{
protected:
	Layer();
	virtual ~Layer() = default;

public:
	HRESULT		AddObj(class GameObj* _obj);
	_int		Tick(_double _timeDelta);
	_int		LateTick(_double _timeDelta);

protected:
	list<class GameObj*> listObj;

public:
	static Layer* Create();
	virtual void Free() override;
};

END