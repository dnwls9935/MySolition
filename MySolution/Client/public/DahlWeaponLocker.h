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

class DahlWeaponLocker final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		CLOSED_IDLE, OPEN, OPENED_IDLE,
		ANIMATION_STATE_END};
protected:
	explicit DahlWeaponLocker(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit DahlWeaponLocker(const CGameObject& rhs);
	virtual ~DahlWeaponLocker() = default;
public:
	/* ������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* ��������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_ColliderCom = nullptr;

private:
	virtual HRESULT SetUp_Components();
public:
	static DahlWeaponLocker* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END