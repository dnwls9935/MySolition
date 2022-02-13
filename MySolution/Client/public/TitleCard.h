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

class TitleCard final : public CGameObject
{
protected:
	explicit TitleCard(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit TitleCard(const CGameObject& rhs);
	virtual ~TitleCard() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture*				m_pTitleCardCom = nullptr;
	CTexture*				m_BackgroundCom  = nullptr;

	CTransform*			m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	XMMATRIX				m_ProjMatrix;

	_float							m_Size = 2.f;
	_float							m_X = 0.f;
	_float							m_Y = 0.f;

private:
	class BossPrimeBeast*		m_Boss = nullptr;
	class CCamera_Dynamic*		m_Camera = nullptr;

private:
	virtual HRESULT SetUp_Components();

public:
	static TitleCard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END