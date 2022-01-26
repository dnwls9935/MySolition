#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Terrain;
class Navigation;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
protected:
	explicit CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CTerrain(const CGameObject& rhs);
	virtual ~CTerrain() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void		SetVerticeY(_uint Index, _float Y);

private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;
	Navigation*					m_NavigationCom = nullptr;
private:
	virtual HRESULT SetUp_Components();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END