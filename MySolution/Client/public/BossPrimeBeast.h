#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class Navigation;
END

BEGIN(Client)

class BossPrimeBeast final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		ATT_C_END, ATT_C_HITWALL, ATT_C_LOOP, ATT_C_START, ATT_C_STRIKE,
		ATT_L_END, ATT_L_IDLE, ATT_L_START,
		ATT_P_V1, ATT_P_V2, ATT_P_V3, ATT_P_V4, ATT_P_V5,
		ATT_TG_V1, ATT_TG_V2,
		ATT_TR_V1, ATT_TR_V2, ATT_TR_V3, ATT_TR_V4,
		DEA_CORROSIVE_V1, DEA_CRITICAL, DEA_FIRE_V1, DEA_LEFT_V1, DEA_RIGHT_V1, DEA_SHOCK_V1,
		PHASELOCK_FALL, PHASELOCK_LAND,
		ROAR_V1, RUN_B, RUN_F_V1, RUN_F_V4, RUN_L, RUN_R,
		SPAWN_DE, SPAWN_DROPDOWNJUMP, SPAWN_WALLJUMP, SPAWN_WALKOUT, SPAWN_BREAKTHROUGH, SPAWN_CLIMBOVER, DODGE_L,
		ANIMATION_STATE_END
	};
protected:
	explicit BossPrimeBeast(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit BossPrimeBeast(const CGameObject& rhs);
	virtual ~BossPrimeBeast() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void		Animation(_double TimeDelta);
	void		GetDistance();
	void		ThrowAttack(_double _TimeDelta);
	void		MeleeAttack(_double _TimeDetla);
	void		Moving(_double _TimeDelta);

	void		AttackBlockCreate(_uint _AnimationState);
	void		ChargeAttack(_double _TimeDelta);
	void		ChargeMove(_double _TimeDelta);
	void		BoneColliderTick(_double _TimeDelta);
	void		AttackCollision(_double _TimeDelta);

public:
	_matrix	GetBoneMatrix(CHierarchyNode*	_HierachyNode);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_ColliderCom = nullptr;
	Navigation*				m_NavigationCom = nullptr;

	CHierarchyNode*			m_rHand1Bone = nullptr;
	CHierarchyNode*			m_rHand2Bone = nullptr;
	CHierarchyNode*			m_lHand1Bone = nullptr;
	CHierarchyNode*			m_lHand2Bone = nullptr;


	CCollider*					m_rHand1Collider = nullptr;
	CCollider*					m_rHand2Collider = nullptr;
	CCollider*					m_lHand1Collider = nullptr;
	CCollider*					m_lHand2Collider = nullptr;


	_bool							m_FrameStart = FALSE;
	_bool							m_IntroEnd = FALSE;

private:
	CGameObject* m_TargetPlayer = nullptr;
	CGameObject* m_TargetPlayerWeapon = nullptr;
	CGameObject* m_Terrain = nullptr;

	_vector				m_PlayerPosition = _vector();
	_vector				m_MyPosition = _vector();
	_float					m_TargetDistance = 0.f;


private:
	_bool					m_ChargeATT = FALSE;
	_double				m_ChargeTime = 0.0;
	_vector				m_TargetPosition = _vector();

private:
	_bool					m_MeleeAttackIsCollisionCheck = FALSE;

private:
	virtual HRESULT SetUp_Components();
public:
	static BossPrimeBeast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END