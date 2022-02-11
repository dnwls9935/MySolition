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

class HITUI final : public CGameObject
{
public:
	enum class SHOW {SHIELD, HP, SHOW_END};

protected:
	explicit HITUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit HITUI(const CGameObject& rhs);
	virtual ~HITUI() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void		SetShow(SHOW _Show) {
		m_TimeAcc = 0.0;
		m_ID = _Show;
	}

private:
	HRESULT		LeftHit();
	HRESULT		RightHit();
	HRESULT		CenterHit();

private:
	CTexture*				m_pTextureCom = nullptr;
	CTransform*				m_pTransformCom[3] = {nullptr};
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	
	XMMATRIX				m_ProjMatrix;

	CGameObject::UIOBJDESC			m_UIDesc;

	SHOW						m_ID = SHOW::SHOW_END;
	_bool						m_Show = FALSE;

	_double					m_TimeAcc = 0.0;

private:
	virtual HRESULT SetUp_Components();

public:
	static HITUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END