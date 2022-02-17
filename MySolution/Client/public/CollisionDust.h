#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_PointInstance_CollisionDust;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CollisionDust final : public CGameObject
{
public:
	typedef struct tagBurrowDustDesc {
		_vector Position = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		class CGameObject* Parent = nullptr;
	}BURROWDUST;

protected:
	explicit CollisionDust(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CollisionDust(const CGameObject& rhs);
	virtual ~CollisionDust() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*												m_TextureCom = nullptr;
	CRenderer*											m_pRendererCom = nullptr;
	CTransform*											m_pTransformCom = nullptr;
	CVIBuffer_PointInstance_CollisionDust*		m_VIBufferCom = nullptr;
	CCollider*												m_ColliderSphereCom = nullptr;

private:
	class BugMorph*						m_Boss = nullptr;

private:
	virtual HRESULT SetUp_Components();
public:
	static CollisionDust* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END