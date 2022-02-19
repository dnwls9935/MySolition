#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "PickUps.h"
BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class Navigation;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum class ANIMATION_STATE { ALL,
		DRAW, HOLSTER, IDLE, RUN_F, RUN_L, RUN_R, 
		JUMP_END, JUMP_IDLE, JUMP_START,
		SPRINT, CROUCH, GRENADE_THROW, 
		RE_TEDIORE_F,
		SI_JUMP_END, SI_JUMP_IDLE, SI_JUMP_START, SI_WALK_F, SI_IDLE,
		SPRINT_DUALWIELD,
		RE_MALIWAN_V2, RE_MALIWAN_F, RE_BANDITMADE_F, RE_HYPERION_F,	RE_BANDITMADE, RE_HYPERION, RE_MALIWAN, RE_MALIWAN_F_V2,
		RE_TEDIORE, RE_DAHL_F, RE_DAHL, BLADED_MELEE, RE_DUALWIELD, HOLSTER_F, INCAP_F, ADD_FIRE_RECOIL, PICKUP_EQUIP,
		ANIMATION_STATE_END};
	enum class MOVE_TYPE { FRONT, BACK, RIGHT, LEFT, MOVE_TYPE_END };
	enum class ROTATION_TYPE { X, Y, ROTATION_TYPE_END };

private:
	typedef struct tagRay {
		_vector Ray;
		_vector Dir;
	}RAY;

protected:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CGameObject& rhs);
	virtual ~CPlayer() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	_fmatrix GetCameraMatrix();
	
	MOVE_TYPE	GetMoveType() const { return m_Move; }
	ROTATION_TYPE	GetRotationType() const { return m_Rotation; }

	void SetUpWeapon();

	_bool		GetShot()const { 
		return m_Shot;
	};

	void		Hit(_int	_HP);
	_bool	GetInteractPick() {
		return m_InteractPick;
	}
	_bool	GetIsIn() {
		return m_IsIn;
	}
	_int		GetShield() {
		return m_Shield;
	}
	_bool	GetChangeForm() {
		return m_ChangeForm;
	}

public:
	void		PickUp(PickUps::TYPE_ID _ID);

private:
	void KeyCheck(_double TimeDelta);
	void SetCamAndSkyBox();
	RAY CreateRay();
	void  IsIn(_vector _MyPosition);

private:
	void	 PickUpHealth();
	void	 PickUpShield();
	void	 PickUpSMGAmmo();
	void	 PickUpShotGunAmmo();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;

	CModel*					m_pModelCom = nullptr;
	CModel*					m_pZeroCom = nullptr;

	CCollider*					m_ColliderCom = nullptr;
	CCollider*					m_ColliderSphereCom = nullptr;
	Navigation*				m_Navigation = nullptr;

	MOVE_TYPE				m_Move = MOVE_TYPE::MOVE_TYPE_END;
	ROTATION_TYPE			m_Rotation = ROTATION_TYPE::ROTATION_TYPE_END;

private:
	CHierarchyNode*			m_CameraBone = nullptr;
	CHierarchyNode*			m_WeaponBone = nullptr;

private:
	_bool		m_Shot = FALSE;
	_bool		m_InteractPick = FALSE;

	_float		m_HPPercent = 0.f;
	_float		m_ShieldPercent = 0.f;

private:
	_int			m_Shield;
	_int			m_MaxShield;

	_bool		m_IsIn = TRUE;

private:
	_bool		m_ChangeForm = FALSE;
	_double		m_ChangeFormDelay = 0.0;

private:
	class CCamera_Dynamic*		m_Camera = nullptr;

private:
	virtual HRESULT SetUp_Components();
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END