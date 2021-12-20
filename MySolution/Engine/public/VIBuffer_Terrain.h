#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	explicit CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath,_uint _verticesX, _uint _verticesZ, const _tchar* pHeightMapPath);
	virtual HRESULT NativeConstruct(void* pArg) override;

	_uint	GetVerticesX() { return m_iNumVerticesX; };
	_uint	GetVerticesZ() { return m_iNumVerticesZ; };

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,const _tchar* pShaderFilePath, _uint _verticesX = 2, _uint _verticesZ = 2, const _tchar* pHeightMapPath = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END