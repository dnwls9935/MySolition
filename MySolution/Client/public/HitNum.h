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

class HitNum final : public CGameObject
{
public:
	typedef struct tagHitNum {
		_int Num = 0;
		_vector Position = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		CGameObject*	Parent = nullptr;
	}HUDESC;

protected:
	explicit HitNum(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit HitNum(const CGameObject& rhs);
	virtual ~HitNum() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Render100();
	HRESULT Render10();
	HRESULT Render1();


private:
	CTexture*				m_pTextureCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	
	XMMATRIX				m_ProjMatrix;

	class CCamera_Dynamic*		m_Camera = nullptr;
	CGameObject*							m_Parent = nullptr;


private:
	_int Num = 0;
	
	_int Num100 = 0;
	_int Num10 = 0;
	_int Num1 = 0;

private:
	_double			m_LifeTime = 0.0;

private:
	_double m_DeltaTime = 0.0;

private:
	virtual HRESULT SetUp_Components();

public:
	static HitNum* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END