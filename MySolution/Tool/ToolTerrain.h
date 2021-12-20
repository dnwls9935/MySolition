#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END
class ToolTerrain final : public CGameObject
{
private:
	explicit ToolTerrain(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit ToolTerrain(const ToolTerrain& rhs);
	virtual ~ToolTerrain() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

	HRESULT		ResetTerrainInfo(const _tchar* _shaderFile, const _tchar* _heightMapFile, _uint _x, _uint _z);
	
private:
	_float3		PickUpOnTerrain();

private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
private:
	virtual HRESULT SetUp_Components();

public:
	static ToolTerrain*		Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void Free() override;
};
