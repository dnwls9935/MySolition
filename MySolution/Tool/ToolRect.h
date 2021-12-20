#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
END
class ToolRect final : public CGameObject
{
private:
	explicit ToolRect(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	explicit ToolRect(const ToolRect& rhs);
	virtual ~ToolRect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
private:
	virtual HRESULT SetUp_Components();

public:
	static ToolRect*		Create(ID3D11Device* _dx11Device, ID3D11DeviceContext* _dx11DeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void Free() override;
};