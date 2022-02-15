#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PointInstance final : public CVIBuffer
{
protected:
	explicit CVIBuffer_PointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_PointInstance(const CVIBuffer_PointInstance& rhs);
	virtual ~CVIBuffer_PointInstance() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, _uint _NumInstance);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT Render(_uint _PassIndex);

public:
	void	  Update(_double _TimeDelta);

private:
	_uint				m_NumInstance = 0;

private:
	ID3D11Buffer*									m_VBInstance = nullptr;
	D3D11_BUFFER_DESC						m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA		m_VBInstSubresourceData;

private:
	_uint					m_InstStride = 0;
	_uint					m_InstNumVertices = 0;
	_double*			m_DropSpeed = nullptr;
	_double*			m_DropX = nullptr;
	_double*			m_DropZ = nullptr;

public:
	static CVIBuffer_PointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, _uint _NumInstance);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END