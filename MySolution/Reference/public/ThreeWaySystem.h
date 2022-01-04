#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_ThreeWaySystem final : public CVIBuffer
{
protected:
	explicit CVIBuffer_ThreeWaySystem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_ThreeWaySystem(const CVIBuffer_ThreeWaySystem& rhs);
	virtual ~CVIBuffer_ThreeWaySystem() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	static CVIBuffer_ThreeWaySystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END