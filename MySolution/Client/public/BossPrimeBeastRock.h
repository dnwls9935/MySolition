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

class BossPrimeBeastRock final : public CGameObject
{
public:
	enum class TYPE_ROCK { SMALL, BIG, TYPE_ROCK_END };
	enum class TYPE_THROW {DIRECT, CURVE, TYPE_THROW_END};
	enum class TYPE_BOSS_ATTACK { ATT_TR_V1, ATT_TR_V2, ATT_TR_V3, ATT_TR_V4, ATT_TG_V1, ATT_TG_V2,
		TYPE_BOSS_ATTACK_END};

public:
	typedef struct tagBossPrimeBeastRockDesc {
		CGameObject*				m_PrimeBeast = nullptr;
		CGameObject*				m_Target = nullptr;
		class CHierarchyNode*				m_BossBone = nullptr;
		_vector							m_TargetPosition = _vector();
		TYPE_ROCK					m_TypeRock = TYPE_ROCK::TYPE_ROCK_END;
		TYPE_THROW				m_TypeThrow = TYPE_THROW::TYPE_THROW_END;
		TYPE_BOSS_ATTACK		m_Type_Attack = TYPE_BOSS_ATTACK::TYPE_BOSS_ATTACK_END;
	}BPRBDESC;

protected:
	explicit BossPrimeBeastRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit BossPrimeBeastRock(const CGameObject& rhs);
	virtual ~BossPrimeBeastRock() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void		CheckState(_double TimeDelta);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_RockModel = nullptr;
	CCollider*					m_ColliderCom = nullptr;

	BPRBDESC					m_PrbDesc;
	_bool							m_ChaseTarget = FALSE;

private:
	virtual HRESULT SetUp_Components();
public:
	static BossPrimeBeastRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END