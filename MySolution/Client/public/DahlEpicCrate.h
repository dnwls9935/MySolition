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

class DahlEpicCrate final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		OPEN, CLOSED_IDLE, OPENED_IDLE,
		ANIMATION_STATE_END};
protected:
	explicit DahlEpicCrate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit DahlEpicCrate(const CGameObject& rhs);
	virtual ~DahlEpicCrate() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
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
	static DahlEpicCrate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END