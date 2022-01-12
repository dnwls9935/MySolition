// TapMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapMap.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "Form.h"
#include "Layer.h"
#include "ToolObject.h"

// TapMap 대화 상자입니다.

IMPLEMENT_DYNAMIC(TapMap, CDialogEx)

TapMap::TapMap(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TAPMAP, pParent)
	, m_radioValue(0)
	, m_brushRadius(0)
{

}

TapMap::~TapMap()
{
}

void TapMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_radioValue);
	DDX_Text(pDX, IDC_EDIT2, m_brushRadius);
	DDX_Control(pDX, IDC_EDIT2, m_BrushRadiusText);
	DDX_Control(pDX, IDC_CHECK1, m_CullMode);
	DDX_Control(pDX, IDC_LIST1, m_objectListBox);
	DDX_Control(pDX, IDC_CHECK2, m_BatchObject);
	DDX_Control(pDX, IDC_CHECK3, m_Modify);
}


BEGIN_MESSAGE_MAP(TapMap, CDialogEx)
	ON_WM_CREATE()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, &TapMap::OnBnClieckRadio)
	ON_EN_CHANGE(IDC_EDIT2, &TapMap::OnEnChangeRadius)
	ON_BN_CLICKED(IDC_CHECK2, &TapMap::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &TapMap::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_BUTTON1, &TapMap::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &TapMap::OnBnClickedButton2)
END_MESSAGE_MAP()


HRESULT TapMap::SaveTerrainLayer(HANDLE& hFile)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> pGameObjectList = pGameInstance->GetObjectList(1, TEXT("Terrain"));

	ToolTerrain* pTerrain = static_cast<ToolTerrain*>(pGameObjectList.front());
	DWORD dwByte;

	_int x = pTerrain->GetX();
	_int z = pTerrain->GetZ();
	void* pVertices = pTerrain->GetVertices();

	for (_int i = 0; i <x ; i++)
	{
		for (_uint j = 0; j < z; j++)
		{
			_uint		iIndex = i * x + j;
			WriteFile(hFile, &((VTXNORTEX*)pVertices)[iIndex].vPosition.y, sizeof(_float), &dwByte, nullptr);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT TapMap::LoadTerrainLayer(HANDLE & hFile)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*>& pGameObjectList = pGameInstance->GetObjectList(1, TEXT("Terrain"));

	ToolTerrain* pTerrain = static_cast<ToolTerrain*>(pGameObjectList.front());

	DWORD dwByte;

	_int x = pTerrain->GetX();
	_float y = 0;
	_int z = pTerrain->GetZ();

	for (_int i = 0; i < x; i++)
	{
		for (_int j = 0; j < z; j++)
		{
			_int iIndex = i * x + j;

			ReadFile(hFile, &y, sizeof(_float), &dwByte, nullptr);
			static_cast<ToolTerrain*>(pGameObjectList.front())->SetVertexY(iIndex, y);
		}
	}

	static_cast<ToolTerrain*>(pGameObjectList.front())->SetNormalVector();

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT TapMap::LoadObjectLayer(HANDLE & hFile)
{
	return S_OK;
}

HRESULT TapMap::SaveObjectLayer(HANDLE& hFile)
{
	return S_OK;
}

void TapMap::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CDialogEx::PostNcDestroy();
}


BOOL TapMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


int TapMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

/* SaveBtn */
void TapMap::OnBnClickedButton1()
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

		if (FAILED(SaveTerrainLayer(hFile)))
			return;

		if (FAILED(SaveObjectLayer(hFile)))
			return;

	CloseHandle(hFile);
	}
}

void TapMap::OnBnClieckRadio(UINT value)
{
	UpdateData(TRUE);

	UpdateData(FALSE);
}


void TapMap::OnEnChangeRadius()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.


	
	CString str;
	m_BrushRadiusText.GetWindowTextW(str);
	m_brushRadius = _ttoi(str);
}


void TapMap::OnBnClickedCheck2()
{
	if (m_BatchObject.GetCheck()) {
		m_Modify.SetCheck(FALSE);
	}
	Invalidate(FALSE);
}


void TapMap::OnBnClickedCheck3()
{
	if (m_Modify.GetCheck()) {
		m_BatchObject.SetCheck(FALSE);
	}
	Invalidate(FALSE);
}

/* LoadBtn */
void TapMap::OnBnClickedButton2()
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

		if (FAILED(LoadTerrainLayer(hFile)))
			return;

		if (FAILED(LoadObjectLayer(hFile)))
			return;

		CloseHandle(hFile);
	}
}
