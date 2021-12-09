#pragma once

#include "Rendering.h"
#include "RectBuffer.h"

BEGIN(Engine)

class ComponentManager final : public SuperBase
{
	DECLARE_SINGLETON(ComponentManager)
private:
	ComponentManager();
	virtual ~ComponentManager() = default;

public:
	HRESULT		AddProtoType(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	class Component*	CloneComponent(const _tchar* _tag, void* _arg);

private:
	class Component*	FindComponent(const _tchar* _tag);

private:
	unordered_map<const _tchar*, class Component*> umapProtoContainer;
	typedef unordered_map<const _tchar*, class Component*> COMPONENTS;

public:
	virtual void Free() override;
};

END