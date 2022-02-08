#pragma once

/* 클라이언트에 정의할 게임요소(객체, 오브젝트)들의 부모가 되는 클래스다. */
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum class OBJTYPE_ID { ENVIRONMENT,  COLLISION_ENVIORNMENT, TERRAIN, INTERACTION , PLAYER, PLAYER_WEAPONE, ENEMY,ENEMY_OBJECT ,BOSS, OBJTYPE_END};

	enum class UITYPE_ID {PLAYER_HP, PLAYER_SHILED, EXP, GRENADE, AMMO, UITYPE_END};

public:
	typedef struct tagToolObjectDesc {
		_tchar	m_BufferTag[MAX_PATH] = TEXT("");
		_tchar  m_ObjTag[MAX_PATH] = TEXT("");
		_float3			m_Position = { 0.f, 0.f, 0.f };
		_float4x4		m_pTransformMatrix = _float4x4();
		_bool				loadCheck = FALSE;
		OBJTYPE_ID		m_Type = OBJTYPE_ID::OBJTYPE_END;
	}TOOLOBJDESC;

public:
	typedef struct tagUiObject {
		_vector			m_Position;
		_tchar				m_TextureTag[MAX_PATH] = L"";
		_bool				m_LoadCheck = FALSE;
		_matrix			m_LoadMatrix = XMMatrixIdentity();
		UITYPE_ID		m_UITypeID = UITYPE_ID::UITYPE_END;
	}UIOBJDESC;

protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	/* 원형객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct_Prototype();
	/* 복제본객체가 생성될때 호출되는 함수. */
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

	virtual	_bool		Picked();

	_int		GetHP() const { 
		return m_HP; 
	};
	_bool		GetDead() const {
		return m_Dead;
	};
	_bool		GetIntroEnd() const {
		return m_IntroEnd;
	}

public:
	class CComponent*		GetComponent(const _tchar* _componentTag);
	void		SetHp(_int _Hp) {
		m_HP += _Hp;
	};

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	_bool			m_Dead = FALSE;

	_bool			Dying = FALSE;
	_int				m_HP = 0;
	_int				m_MaxHP = 0;

	_bool			m_IntroEnd = FALSE;

	OBJTYPE_ID		m_Type = OBJTYPE_ID::OBJTYPE_END;

protected:
	unordered_map<const _tchar*, class CComponent*>		m_Components;

protected:
	virtual HRESULT SetUp_Components(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);
public:
	virtual CGameObject* Clone(void* pArg) = 0; 
	virtual void Free() override;
};

END