#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL RectBuffer final : public VIBuffer
{
private:
	explicit RectBuffer(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit RectBuffer(const RectBuffer& rhs);
	virtual ~RectBuffer() = default;

public:
	virtual HRESULT		NativeConstructProtoType(const _tchar* _shaderFilePath);
	virtual HRESULT		NativeConstruct(void* _arg) override;

public:
	static RectBuffer*	Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext, const _tchar* _shaderFilePath);
	virtual Component*	Clone(void* _arg) override; 
	virtual void		Free() override;
};

END