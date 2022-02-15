#include "..\public\VIBuffer_PointInstance_Dust.h"

CVIBuffer_PointInstance_Dust::CVIBuffer_PointInstance_Dust(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_PointInstance_Dust::CVIBuffer_PointInstance_Dust(const CVIBuffer_PointInstance_Dust & rhs)
	: CVIBuffer(rhs)
	, m_NumInstance(rhs.m_NumInstance)
	, m_VBInstance(rhs.m_VBInstance)
	, m_VBInstDesc(rhs.m_VBInstDesc)
	, m_VBInstSubresourceData(rhs.m_VBInstSubresourceData)
	, m_InstStride(rhs.m_InstStride)
	, m_InstNumVertices(rhs.m_InstNumVertices)
	, m_Look(rhs.m_Look)
{
	Safe_AddRef(m_VBInstance);
}

HRESULT CVIBuffer_PointInstance_Dust::NativeConstruct_Prototype(const _tchar* pShaderFilePath, _uint _NumInstance)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices  = _NumInstance;
	m_NumInstance = _NumInstance;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;


	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		((VTXPOINT*)m_pVertices)[i].vPosition = _float3(0.0f, 0.0f, 0.0f);
		((VTXPOINT*)m_pVertices)[i].vPSize = _float2(10.f, 10.f);
	}

	m_VBSubresourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_VBInstance, sizeof(D3D11_BUFFER_DESC));

	m_InstStride = sizeof(VTXMATRIX);
	m_InstNumVertices = m_NumInstance;

	m_VBInstDesc.ByteWidth = m_InstStride * m_InstNumVertices;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstDesc.MiscFlags = 0;
	m_VBInstDesc.StructureByteStride = m_InstStride;

	ZeroMemory(&m_VBInstSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	VTXMATRIX*		pVertices = new VTXMATRIX[m_InstNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMATRIX) * m_InstNumVertices);

	for (_uint i = 0; i < m_InstNumVertices; i++) {
		pVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	}
	m_VBInstSubresourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstDesc, &m_VBInstSubresourceData, &m_VBInstance)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	D3D11_INPUT_ELEMENT_DESC		ElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PSIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	if (FAILED(Compile_ShaderFiles(pShaderFilePath, ElementDescs, 6)))
		return E_FAIL;

	m_Look = new _vector[m_NumInstance];
	for (_uint i = 0; i < m_NumInstance; i++) {
		_double random = (_double)(rand() * (_double)1 / ((_double)RAND_MAX) + (_double)(1)) - 1;
		m_Look[i] = XMVectorSetX(m_Look[i], random);
		random = (_double)(rand() * (_double)1 / ((_double)RAND_MAX) + (_double)(1)) - 1;
		m_Look[i] = XMVectorSetY(m_Look[i], 0);
		random = (_double)(rand() * (_double)1 / ((_double)RAND_MAX) + (_double)(1)) - 1;
		m_Look[i] = XMVectorSetZ(m_Look[i], random);
	}

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Dust::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_PointInstance_Dust::Render(_uint _PassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	ID3D11Buffer*				pVertexBuffer[] = {
		m_pVB,
		m_VBInstance
	};

	_uint		Strides[] = {
		m_iStride,
		m_InstStride
	};

	_uint		Offset[] = {
		0,
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, 2, pVertexBuffer, Strides, Offset);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);
	m_pDeviceContext->IASetInputLayout(m_EffectDescs[_PassIndex]->pInputLayout);

	if (FAILED(m_EffectDescs[_PassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_pDeviceContext->DrawInstanced(1, m_NumInstance, 0, 0);

	return S_OK;
}

_bool CVIBuffer_PointInstance_Dust::Update(_double _TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	m_pDeviceContext->Map(m_VBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	_bool HitBottom = TRUE;

	for (_uint i = 0; i < m_NumInstance; i++)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition.x += XMVectorGetX(m_Look[i]);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y += XMVectorGetY(m_Look[i]);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z += XMVectorGetZ(m_Look[i]);
	}

	m_pDeviceContext->Unmap(m_VBInstance, 0);

	return HitBottom;
}

HRESULT CVIBuffer_PointInstance_Dust::SettingPosition(_fvector _Position)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	m_pDeviceContext->Map(m_VBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (_uint i = 0; i < m_NumInstance; i++)
	{
		((VTXMATRIX*)SubResource.pData)[i].vPosition.x = XMVectorGetX(_Position);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.y = XMVectorGetY(_Position);
		((VTXMATRIX*)SubResource.pData)[i].vPosition.z = XMVectorGetZ(_Position);
	}

	m_pDeviceContext->Unmap(m_VBInstance, 0);

	return S_OK;
}

CVIBuffer_PointInstance_Dust * CVIBuffer_PointInstance_Dust::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, _uint _NumInstance)
{
	CVIBuffer_PointInstance_Dust*		pInstance = new CVIBuffer_PointInstance_Dust(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, _NumInstance)))
	{
		MSGBOX("Failed to Creating CVIBuffer_PointInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_PointInstance_Dust::Clone(void * pArg)
{
	CVIBuffer_PointInstance_Dust*		pInstance = new CVIBuffer_PointInstance_Dust(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning CVIBuffer_PointInstance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_PointInstance_Dust::Free()
{
	__super::Free();

	if (FALSE == m_isCloned)
	{
		Safe_Delete_Array(m_Look);
		Safe_Delete_Array(m_Look);
	}

	Safe_Release(m_VBInstance);
}
