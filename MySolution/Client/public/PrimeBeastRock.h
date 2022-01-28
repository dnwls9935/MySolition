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

class PrimeBeastRock final : public CGameObject
{
public:
	enum class TYPE_ROCK { SMALL, BIG, TYPE_ROCK_END };
	enum class TYPE_THROW {DIRECT, CURVE, TYPE_THROW_END};

	enum class ANIMATION_STATE {
		ALL,
		RE_DAHL, RE_HYPERION, FIRE_TEDIORE, FIRE_MALIWAN, FIRE_HYPERION, FIRE_DAHL, RE_TEDIORE_F, RE_BANDITMADE_F, RE_MALIWAN_F,
		RE_DAHL_F, RE_BANDITMADE, RE_MALIWAN, RE_HYPERION_F, RE_MALIWAN_F_V2, RE_TEDIORE, RE_MALIWAN_V2,
		ANIMATION_STATE_END
	};

	typedef struct tagPrimeBeastRockDesc {
		CGameObject* m_PrimeBeast = nullptr;
		_matrix				m_SpawnMatrix = XMMatrixIdentity();
		_float3				m_TargetPosition = _float3();
		TYPE_ROCK		m_TypeRock = TYPE_ROCK::TYPE_ROCK_END;
		TYPE_THROW	m_TypeThrow = TYPE_THROW::TYPE_THROW_END;
	}PRBDESC;

protected:
	explicit PrimeBeastRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit PrimeBeastRock(const CGameObject& rhs);
	virtual ~PrimeBeastRock() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_RockModel = nullptr;
	CModel*					m_SnowModel = nullptr;

	PRBDESC					m_PrbDesc;

private:
	virtual HRESULT SetUp_Components();
public:
	static PrimeBeastRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END