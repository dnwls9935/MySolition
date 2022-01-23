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

class MonsterTest final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		FLY_F, FLY_B, FLY_L,
		IDLE_FLIGHT, IDLE_VAR, 
		RUN_F, SPRINT_F,
		SPAWN_RUN, DEATH_CRITICAL,
		ATTACK_STING, ATTACK_SPIT, ATTACK_RADIUS, ATTACK_FLIGHT_SPIT ,
		ANIMATION_STATE_END};
protected:
	explicit MonsterTest(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit MonsterTest(const CGameObject& rhs);
	virtual ~MonsterTest() = default;
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
	static MonsterTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END