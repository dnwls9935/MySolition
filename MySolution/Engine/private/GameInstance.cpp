#include "..\public\GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
{
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CGameInstance::Initialize_Engine(HWND hWnd, _uint iNumLevel, CGraphic_Device::WINMODE eWinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pObject_Manager)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(hWnd, eWinMode, iWinCX, iWinCY, ppDeviceOut, ppDeviceContextOut)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevel)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevel)))
		return E_FAIL;	

	return S_OK;
}

_int CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pLevel_Manager ||
		 nullptr == m_pObject_Manager)
		return -1;

	_int	iProgress = 0;

	iProgress = m_pLevel_Manager->Tick(TimeDelta);
	if (0 > iProgress)
		return -1;

	iProgress = m_pObject_Manager->Tick(TimeDelta);
	if (0 > iProgress)
		return -1;

	iProgress = m_pObject_Manager->LateTick(TimeDelta);
	if (0 > iProgress)
		return -1;

	return _int();
}

HRESULT CGameInstance::Render_Engine()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pLevel_Manager->Render();	

	return S_OK;
}

HRESULT CGameInstance::Clear_Engine(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pObject_Manager->Clear_Object_Manager(iLevelIndex);

	return S_OK;
}

HRESULT CGameInstance::Clear_BackBuffer_View(XMFLOAT4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);;
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

_double CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

void CGameInstance::Update_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Update_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Ready_Timer(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pOpenLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pOpenLevel);
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(iLevelIndex, pLayerTag, pPrototypeTag, pArg);	
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

void CGameInstance::Release_Engine()
{

	if (0 != CGameInstance::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release CGameInstance");	

	if (0 != CLevel_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release CLevel_Manager");

	if (0 != CObject_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release CObject_Manager");

	if (0 != CComponent_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release CComponent_Manager");

	if (0 != CTimer_Manager::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release CTimer_Manager");

	if (0 != CGraphic_Device::GetInstance()->DestroyInstance())
		MSGBOX("Failed to Release CGraphic_Device");
}

void CGameInstance::Free()
{
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pGraphic_Device);
}


