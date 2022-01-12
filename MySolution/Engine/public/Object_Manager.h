#pragma once

#include "Base.h"

/* 객체(원형)들을 모아서 관리한다. */
/* 객체(복사본)들을 레벨별로 나의 기준에 따라 나누어서(CLayer)  관리한다.*/
/* 보유하고있는 객체들의 Tick을 호출해주낟. */
/* 보유하고있는 객체들의 LateTick을 호출해주낟. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg);
	_int Tick(_double TimeDelta);
	_int LateTick(_double TimeDelta);
public:
	HRESULT Clear_Object_Manager(_uint iLevelIndex);
	CGameObject*	FindObject(const _tchar* _tag);
	list<class CGameObject*>& GetObjectList(_uint _levelIdx, const _tchar* _tag);
	//_uint		GetNumLevel() const { return m_iNumLevels; };

private:
	unordered_map<const _tchar*, class CGameObject*>			m_Prototype;
	typedef unordered_map<const _tchar*, class CGameObject*>	PROTOTYPES;

	unordered_map<const _tchar*, class CLayer*>*				m_pLayers = nullptr;
	typedef unordered_map<const _tchar*, class CLayer*>			LAYERS;

private:
	CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
private:
	_uint			m_iNumLevels = 0;
public:
	virtual void Free() override;
};

END