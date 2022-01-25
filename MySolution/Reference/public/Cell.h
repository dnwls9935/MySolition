#pragma once

#include "Base.h"

BEGIN(Engine)
class Cell final : public CBase
{
public:
	enum class POINT {POINT_1st, POINT_2nd, POINT_3rd, POINT_END};
	enum class LINE {LINE_12, LINE_23, LINE_31, LINE_END};

private:
	explicit Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~Cell() = default;

public:
	virtual HRESULT NativeConstruct(_float3* _Point, _uint _Index, const _tchar* _ShaderFilePath);

public:
	_fvector		GetPoint(POINT _Point) { 
		return XMLoadFloat3(&m_Point[(_uint)_Point]); 
	}
	_uint				GetIndex() const {
		return m_Index;
	}

	void				SetNeighbor(LINE _Line, Cell* _Neighbor) {
		m_Neighbor[(_uint)_Line] = _Neighbor;
		Safe_AddRef(_Neighbor);
	}

	_bool			Compare(_fvector _SourPoint, _fvector _DestPoint);
	_bool			IsIn(_fvector _Position, Cell** _OutNeighbor);

#ifdef _DEBUG
public:
	HRESULT		ReadyDebugBuffer(const _tchar* _ShaderFilePath);
	HRESULT		Render(_fmatrix _WorldMatrix, _uint _CurrentIndex);
#endif // _DEBUG


private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	_float3							m_Point[(_uint)POINT::POINT_END];
	_float3							m_Line[(_uint)LINE::LINE_END];
	_float3							m_Normal[(_uint)LINE::LINE_END];
	Cell*								m_Neighbor[(_uint)LINE::LINE_END];
	_uint								m_Index = 0;


#ifdef _DEBUG
private:
	class CVIBuffer_TriangleToLine*						m_VIBuffer = nullptr;
#endif // _DEBUG


public:
	static Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float3* _Point, _uint _Index, const _tchar* _ShaderFilePath = TEXT("../Bin/ShaderFiles/Shader_TriangleToLine.hlsl"));
	virtual void Free() override;
};

END