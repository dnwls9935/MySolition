#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Rendering final : public Component
{
public:
	enum RENDERING_ID{ RENDER_PRIORITY, RENDER_NOALPHA, RENDER_ALPHA, RENDER_END };
private:
	explicit Rendering(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual ~Rendering() = default;

public:
	virtual HRESULT		NativeConstructProtoType();
	virtual HRESULT		NativeConstruct(void* _arg);

	HRESULT		AddRenderingGroup(RENDERING_ID _id, class GameObj* _obj);
	HRESULT		RenderingGroup();

private:
	HRESULT	RenderingPrioirty();
	HRESULT	RenderingNoalpha();
	HRESULT	RenderingAlpha();

private:
	list<class GameObj*>				renderGroup[RENDER_END];
	typedef list<class GameObj*>		RENDERGROUP;

public:
	static Component*	Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual Component* Clone(void* _arg) override;
	virtual void Free() override;
};

END