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
	virtual HRESULT		NativeConstruct();

public:
	virtual GameObj*	Clone(void* _arg) = 0;
	virtual void Free();
};

END