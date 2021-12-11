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
	HRESULT		ReserveManager(_uint	_numLVL);
	HRESULT		AddProtoType(_uint _numLVL, const _tchar* _protoTag, class Component* _protoComponent);
	class Component*	CloneComponent(_uint _numLVL, const _tchar* _tag, void* _arg);

private:
	class Component*	FindComponent(_uint _numLVL, const _tchar* _tag);

private:
	unordered_map<const _tchar*, class Component*>* umapProtoContainer = nullptr;
	typedef unordered_map<const _tchar*, class Component*> COMPONENTS;

	_uint		numLVL;

public:
	virtual void Free() override;
};

END