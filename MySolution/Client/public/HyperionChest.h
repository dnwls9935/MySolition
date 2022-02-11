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
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	_matrix		GetBoneMatrix();

	_bool		GetOpend() {
		return m_Opend;
	}
	_bool		GetIteShow() {
		return m_ItemShow;
	}
private:
	void		Picking();
	HRESULT		SettingItem();

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*					m_ColliderCom = nullptr;

	CHierarchyNode*			m_ItemPositionBone = nullptr;

private:
	_bool				m_Opend = FALSE;
	_bool				m_ItemShow = TRUE;

private:
	virtual HRESULT SetUp_Components();
public:
	static HyperionChest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END