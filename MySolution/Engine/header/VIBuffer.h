#pragma once

#include "Component.h"

class ENGINE_DLL VIBuffer abstract : public Component
{
public:
	typedef struct tagEffectDesc {
		ID3D11InputLayout*			tagInputLayout = nullptr;
		ID3DX11EffectPass*			tagEffectPass = nullptr;
	}EFFECT_DESC;
protected:
	explicit VIBuffer(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit VIBuffer(const VIBuffer& rhs);
	virtual ~VIBuffer() = default;

public:
	virtual HRESULT		NativeConstructProtoType() override;
	virtual HRESULT		NativeConstruct(void* _arg) override;
	virtual	HRESULT		Render(_uint _passIdx);

public:
	HRESULT		SetUpValueOnShader(const char* _constantName, void* _data, _uint _size);

protected:
	HRESULT			CreateVertextBuffer();
	HRESULT			CreateIndexBuffer();
	HRESULT			ComplieShaderFile(const _tchar* _shaderFilePath, D3D11_INPUT_ELEMENT_DESC* _elementDesc, _uint _count);

protected:
	ID3D11Buffer*				vertextBuffer = nullptr;
	D3D11_BUFFER_DESC			vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA		vertexSubresourceData;
	_uint						stride = 0;
	_uint						numVertices = 0;
	void*						vertices = nullptr;

protected:
	ID3D11Buffer*				indexBuffer = nullptr;
	D3D11_BUFFER_DESC			indexBufferDesc;
	D3D11_SUBRESOURCE_DATA		indexSubresourceData;
	_uint						indexByteLength = 0;
	_uint						numPrimitive = 0;
	_uint						numIndicesFiqure = 0;
	D3D11_PRIMITIVE_TOPOLOGY	primitiveTopology;

private:
	ID3DX11Effect*				effect = nullptr;
	vector<EFFECT_DESC*>		vecEffectDesc;

public:
	virtual Component* Clone(void* _arg) = 0;
	virtual void Free() override;
};