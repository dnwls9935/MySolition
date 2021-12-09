#pragma once

#include "SuperBase.h"

BEGIN(Engine)

class ENGINE_DLL Component abstract : public SuperBase
{
protected:
	explicit Component(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit Component(const Component& rhs);
	virtual ~Component() = default ;

public:
	virtual HRESULT		NativeConstructProtoType();
	virtual HRESULT		NativeConstruct(void* _arg);
	
protected:
	ID3D11Device* dx11Device;
	ID3D11DeviceContext* dx11DeviceContext;
	_bool		cloned = FALSE;

public:
	virtual Component* Clone(void* _arg) = 0;
	virtual void Free() override;
};

END