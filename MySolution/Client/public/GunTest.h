#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)

class GunTest final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		FIRE_DAHL, FIRE_JAKOBS, FIRE_MALIWAN, FIRE_TEDIORE, FIRE_TORGUE,
		IDLE_DAHL, IDLE_HYPERION, IDLE_JAKOBS, IDLE_MALIWAN, IDLE_TORGUE,
		RELOAD_JAKOBS, RELOAD_JAKOBS_VAR, RELOAD_HYPERION, FIRE_FAN,
		ANIMATION_STATE_END};
protected:
	explicit GunTest(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit GunTest(const CGameObject& rhs);
	virtual ~GunTest() = default;
public:
	/* ������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* ��������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_ColliderCom = nullptr;

	class CGameObject* m_TargetObject = nullptr;

private:
	virtual HRESULT SetUp_Components();
public:
	static GunTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END