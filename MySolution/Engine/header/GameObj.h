#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class ENGINE_DLL GameObj : public SuperBase
{
protected:
	explicit GameObj(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit GameObj(const GameObj& rhs);
	virtual  ~GameObj() = default;

public:
	virtual HRESULT		NativeConstructProtoType();
	virtual HRESULT		NativeConstruct(void* _arg);
	virtual _int		Tick(_double _timeDelta);
	virtual _int		LateTick(_double _timeDelta);

protected:
	ID3D11Device*		 dx11Device;
	ID3D11DeviceContext* dx11DeviceContext;


public:
	virtual GameObj*	Clone(void* _arg) = 0;
	virtual void Free();
};

END