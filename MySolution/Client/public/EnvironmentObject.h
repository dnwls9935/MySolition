#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class EnvrionmentObject final : public CGameObject
{
protected:
	explicit EnvrionmentObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit EnvrionmentObject(const CGameObject& rhs);
	virtual ~EnvrionmentObject() = default;
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

	const _tchar*			m_ObjectTag = nullptr;

private:
	virtual HRESULT SetUp_Components();

public:
	static EnvrionmentObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END