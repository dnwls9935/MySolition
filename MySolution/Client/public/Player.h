#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
protected:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CGameObject& rhs);
	virtual ~CPlayer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void		Rotation_AxisRotation_Axis(_fvector vAxis, _double TimeDelta);
	_matrix Get_CameraMatrix();

private:
	void		KeyCheck(_double TimeDelta);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
private:
	virtual HRESULT SetUp_Components();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END