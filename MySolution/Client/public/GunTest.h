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

class GunTest final : public CGameObject
{
public:
	enum class ANIMATION_STATE {
		ALL,
		FIRE, IDLE, RELOAD, FIRE_FAN,
		ANIMATION_STATE_END
	};
protected:
	explicit GunTest(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit GunTest(const CGameObject& rhs);
	virtual ~GunTest() = default;
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

private:
	void KeyCheck();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;

	_matrix						m_OriginMatrix = XMMatrixIdentity();

	class CGameObject* m_TargetObject = nullptr;

private:
	virtual HRESULT SetUp_Components();
public:
	static GunTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END