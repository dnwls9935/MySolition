#include "..\public\VIBuffer.h"
#include "Texture.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_VBDesc(rhs.m_VBDesc)
	, m_VBSubresourceData(rhs.m_VBSubresourceData)
	, m_iStride(rhs.m_iStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_pVertices(rhs.m_pVertices)
	, m_pIB(rhs.m_pIB)
	, m_IBDesc(rhs.m_IBDesc)
	, m_IBSubresourceData(rhs.m_IBSubresourceData)
	, m_IndicesByteLength(rhs.m_IndicesByteLength)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iNumIndicesFigure(rhs.m_iNumIndicesFigure)
	, m_ePrimitiveTopology(rhs.m_ePrimitiveTopology)
	, m_pEffect(rhs.m_pEffect)
	, m_EffectDescs(rhs.m_EffectDescs)
	, m_eFormat(rhs.m_eFormat)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
	Safe_AddRef(m_pEffect);

	for (auto& pEffectDesc : m_EffectDescs)
	{
		Safe_AddRef(pEffectDesc->pInputLayout);
		Safe_AddRef(pEffectDesc->pPass);
	}
}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB
	};

	_uint				iStrides[] = {
		m_iStride
	};

	_uint				iOffsets[] = {
		0
	};

	/* 그려야할 버텍싀버퍼들을 장치에 바인드한다. */
	m_pDeviceContext->IASetVertexBuffers(0, 1, pVertexBuffers, iStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	m_pDeviceContext->IASetInputLayout(m_EffectDescs[iPassIndex]->pInputLayout);


	if (FAILED(m_EffectDescs[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_pDeviceContext->DrawIndexed(m_iNumIndicesFigure * m_iNumPrimitive, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::SetUp_ValueOnShader(const char* pConstantName, void* pData, _uint iSize)
{
	ID3DX11EffectVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	if (FAILED(pVariable->SetRawValue(pData, 0, iSize)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::SetUp_TextureOnShader(const char * pConstantName, CTexture * pTextureCom, _uint iTextureIndex)
{
	ID3D11ShaderResourceView*	pShaderResourceView = pTextureCom->Get_ShaderResourceView(iTextureIndex);
	if (nullptr == pShaderResourceView)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*		pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetResource(pShaderResourceView);
}


HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	/* 메모리 버퍼를 생성하낟. */
	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubresourceData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	/* 메모리 버퍼를 생성하낟. */
	if (FAILED(m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubresourceData, &m_pIB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Compile_ShaderFiles(const _tchar * pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElementDesc, _uint iNumElements)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob*			pShaderByteCodes = nullptr;
	ID3DBlob*			pCompileErrorMessage = nullptr;

	if (FAILED(D3DCompileFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, nullptr, "fx_5_0", iFlag, 0, &pShaderByteCodes, &pCompileErrorMessage)))
		return E_FAIL;

	if (FAILED(D3DX11CreateEffectFromMemory(pShaderByteCodes->GetBufferPointer(), pShaderByteCodes->GetBufferSize(), 0, m_pDevice, &m_pEffect)))
		return E_FAIL;

	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC			EffectDesc;

	if (FAILED(pTechnique->GetDesc(&EffectDesc)))
		return E_FAIL;

	m_EffectDescs.reserve(EffectDesc.Passes);

	for (_uint i = 0; i < EffectDesc.Passes; ++i)
	{
		EFFECTDESC*		pEffectDesc = new EFFECTDESC;
		ZeroMemory(pEffectDesc, sizeof(EFFECTDESC));

		pEffectDesc->pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC			PassDesc;

		pEffectDesc->pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(pElementDesc, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pEffectDesc->pInputLayout)))
			return E_FAIL;

		m_EffectDescs.push_back(pEffectDesc);
	}

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	for (auto& pEffectDesc : m_EffectDescs)
	{
		/*Safe_Release(pEffectDesc->pPass);*/
		Safe_Release(pEffectDesc->pInputLayout);
	}

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pVertices);

		for (auto& pEffectDesc : m_EffectDescs)
			Safe_Delete(pEffectDesc);
		m_EffectDescs.clear();
	}

	Safe_Release(m_pEffect);
	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
