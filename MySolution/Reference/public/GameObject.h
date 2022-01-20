#pragma once

/* Ŭ���̾�Ʈ�� ������ ���ӿ��(��ü, ������Ʈ)���� �θ� �Ǵ� Ŭ������. */
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	/* ������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct_Prototype();
	/* ��������ü�� �����ɶ� ȣ��Ǵ� �Լ�. */
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	class CComponent*		GetComponent(const _tchar* _componentTag);
	void	CheckHit(_fvector _AimRay, _fvector _AimDir);

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	_bool			m_Dead = FALSE;

	_bool			Dying = FALSE;
	_int				m_HP = 3;

protected:
	unordered_map<const _tchar*, class CComponent*>		m_Components;

protected:
	virtual HRESULT SetUp_Components(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);
public:
	virtual CGameObject* Clone(void* pArg) = 0; 
	virtual void Free() override;
};

END