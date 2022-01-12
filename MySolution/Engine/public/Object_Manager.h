#pragma once

#include "Base.h"

/* ��ü(����)���� ��Ƽ� �����Ѵ�. */
/* ��ü(���纻)���� �������� ���� ���ؿ� ���� �����(CLayer)  �����Ѵ�.*/
/* �����ϰ��ִ� ��ü���� Tick�� ȣ�����ֳ�. */
/* �����ϰ��ִ� ��ü���� LateTick�� ȣ�����ֳ�. */

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