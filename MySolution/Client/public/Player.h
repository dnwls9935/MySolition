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

class CPlayer final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL, CROUCH, DRAW, GRENADE_THROW, HOLSTER, IDLE,
		JUMP_END, JUMP_IDLE, JUMP_START,
		RELOAD_BANDITMADE, RELOAD_BANDITMADE_FAST,
		RELOAD_DAHL, RELOAD_DAHL_FAST,
		RELOAD_HYPERION, RELOAD_HYPERION_FAST,
		RELOAD_JAKOBS, RELOAD_JAKOBS_FAST,
		RELOAD_MALIWAN, RELOAD_MALIWAN_FAST,
		RELOAD_TORGUE, RELOAD_TORGUE_FAST, RELOAD_TEDIORE,
		RELOAD_VLADOF, RELOAD_VLADOF_FAST,
		RUN_F, RUN_L, RUN_R,
		SIGHT_IDLE, SIGHT_JUMP_END, SIGHT_JUMP_IDLE, SIGHT_JUMP_START, SIGHT_WALK_F,
		SPRINT, SPRINT_DUALWIELD,
		RELOAD_JAKOBS_VAR2, RELOAD_JAKOBS_FAST_VAR2,
		BLADED_MELEE, 
		RELOAD_DUALWIELD, HOLSTER_FAST,
		INCAP_F, RELORD_TEDIROE_FAST,
		ADD_FIRE_RECOIL, ADD_FIRE_REVOLVE,
		PICKUP_EQUIP, FIRE_FAN_RECOIL,
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

private:
	void KeyCheck(_double TimeDelta);
	void SetCamAndSkyBox();
	void Shotting();

	RAY CreateRay();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	CCollider*					m_ColliderCom = nullptr;

	MOVE_TYPE				m_Move = MOVE_TYPE::MOVE_TYPE_END;
	ROTATION_TYPE			m_Rotation = ROTATION_TYPE::ROTATION_TYPE_END;

private:
	CHierarchyNode*			m_CameraBone = nullptr;
private:
	virtual HRESULT SetUp_Components();
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END