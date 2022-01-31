#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectViewPort final : public CVIBuffer
{
public:
	typedef struct tagRectViewPortDesc {
		tagRectViewPortDesc() {};
		tagRectViewPortDesc(_float _X, _float _Y, _float _SizeX, _float _SizeY) : X(_X), Y(_Y), SizeX(_SizeX), SizeY(_SizeY) {};

		_float	X = 0.f;
		_float	Y = 0.f;
		_float SizeX = 0.f;
		_float SizeY = 0.f;
	}RVPDESC;
protected:
	explicit CVIBuffer_RectViewPort(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_RectViewPort(const CVIBuffer_RectViewPort& rhs);
	virtual ~CVIBuffer_RectViewPort() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(RVPDESC RVPDesc, const _tchar* pShaderFilePath);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	static CVIBuffer_RectViewPort* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext , RVPDESC RVPDesc, const _tchar* pShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END