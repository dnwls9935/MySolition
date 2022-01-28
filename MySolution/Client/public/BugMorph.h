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

class BugMorph final : public CGameObject
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
	explicit BugMorph(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit BugMorph(const CGameObject& rhs);
	virtual ~BugMorph() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void		HitCheck();
	void		Intro();
	void		Animation();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_ColliderCom = nullptr;

	list<class CGameObject*>* m_TargetObjectList = nullptr;

	_bool						m_FrameStart = FALSE;

private:
	virtual HRESULT SetUp_Components();
public:
	static BugMorph* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END