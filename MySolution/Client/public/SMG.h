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

class SMG final : public CGameObject
{
public:
	enum class ANIMATION_STATE {
		ALL,
		RE_DAHL, RE_HYPERION, FIRE_TEDIORE, FIRE_MALIWAN, FIRE_HYPERION, FIRE_DAHL, RE_TEDIORE_F, RE_BANDITMADE_F, RE_MALIWAN_F,
		RE_DAHL_F, RE_BANDITMADE, RE_MALIWAN, RE_HYPERION_F, RE_MALIWAN_F_V2, RE_TEDIORE, RE_MALIWAN_V2,
		ANIMATION_STATE_END
	};
protected:
	explicit SMG(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit SMG(const CGameObject& rhs);
	virtual ~SMG() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void SetUpWeapon(_fmatrix WeaponeBoneMatrix, _fmatrix PlayerWorldMatrix);
	_bool	GetFireFrame() const {
		return m_FireFrame; 
	}

	_int		GetAttackDamage() const {
		return m_AttackDamage;
	}

private:
	void KeyCheck();
	void		Reloading();
	
private:
	_bool	m_AnimationPlay = FALSE;
	_bool	m_FireFrame = FALSE;
	_double	m_FrameSpeed = 1.0;

private:
	_int			m_MaxAmmo = 35;
	_int			m_Ammo = 35;
	_int			m_Magazine = 70;
	_bool		m_Reload = FALSE;
	_float		m_BarPercent = 0.f;

private:
	_int			m_AttackDamage = 80;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	class CGameObject* m_TargetObject = nullptr;

private:
	virtual HRESULT SetUp_Components();
public:
	static SMG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END