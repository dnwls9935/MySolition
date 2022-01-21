#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
class Navigation;
END
class ToolTerrain final : public CGameObject
{
public:
	enum class RENDER_ID { DEFAULT, WIREFRAME, RENDER_ID_END };
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

	void*		GetVertices();
	_uint		GetX();
	_uint		GetZ();

	HRESULT SetVertexY(_int _index, _float _y);
	void		SetNormalVector();

private:
	void			PickHeightTerrain(_double _timeDelta);
	void			BatchingObject(_double _timeDelta);
	_fvector		CalcMousePos();
	void			Navigation(_double TimeDelta);

private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	Navigation*					m_NavigationCom = nullptr;

	_float4					m_mousePos;
	_int					m_mouseBrushType;
	_int					m_mouseBrushRadius;

	RENDER_ID				m_RenderID = RENDER_ID::DEFAULT;

	class CMainFrame* m_mainFrm = nullptr;
	class Form* m_form = nullptr;

	list<_float3*>					m_NaviPoints;
	typedef list<_float3*>		NAVIPOINTS;

private:
	virtual HRESULT SetUp_Components();

public:
	static ToolTerrain*		Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void Free() override;
};

