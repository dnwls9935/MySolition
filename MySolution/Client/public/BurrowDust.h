#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_PointInstance_Dust;
class CTexture;
class CCollider;
END

BEGIN(Client)

class BurrowDust final : public CGameObject
{
protected:
	explicit BurrowDust(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit BurrowDust(const CGameObject& rhs);
	virtual ~BurrowDust() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*									m_TextureCom = nullptr;
	CRenderer*								m_pRendererCom = nullptr;
	CTransform*								m_pTransformCom = nullptr;
	CVIBuffer_PointInstance_Dust*		m_VIBufferCom = nullptr;
	CCollider*									m_ColliderSphereCom = nullptr;

private:
	virtual HRESULT SetUp_Components();
public:
	static BurrowDust* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END