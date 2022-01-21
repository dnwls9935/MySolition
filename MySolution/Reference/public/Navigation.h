#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Navigation final : public CComponent
{
private:
	explicit Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Navigation(const Navigation& rhs);
	virtual ~Navigation() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* _NavigationFilePath);
	virtual HRESULT NativeConstruct(void* pArg);

	_bool	MoveOnNavigation(_fvector _Position);

public:
	void		AddCell(_float3* _Points);

#ifdef _DEBUG
	HRESULT	Render(_fmatrix _WorldMatrix = XMMatrixIdentity());
#endif // _DEBUG

private:
	HRESULT	LoadNavigationCells(const _tchar* _NavigationFilePath);
	HRESULT	SetUpNeighbor();
	
private:
	vector<class Cell*>					m_Cells;
	typedef vector<class Cell*>		CELLS;
	_uint											m_CurrentCellIndex = 0;

public:
	static Navigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* _NavigationFilePath= nullptr);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END