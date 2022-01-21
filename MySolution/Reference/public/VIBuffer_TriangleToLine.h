#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_TriangleToLine final : public CVIBuffer
{
protected:
	explicit CVIBuffer_TriangleToLine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_TriangleToLine(const CVIBuffer_TriangleToLine& rhs);
	virtual ~CVIBuffer_TriangleToLine() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, _float3* _Points = nullptr);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	static CVIBuffer_TriangleToLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, _float3* _Points = nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END