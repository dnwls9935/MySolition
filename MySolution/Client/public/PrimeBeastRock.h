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

	typedef struct tagPrimeBeastRockDesc {
		CGameObject* m_PrimeBeast = nullptr;
		CGameObject* m_Target = nullptr;
		_matrix				m_SpawnMatrix = XMMatrixIdentity();
		_vector				m_TargetPosition = _vector();
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

	void		CheckState(_double TimeDelta);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_RockModel = nullptr;
	CCollider*					m_ColliderCom = nullptr;

	PRBDESC					m_PrbDesc;
	_bool							m_ChaseTarget = FALSE;

private:
	virtual HRESULT SetUp_Components();
public:
	static PrimeBeastRock* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END