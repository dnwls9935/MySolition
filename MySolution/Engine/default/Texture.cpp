#include "Texture.h"



Texture::Texture(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: Component(_dx11Device, _dx11DeviceContext)
{
}

Texture::Texture(const Texture & rhs)
	: Component(rhs)
	, vecTexture(rhs.vecTexture)
{
	for (auto& iter : vecTexture)
		Safe_AddRef(iter);
}

HRESULT Texture::NativeConstructProtoType(const _tchar* _filePath, _uint _textureNum)
{
	if (FAILED(__super::NativeConstructProtoType()))
		return E_FAIL;

	if (nullptr == dx11Device)
		return E_FAIL;

	_tchar		pFileFullPath[MAX_PATH] = L"";

	DirectX::ScratchImage pScratchImage;
	HRESULT hr = 0;

	for (_uint i = 0; i < _textureNum; i++)
	{
		_tchar	pExt[MAX_PATH] = TEXT("");
		_wsplitpath_s(_filePath, nullptr, 0, nullptr, 0, nullptr, 0, pExt, MAX_PATH);

		wsprintf(pFileFullPath, _filePath);
		
		if (0 == lstrcmp(pExt, TEXT(".dds")))
			hr = DirectX::LoadFromDDSFile(pFileFullPath, DirectX::CP_FLAGS_NONE, nullptr, pScratchImage);
		else if (0 == lstrcmp(pExt, TEXT(".tga")))
			hr = DirectX::LoadFromTGAFile(pFileFullPath,nullptr, pScratchImage);
		else
			hr = DirectX::LoadFromWICFile(pFileFullPath, DirectX::CP_FLAGS_NONE, nullptr, pScratchImage);

		if (FAILED(hr))
			return E_FAIL;

		ID3D11Resource*		pTextureResource = nullptr;

		if (FAILED(DirectX::CreateTexture(dx11Device, pScratchImage.GetImages(), pScratchImage.GetImageCount(), pScratchImage.GetMetadata(), &pTextureResource)))
			return E_FAIL;

		ID3D11ShaderResourceView*		pShaderResourceView = nullptr;
		if (FAILED(dx11Device->CreateShaderResourceView(pTextureResource, nullptr, &pShaderResourceView)))
			return E_FAIL;

		vecTexture.push_back(pShaderResourceView);

		Safe_Release(pTextureResource);
		pScratchImage.Release();
	}

	return S_OK;
}

HRESULT Texture::NativeConstruct(void * _arg)
{
	if (FAILED(__super::NativeConstruct(_arg)))
		return E_FAIL;

	return E_NOTIMPL;
}

Texture * Texture::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext, const _tchar* _filePath, _uint _textureNum)
{
	Texture* pInstance = new Texture(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstructProtoType(_filePath, _textureNum)))
	{
		MSGBOX("Failed to Create Component Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

Component * Texture::Clone(void * _arg)
{
	Texture* pInstance = new Texture(*this);
	if (FAILED(pInstance->NativeConstruct(_arg)))
	{
		MSGBOX("Failed to Clone Component Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Texture::Free()
{
	__super::Free();
	
	for (auto& iter : vecTexture)
		Safe_Release(iter);
}
