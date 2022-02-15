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

class Num final : public CGameObject
{
protected:
	explicit Num(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit Num(const CGameObject& rhs);
	virtual ~Num() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void TickHP();
	void TickShield();
	void TickAmmo();
	void TickMagazine();

	HRESULT RenderHP();
	HRESULT RenderHP0();
	HRESULT RenderHP1();
	HRESULT RenderHP2();
	HRESULT RenderHP3();

	HRESULT RenderShield();
	HRESULT RenderShield0();
	HRESULT RenderShield1();
	HRESULT RenderShield2();

	HRESULT RenderAmmo();
	HRESULT RenderAmmo0();
	HRESULT RenderAmmo1();

	HRESULT RenderSlash();

	HRESULT RenderMagazine();
	HRESULT RenderMagazine0();
	HRESULT RenderMagazine1();
	HRESULT RenderMagazine2();


private:
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pSlashCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	
	XMMATRIX				m_ProjMatrix;

	class CCamera_Dynamic*		m_Camera = nullptr;
	CGameObject*							m_Player = nullptr;
	CGameObject*								m_SMG = nullptr;

private:
	_int H1 = 0;
	_int H2 = 0;
	_int H3 = 0;
	_int H4 = 0;

	_int S1 = 0;
	_int S2 = 0;
	_int S3 = 0;

	_int A1 = 0;
	_int A2 = 0;

	_int M1 = 0;
	_int M2 = 0;
	_int M3 = 0;

private:
	_float2		HpPos;
	_float2		ShieldPos;
	_float2		AmmoPos;

	_float			Gap;


private:
	virtual HRESULT SetUp_Components();

public:
	static Num* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END