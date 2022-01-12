// TapMap.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapMap.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "Form.h"
#include "Layer.h"
#include "ToolObject.h"

// TapMap ��ȭ �����Դϴ�.

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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	delete this;
	CDialogEx::PostNcDestroy();
}


BOOL TapMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialogEx::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.


	
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
