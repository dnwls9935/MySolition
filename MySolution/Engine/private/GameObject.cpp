#include "..\public\GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);	
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_Dead(rhs.m_Dead)
	, m_Type(rhs.m_Type)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg)
{
	m_Components.reserve(0);
	return S_OK;
}

_int CGameObject::Tick(_double TimeDelta)
{
	return _int();
}

_int CGameObject::LateTick(_double TimeDelta)
{
	if (0 >= m_HP)
		m_Dead = TRUE;

	return m_Dead;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

CComponent * CGameObject::GetComponent(const _tchar * _componentTag)
{
	auto& iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(_componentTag));

	if (m_Components.end() == iter)
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::SetUp_Components(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter != m_Components.end())
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	/* 맵에도 보관. + 자식의 변수에도 저장. */
	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);
	m_Components.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
