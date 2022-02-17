#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCollider;
class Navigation;
class HP;
class CTexture;
END

BEGIN(Client)

class ClapTrap final : public CGameObject
{
public:
	enum class ANIMATION_STATE {ALL,
		DANCE_1, DANCE_2, DANCE_3, DANCE_4, DANCE_5,
		ANIMATION_STATE_END};
protected:
	explicit ClapTrap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit ClapTrap(const CGameObject& rhs);
	virtual ~ClapTrap() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*					m_pBurnedTextureCom = nullptr;

private:
	_bool							m_Burned = FALSE;
	_float							m_BurnedTime = 0.f;
	_float3						m_BurnColor;
private:
	virtual HRESULT SetUp_Components();
public:
	static ClapTrap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END