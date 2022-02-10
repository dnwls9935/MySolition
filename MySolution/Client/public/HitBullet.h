#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

class HitBullet final : public CGameObject
{
protected:
	explicit HitBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit HitBullet(const CGameObject& rhs);
	virtual ~HitBullet() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;


private:
	_double						m_AnimationDuration = 0.0;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Rect*		m_VIBufferCom = nullptr;
	CTexture*					m_TextureCom = nullptr;

private:
	virtual HRESULT SetUp_Components();
public:
	static HitBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END