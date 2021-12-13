#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CBackGround final : public CGameObject
{
public:
	typedef struct tagBackGroundDesc
	{
		_float		fX, fY, fZ;
	}BACKGROUNDDESC;

protected:
	explicit CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBackGround(const CGameObject& rhs);
	virtual ~CBackGround() = default;
public:
	/* ������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct_Prototype() override;
	/* ��������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
private:
	BACKGROUNDDESC			m_BackGroundDesc;
private:
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	
	XMMATRIX				m_ProjMatrix;
	XMMATRIX				m_WorldMatrix;

private:
	virtual HRESULT SetUp_Components();

public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END