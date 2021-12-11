#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Texture : public Component
{
private:
	explicit Texture(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit Texture(const Texture& rhs);
	virtual ~Texture() = default;

public:
	virtual HRESULT		NativeConstructProtoType(const _tchar* _filePath, _uint _textureNum);
	virtual HRESULT		NativeConstruct(void* _arg) override;

private:
	vector<ID3D11ShaderResourceView*>			vecTexture;
	typedef vector<ID3D11ShaderResourceView>	TEXTURES;

public:
	static Texture*	Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext, const _tchar* _filePath, _uint _textureNum = 1);
	virtual Component* Clone(void* _arg) override;
	virtual void Free() override;
};

END