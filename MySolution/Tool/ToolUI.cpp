#include "stdafx.h"
#include "ToolUI.h"

#include "GameInstance.h"
#include "ToolDefines.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Form.h"
#include "TapUI.h"
#include "UIObject.h"

ToolUI::ToolUI(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: CGameObject(_dx11Device, _dx11DeviceContext)
{
}

ToolUI::ToolUI(const ToolUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT ToolUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT ToolUI::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	
	return S_OK;
}

_int ToolUI::Tick(_double TimeDelta)
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form * m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
	if (2 != m_form->m_tabCtrl.GetCurSel())
		return _int();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint count = m_form->tapUI->UIListBox.GetCount();
	_uint focus = m_form->tapUI->UIListBox.GetCurSel();

	_bool	modify = m_form->tapUI->m_Modify.GetCheck();
	_bool	batch = m_form->tapUI->m_Batch.GetCheck();

	if (pGameInstance->Get_MouseButtonState(CInput_Device::MBS_LBUTTON))
	{
		if ((pGameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80) &&
			TRUE == batch &&
			count >= focus)
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(g_hWnd, &pt);
			_vector WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, 1280, 720, pt.x, pt.y);

			CString str = L"";
			m_form->tapUI->UIListBox.GetText(focus, str);

			CGameObject::UIOBJDESC		UIObjDesc;
			ZeroMemory(&UIObjDesc, sizeof(UIObject::UIOBJDESC));
			UIObjDesc.m_Position = WindowPosition;
			lstrcpy(UIObjDesc.m_TextureTag, str);
			UIObjDesc.m_UITypeID = (CGameObject::UITYPE_ID)(m_form->VecUIType[focus]);

			if (FAILED(pGameInstance->Add_GameObjectToLayer(1, TEXT("UI"), TEXT("Prototype_GameObject_UIObject"), &UIObjDesc)))
				return 0;

			m_form->tapUI->m_Batch.SetCheck(FALSE);
		}
		else if(TRUE == modify){
			POINT pt;
			ZeroMemory(&pt, sizeof(POINT));
			GetCursorPos(&pt);
			ScreenToClient(g_hWnd, &pt);

			list<CGameObject*> ObjList = pGameInstance->GetObjectList(1, TEXT("UI"));

			for (auto& Object : ObjList)
			{
				if (TRUE == static_cast<UIObject*>(Object)->PickCheck(pt))
				{
					static_cast<UIObject*>(Object)->SetPickCheck(TRUE);
				}
				else {
					static_cast<UIObject*>(Object)->SetPickCheck(FALSE);
				}
			}
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int ToolUI::LateTick(_double TimeDelta)
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form * m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
	if (2 != m_form->m_tabCtrl.GetCurSel())
		return _int();


	return _int();
}

HRESULT ToolUI::Render()
{
	return S_OK;
}


ToolUI * ToolUI::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
{
	ToolUI* pInstance = new ToolUI(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX(L"Failed to Create ToolUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * ToolUI::Clone(void * pArg)
{
	ToolUI* pInstance = new ToolUI(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(L"Failed to Clone ToolUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void ToolUI::Free()
{
	__super::Free();

}
