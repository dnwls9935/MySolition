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

class Play final : public CGameObject
{
protected:
	explicit Play(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Play(const CGameObject& rhs);
	virtual ~Play() = default;
public:
	/* ������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* ��������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	_bool			GetPicked() {
		return m_Picked;
	}

private:
	CTexture*				m_pTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	
	XMMATRIX				m_ProjMatrix;

	_bool							m_Picked = FALSE;

	_float							m_X = 0.f;

private:
	virtual HRESULT SetUp_Components();

public:
	static Play* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END