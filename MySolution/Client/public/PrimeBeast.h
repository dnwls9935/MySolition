#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class Navigation;
class HP;
END

BEGIN(Client)

class PrimeBeast final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		ATT_C_END , ATT_C_HITWALL , ATT_C_LOOP, ATT_C_START, ATT_C_STRIKE,
		ATT_L_END, ATT_L_IDLE, ATT_L_START,
		ATT_P_V1, ATT_P_V2, ATT_P_V3, ATT_P_V4, ATT_P_V5, 
		ATT_TG_V1, ATT_TG_V2, 
		ATT_TR_V1, ATT_TR_V2, ATT_TR_V3, ATT_TR_V4,
		DEA_CORROSIVE_V1, DEA_CRITICAL, DEA_FIRE_V1, DEA_LEFT_V1, DEA_RIGHT_V1, DEA_SHOCK_V1, 
		PHASELOCK_FALL, PHASELOCK_LAND, 
		ROAR_V1, RUN_B, RUN_F_V1, RUN_F_V4, RUN_L, RUN_R, 
		SPAWN_DE, SPAWN_DROPDOWNJUMP, SPAWN_WALLJUMP, SPAWN_WALKOUT, SPAWN_BREAKTHROUGH, SPAWN_CLIMBOVER, DODGE_L,
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
	virtual _bool		Picked() override;

	_bool		ThrowMotionFrame();
	_matrix	SetRockPosition();


private:
	void		GetTargetDistance();
	void		HitCheck();
	void		Animation(_double TimeDelta);
	void		Moving(_double TimeDelta);
	void		Attack();
	void		Dodge(_double TimeDelta);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_ColliderCom = nullptr;
	HP*								m_HpCom = nullptr;

	CHierarchyNode*			m_rHand1Bone = nullptr;
	CHierarchyNode*			m_rHand2Bone = nullptr;
	CHierarchyNode*			m_lHand1Bone = nullptr;
	CHierarchyNode*			m_lHand2Bone = nullptr;

	Navigation*				m_Navigation = nullptr;

	_bool							m_FrameStart = FALSE;


private:
	CGameObject* m_TargetPlayer = nullptr;
	CGameObject* m_TargetPlayerWeapon = nullptr;
	CGameObject*	m_Terrain = nullptr;

	_vector				m_PlayerPosition = _vector();
	_vector				m_MyPosition = _vector();
	_float					m_TargetDistance = 0.f;

private:
	_bool					m_Dodge = FALSE;
	_bool					m_Hit = FALSE;
	_double				m_AccDodgeTime = 0.0;


private:
	virtual HRESULT SetUp_Components();
public:
	static PrimeBeast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END