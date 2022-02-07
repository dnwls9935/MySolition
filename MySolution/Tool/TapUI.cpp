// TapUI.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapUI.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "Form.h"
#include "Layer.h"
#include "UIObject.h"
#include "GameObject.h"

// TapUI 대화 상자입니다.

IMPLEMENT_DYNAMIC(TapUI, CDialogEx)

TapUI::TapUI(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TAPUI, pParent)
{

}

TapUI::~TapUI()
{
}

void TapUI::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CDialogEx::PostNcDestroy();
}

void TapUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, UIListBox);
	DDX_Control(pDX, IDC_CHECK1, m_Batch);
	DDX_Control(pDX, IDC_CHECK2, m_Modify);
}


BEGIN_MESSAGE_MAP(TapUI, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST1, &TapUI::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_CHECK1, &TapUI::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON1, &TapUI::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &TapUI::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK2, &TapUI::OnBnClickedCheck2)
END_MESSAGE_MAP()


// TapUI 메시지 처리기입니다.


void TapUI::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void TapUI::OnBnClickedCheck1()
{
	if(TRUE == m_Batch.GetCheck())
		m_Modify.SetCheck(FALSE);
}

/* Save */
void TapUI::OnBnClickedButton1()
{
	CFileDialog dlg(FALSE, L"dat", L"*.dat");
	TCHAR pFilePath[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, pFilePath);
	PathRemoveFileSpec(pFilePath);
	lstrcat(pFilePath, L"\\Client\\Data\\");
	dlg.m_ofn.lpstrInitialDir = pFilePath;

	if (IDOK == dlg.DoModal())
	{
		CString strFilePath = dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		list<CGameObject*>	ObjectList = pGameInstance->GetObjectList(1, TEXT("UI"));
		_ulong dwByte = 0;

		for (auto& Object : ObjectList)
		{
			UIObject::UIOBJDESC UIOBJDesc = static_cast<UIObject*>(Object)->m_UIOBJDesc;

			UIOBJDesc.m_LoadMatrix = static_cast<CTransform*>(Object->GetComponent(TEXT("Com_Transform")))->Get_WorldMatrix();

			WriteFile(hFile, &UIOBJDesc, sizeof(UIObject::UIOBJDESC), &dwByte, nullptr);
		}
		RELEASE_INSTANCE(CGameInstance);
		CloseHandle(hFile);
	}
}

/* Load */
void TapUI::OnBnClickedButton2()
{
	CFileDialog dlg(true, L"dat", L"*.dat");

	TCHAR szFilePath[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szFilePath);
	PathRemoveFileSpec(szFilePath);
	lstrcat(szFilePath, L"\\Client\\Data\\");
	dlg.m_ofn.lpstrInitialDir = szFilePath;

	if (IDOK == dlg.DoModal()) {
		CString strFilePath = dlg.GetPathName();
		HANDLE hFile = CreateFile(strFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		DWORD dwByte = 0;
		while (TRUE)
		{
			UIObject::UIOBJDESC		UIObjectDesc;
			ZeroMemory(&UIObjectDesc, sizeof(UIObject::UIOBJDESC));

			ReadFile(hFile, &UIObjectDesc, sizeof(UIObject::UIOBJDESC), &dwByte, nullptr);
			UIObjectDesc.m_LoadCheck = TRUE;

			if (0 == dwByte)
				break;

			pGameInstance->Add_GameObjectToLayer(1, TEXT("UI"), TEXT("Prototype_GameObject_UIObject"), &UIObjectDesc);
		}

		RELEASE_INSTANCE(CGameInstance);
		CloseHandle(hFile);
	}
}


void TapUI::OnBnClickedCheck2()
{
	if (TRUE == m_Modify.GetCheck())
		m_Batch.SetCheck(FALSE);
}
