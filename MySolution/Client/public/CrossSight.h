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

class CrossSIght final : public CGameObject
{
protected:
	explicit CrossSIght(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CrossSIght(const CGameObject& rhs);
	virtual ~CrossSIght() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	CGameObject::UIOBJDESC		GetUIDesc() {
		return m_UIDesc;
	};

	void			SetLength(_float	_Percent, _bool _LRCheck);

private:
	CTexture*				m_pTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	
	XMMATRIX				m_ProjMatrix;

	CGameObject::UIOBJDESC			m_UIDesc;

	_float							m_BarPercent = 1.f;
	_bool							m_LRCheck = FALSE;

private:
	virtual HRESULT SetUp_Components();

public:
	static CrossSIght* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END