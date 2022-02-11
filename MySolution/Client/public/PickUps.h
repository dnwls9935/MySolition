#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)

class PickUps final : public CGameObject
{
public:
	enum class TYPE_ID{SMG_AMMO, SHOTGUN_AMMO, HEALTH_VIRAL, BOOSTER_SHILED, TYPE_ID_END};

	typedef struct tagPickUpsDesc {
		TYPE_ID					m_TypeID = TYPE_ID::TYPE_ID_END;
		CGameObject*		m_Parent = nullptr;
		_matrix					m_WorldMatrix = XMMatrixIdentity();
	}PUDESC;

protected:
	explicit PickUps(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit PickUps(const CGameObject& rhs);
	virtual ~PickUps() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_ModelCom = nullptr;
	CCollider*					m_ColliderSphere = nullptr;

	PUDESC						m_PUDesc;

	_bool							m_IsShow = FALSE;
	_bool							m_Chase = FALSE;

private:
	virtual HRESULT SetUp_Components();

public:
	static PickUps* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END