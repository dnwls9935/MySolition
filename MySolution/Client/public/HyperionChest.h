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

class HyperionChest final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		CLOSED_IDLE, OPEN, OPENED_IDLE,
		ANIMATION_STATE_END};
protected:
	explicit HyperionChest(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit HyperionChest(const CGameObject& rhs);
	virtual ~HyperionChest() = default;
public:
	/* ������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* ��������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void		Picking();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_ColliderCom = nullptr;

private:
	_bool				m_Opend = FALSE;

private:
	virtual HRESULT SetUp_Components();
public:
	static HyperionChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END