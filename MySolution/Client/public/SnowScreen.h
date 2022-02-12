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

class SnowScreen final : public CGameObject
{
protected:
	explicit SnowScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit SnowScreen(const CGameObject& rhs);
	virtual ~SnowScreen() = default;
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

private:
	_float							m_X = 0.f;
	_float							m_Y = 0.f;
	_float							m_AlphaValue = 0.f;

	_bool							m_Show = TRUE;

	CGameObject*			m_Player = nullptr;
	class CCamera_Dynamic*		m_Camera = nullptr;


private:
	virtual HRESULT SetUp_Components();

public:
	static SnowScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END