#include "..\header\Component.h"

Component::Component(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: dx11Device(_dx11Device)
	, dx11DeviceContext(_dx11DeviceContext)
	, cloned(FALSE)
{
	Safe_AddRef(dx11Device);
	Safe_AddRef(dx11DeviceContext);
}

Component::Component(const Component & rhs)
	: dx11Device(rhs.dx11Device)
	, dx11DeviceContext(rhs.dx11DeviceContext)
	, cloned(TRUE)
{
	Safe_AddRef(dx11Device);
	Safe_AddRef(dx11DeviceContext);
}

HRESULT Component::NativeConstructProtoType()
{
	return S_OK;
}

HRESULT Component::NativeConstruct(void * _arg)
{
	return S_OK;
}

void Component::Free()
{
	Safe_Release(dx11Device);
	Safe_Release(dx11DeviceContext);
}
