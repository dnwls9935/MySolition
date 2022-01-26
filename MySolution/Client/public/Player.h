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

class CPlayer final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL, 
		IDLE, RELOAD, RUN_F, RUN_L, RUN_B, SPRINT,
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
	CCollider*					m_ColliderSphereCom = nullptr;

	Navigation*				m_Navigation = nullptr;

	MOVE_TYPE				m_Move = MOVE_TYPE::MOVE_TYPE_END;
	ROTATION_TYPE			m_Rotation = ROTATION_TYPE::ROTATION_TYPE_END;

private:
	CHierarchyNode*			m_CameraBone = nullptr;
	CHierarchyNode*			m_WeaponBone = nullptr;

private:
	_bool		m_Shot = FALSE;

private:
	virtual HRESULT SetUp_Components();
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END