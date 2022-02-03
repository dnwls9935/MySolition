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
		ATT_BITE, BURROW_ENTER, BURROW_EXIT, RUN_F, DEA_CRITICAL, SPAWN,
		ANIMATION_STATE_END};
protected:
	explicit BugMorph(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit BugMorph(const CGameObject& rhs);
	virtual ~BugMorph() = default;
public:
	/* ������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* ��������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
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

	CHierarchyNode*		m_LowerJawBone = nullptr;

	CCollider*					m_ColliderLowerJaw = nullptr;

	Navigation*				m_Navigation = nullptr;

	list<class CGameObject*>* m_TargetObjectList = nullptr;

	_bool							m_FrameStart = FALSE;
	_bool							m_IntroEnd = FALSE;
	/* BugMorph �۾��ؾ��� */	
private:
	CGameObject* m_TargetPlayer = nullptr;
	CGameObject* m_TargetPlayerWeapon = nullptr;
	CGameObject*	m_Terrain = nullptr;

	_vector				m_PlayerPosition = _vector();
	_vector				m_MyPosition = _vector();
	_float					m_TargetDistance = 0.f;


private:
	virtual HRESULT SetUp_Components();
public:
	static BugMorph* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END