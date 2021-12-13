#pragma once

#include "Base.h"

/* 내가 나눈기준을 의미한다. */
BEGIN(Engine)

class CLayer final : public CBase
{
public:
	CLayer();
	virtual ~CLayer() = default;
public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	_int Tick(_double TimeDelta);
	_int LateTick(_double TimeDelta);

private:
	list<class CGameObject*>			m_Objects;
	typedef list<class CGameObject*>	OBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END