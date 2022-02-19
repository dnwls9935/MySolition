#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)

class CrossSight final : public CGameObject
{
protected:
	explicit CrossSight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CrossSight(const CGameObject& rhs);
	virtual ~CrossSight() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	XMMATRIX				m_ProjMatrix;
	list<CGameObject*>			m_EnemyObjectList;
	_bool								m_IsCollision = FALSE;

private:
	class CCamera_Dynamic*		m_Camera = nullptr;
	class CPlayer*							m_Player = nullptr;

private:
	virtual HRESULT SetUp_Components();

public:
	static CrossSight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END