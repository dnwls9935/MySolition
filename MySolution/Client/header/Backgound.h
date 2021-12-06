#pragma once

#include "ClientDefine.h"
#include "GameObj.h"

class Backgound final : public GameObj
{
private:
	explicit Backgound(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit Backgound(const Backgound& rhs);
	virtual ~Backgound() = default;

public:
	HRESULT NativeConstructProtoType() override;
	HRESULT NativeConstruct(void* _arg) override;
	_int Tick(_double _timeDelta) override;
	_int LateTick(_double _timeDelta) override;
	
private:
	HRESULT		AddComponent();

public:
	static	Backgound*	Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual Backgound*	Clone(void* _arg) override;
	virtual void		Free() override;

};

