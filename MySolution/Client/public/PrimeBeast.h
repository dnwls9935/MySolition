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

class PrimeBeast final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		ATTACK_PUNCH_V1, ATTACK_PUNCH_V2, ATTACK_PUNCH_V3, ATTACK_PUNCH_V4, ATTACK_PUNCH_V5,
		ATTACK_THROWGROUND_V1, ATTACK_THROWGROUND_V2, 
		ATTACK_THROWROCK_V1, ATTACK_THROWROCK_V2, ATTACK_THROWROCK_V3, ATTACK_THROWROCK_V4,
		DEATH_CORROSIVE_V1, DEATH_CRITICAL, DEATH_FIRE_V1, 
		PHASELOCK_FAIL, PHASELOCK_LAND, ROAD_V1,
		RUN_B, RUN_F_V1, RUN_V4, RUN_L, RUN_R,
		SPAWN_DENLEAP_V1, SPAWN_DROP, SPAWN_WALLDROPJUMP,
		ANIMATION_STATE_END};
protected:
	explicit PrimeBeast(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit PrimeBeast(const CGameObject& rhs);
	virtual ~PrimeBeast() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	_bool		ThrowMotionFrame();
	_matrix	SetRockPosition();

private:
	void		GetTargetDistance();
	void		HitCheck();
	void		Animation(_double TimeDelta);
	void		Moving(_double TimeDelta);
	void		Attack();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_ColliderCom = nullptr;

	CHierarchyNode*			m_rHand1Bone = nullptr;
	CHierarchyNode*			m_rHand2Bone = nullptr;
	CHierarchyNode*			m_lHand1Bone = nullptr;
	CHierarchyNode*			m_lHand2Bone = nullptr;


	CCollider*					m_ColliderSphere1 = nullptr;
	CCollider*					m_ColliderSphere2 = nullptr;
	CCollider*					m_ColliderSphere3 = nullptr;
	CCollider*					m_ColliderSphere4 = nullptr;


	_bool							m_FrameStart = FALSE;
	_bool							m_IntroEnd = FALSE;

private:
	CGameObject* m_TargetPlayer = nullptr;
	CGameObject* m_TargetPlayerWeapon = nullptr;

	_vector				m_PlayerPosition = _vector();
	_vector				m_MyPosition = _vector();
	_float					m_TargetDistance = 0.f;


private:
	virtual HRESULT SetUp_Components();
public:
	static PrimeBeast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END