#include "..\header\RectBuffer.h"

RectBuffer::RectBuffer(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: VIBuffer(_dx11Device, _dx11DeviceContext)
{
}

RectBuffer::RectBuffer(const RectBuffer& rhs)
	: VIBuffer(rhs)
{
}

HRESULT RectBuffer::NativeConstructProtoType(const _tchar* _shaderFilePath)
{
	if (FAILED(__super::NativeConstructProtoType()))
		return E_FAIL;

	/*
	UINT ByteWidth;
	D3D11_USAGE Usage;
	UINT BindFlags;
	UINT CPUAccessFlags;
	UINT MiscFlags;
	UINT StructureByteStride;
	*/

	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	stride = sizeof(VTXTEX);
	numVertices = 4;
	
	vertexBufferDesc.ByteWidth = stride * numVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = stride;
	
	ZeroMemory(&vertexSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertices = new VTXTEX[numVertices];
	ZeroMemory(vertices, sizeof(VTXTEX) * numVertices);

	((VTXTEX*)vertices)[0].position = _vec3(-0.5f, 0.5f, 0.f);
	((VTXTEX*)vertices)[0].texUV = _vec2(0.0f, 0.0f);

	((VTXTEX*)vertices)[1].position = _vec3(0.5f, 0.5f, 0.f);
	((VTXTEX*)vertices)[1].texUV = _vec2(1.0f, 0.0f);

	((VTXTEX*)vertices)[2].position = _vec3(0.5f, -0.5f, 0.f);
	((VTXTEX*)vertices)[2].texUV = _vec2(1.0f, 1.0f);

	((VTXTEX*)vertices)[3].position = _vec3(-0.5f, -0.5f, 0.f);
	((VTXTEX*)vertices)[3].texUV = _vec2(0.0f, 1.0f);

	vertexSubresourceData.pSysMem = vertices;

	if (FAILED(__super::CreateVertextBuffer()))
		return E_FAIL;


	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexByteLength = sizeof(FACEIDX16);
	numPrimitive = 2;
	numIndicesFiqure = 3;
	primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	indexBufferDesc.ByteWidth = indexByteLength * numPrimitive;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	FACEIDX16*		indices = new FACEIDX16[numPrimitive];
	ZeroMemory(indices, sizeof(FACEIDX16) * numPrimitive);

	indices[0]._0 = 0;
	indices[0]._1 = 1;
	indices[0]._2 = 2;

	indices[1]._0 = 0;
	indices[1]._1 = 2;
	indices[1]._2 = 3;

	indexSubresourceData.pSysMem = indices;

	if (FAILED(__super::CreateIndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(indices);
	

	D3D11_INPUT_ELEMENT_DESC		pElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(ComplieShaderFile(_shaderFilePath, pElementDesc, 2)))
		return E_FAIL;

	return S_OK;
}

HRESULT RectBuffer::NativeConstruct(void * _arg)
{
	if (FAILED(__super::NativeConstruct(_arg)))
		return E_FAIL;

	return S_OK;
}

RectBuffer * RectBuffer::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext, const _tchar* _shaderFilePath)
{
	RectBuffer* pInstance = new RectBuffer(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstructProtoType(_shaderFilePath)))
	{ 
		MSGBOX("Failed to Create Component VIBuffer RectBuffer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

Component * RectBuffer::Clone(void * _arg)
{
	RectBuffer* pInstance = new RectBuffer(*this);
	if (FAILED(pInstance->NativeConstruct(_arg)))
	{
		MSGBOX("Failed to Clone Component VIBuffer RectBuffer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void RectBuffer::Free()
{
	__super::Free();
}
