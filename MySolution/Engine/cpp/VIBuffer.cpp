#include "..\header\VIBuffer.h"


VIBuffer::VIBuffer(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: Component(_dx11Device, _dx11DeviceContext)
{
}

VIBuffer::VIBuffer(const VIBuffer & rhs)
	: Component(rhs)
	, vertextBuffer(rhs.vertextBuffer)
	, indexBuffer(rhs.indexBuffer)
	, vertexBufferDesc(rhs.vertexBufferDesc)
	, indexBufferDesc(rhs.indexBufferDesc)
	, vertexSubresourceData(rhs.vertexSubresourceData)
	, indexSubresourceData(rhs.indexSubresourceData)
	, stride(rhs.stride)
	, numVertices(rhs.numVertices)
	, vertices(rhs.vertices)
	, indexByteLength(rhs.indexByteLength)
	, numPrimitive(rhs.numPrimitive)
	, numIndicesFiqure(rhs.numIndicesFiqure)
	, primitiveTopology(rhs.primitiveTopology)
	, effect(rhs.effect)
	, vecEffectDesc(rhs.vecEffectDesc)
{
	Safe_AddRef(vertextBuffer);
	Safe_AddRef(indexBuffer);
	Safe_AddRef(effect);

	for (auto& iter : vecEffectDesc)
	{
		Safe_AddRef(iter->tagEffectPass);
		Safe_AddRef(iter->tagInputLayout);
	}
}

HRESULT VIBuffer::NativeConstructProtoType()
{
	if (FAILED(__super::NativeConstructProtoType()))
		return E_FAIL;

	return S_OK;
}

HRESULT VIBuffer::NativeConstruct(void * _arg)
{
	if (FAILED(__super::NativeConstruct(_arg)))
		return E_FAIL;

	return S_OK;
}

HRESULT VIBuffer::Render(_uint _passIdx)
{
	if (nullptr == dx11DeviceContext)
		return E_FAIL;

	ID3D11Buffer*		vertexBufferArr[]{
		vertextBuffer
	};

	const _uint strides[]{
		stride
	};

	const _uint offset[]{
		0
	};

	dx11DeviceContext->IASetVertexBuffers(0, 1, vertexBufferArr, strides, offset);
	dx11DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dx11DeviceContext->IASetPrimitiveTopology(primitiveTopology);

	dx11DeviceContext->IASetInputLayout(vecEffectDesc[_passIdx]->tagInputLayout);

	if (FAILED(vecEffectDesc[_passIdx]->tagEffectPass->Apply(0, dx11DeviceContext)))
		return E_FAIL;

	dx11DeviceContext->DrawIndexed(numIndicesFiqure* primitiveTopology, 0, 0);

	return S_OK;
}

HRESULT VIBuffer::SetUpValueOnShader(const char* _constantName, void * _data, _uint _size)
{
	ID3DX11EffectVariable*			pVariable = effect->GetVariableByName(_constantName);
	if (nullptr == pVariable)
		return E_FAIL;

	if (FAILED(pVariable->SetRawValue(_data, 0, _size)))
		return E_FAIL;
	return S_OK;
}

HRESULT VIBuffer::CreateVertextBuffer()
{
	if (nullptr == dx11Device)
		return E_FAIL;

	if(FAILED(dx11Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, &vertextBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT VIBuffer::CreateIndexBuffer()
{
	if (nullptr == dx11Device)
		return E_FAIL;

	if (FAILED(dx11Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, &indexBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT VIBuffer::ComplieShaderFile(const _tchar * _shaderFilePath, D3D11_INPUT_ELEMENT_DESC* _elementDesc, _uint _count)
{
	_uint pFlag = 0;
#ifdef _DEBUG
	pFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	pFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

	ID3DBlob*		pShaderCode = nullptr;
	ID3DBlob*		pShaderErrMsg = nullptr;

	if (FAILED(D3DCompileFromFile(_shaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", pFlag, 0, &pShaderCode, &pShaderErrMsg)))
		return E_FAIL;

	if (FAILED(D3DX11CreateEffectFromMemory(pShaderCode->GetBufferPointer(), pShaderCode->GetBufferSize(), 0, dx11Device, &effect)))
		return E_FAIL;

	ID3DX11EffectTechnique*			pTechnique = effect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC			effectDesc;

	if (FAILED(pTechnique->GetDesc(&effectDesc)))
		return E_FAIL;

	vecEffectDesc.reserve(effectDesc.Passes);

	for (_uint i = 0; i < effectDesc.Passes; ++i)
	{
		EFFECT_DESC*		pEffectDesc = new EFFECT_DESC;
		ZeroMemory(pEffectDesc, sizeof(EFFECT_DESC));

		pEffectDesc->tagEffectPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC			PassDesc;

		if (FAILED(pEffectDesc->tagEffectPass->GetDesc(&PassDesc)))
			return E_FAIL;

		if (FAILED(dx11Device->CreateInputLayout(_elementDesc, _count, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pEffectDesc->tagInputLayout)))
			return E_FAIL;

		vecEffectDesc.push_back(pEffectDesc);
	}


	return S_OK;
}

void VIBuffer::Free()
{
	__super::Free(); 

	for (auto& iter : vecEffectDesc)
	{
		Safe_Release(iter->tagEffectPass);
		Safe_Release(iter->tagInputLayout);
	}

	if (FALSE == cloned)
	{
		Safe_Delete_Array(vertices);

		for (auto& iter : vecEffectDesc)
			Safe_Delete(iter);

		vecEffectDesc.clear();
	}
	
	Safe_Release(effect);
	Safe_Release(indexBuffer);
	Safe_Release(vertextBuffer);
}
