#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
public:
	typedef struct tagEffectDesc
	{
		ID3D11InputLayout*			pInputLayout = nullptr;
		ID3DX11EffectPass*			pPass = nullptr;
	}EFFECTDESC;
protected:
	explicit CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(_uint iPassIndex);

public:
	HRESULT SetUp_ValueOnShader(const char* pConstantName, void* pData, _uint iSize);
	HRESULT SetUp_TextureOnShader(const char* pConstantName, class CTexture* pTextureCom, _uint iTextureIndex = 0);
protected: /* For.VertexBuffer */
	ID3D11Buffer*				m_pVB = nullptr;
	D3D11_BUFFER_DESC			m_VBDesc;
	D3D11_SUBRESOURCE_DATA		m_VBSubresourceData;
	_uint						m_iStride = 0; /*정점하나의 바이트 크기. */
	_uint						m_iNumVertices = 0;
	void*						m_pVertices = nullptr;

protected: /* For.IndexBuffer */
	ID3D11Buffer*				m_pIB = nullptr;
	D3D11_BUFFER_DESC			m_IBDesc;
	D3D11_SUBRESOURCE_DATA		m_IBSubresourceData;
	_uint						m_IndicesByteLength = 0; /* 도형하나를 그리기위한인덱스들의 byte크기 */
	_uint						m_iNumPrimitive = 0; /* 도형의 갯수 */
	_uint						m_iNumIndicesFigure = 0; /* 도형하나를 그리기위한 인덱스 갯수. */	
	DXGI_FORMAT					m_eFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology;

private:
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<EFFECTDESC*>			m_EffectDescs;

protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();
	HRESULT Compile_ShaderFiles(const _tchar* pShaderFilePath, D3D11_INPUT_ELEMENT_DESC* pElementDesc, _uint iNumElements);

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END