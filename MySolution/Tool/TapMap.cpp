// TapMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "TapMap.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "Form.h"
#include "Layer.h"
#include "Cell.h"
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
	DDX_Control(pDX, IDC_CHECK4, m_Navigation);
}


BEGIN_MESSAGE_MAP(TapMap, CDialogEx)
	ON_WM_CREATE()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO1, IDC_RADIO3, &TapMap::OnBnClieckRadio)
	ON_EN_CHANGE(IDC_EDIT2, &TapMap::OnEnChangeRadius)
	ON_BN_CLICKED(IDC_CHECK2, &TapMap::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &TapMap::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_BUTTON1, &TapMap::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &TapMap::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_CHECK4, &TapMap::OnBnClickedCheck4)
	ON_LBN_SELCHANGE(IDC_LIST1, &TapMap::OnLbnSelchangeList1)
END_MESSAGE_MAP()


HRESULT TapMap::SaveNavigation(HANDLE & hFile)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	vector<Cell*> pNavigationCells = static_cast<Navigation*>(pGameInstance->GetObjectList(1, TEXT("Terrain")).front()->GetComponent(TEXT("Com_Navigation")))->GetCells();
	_int size = pNavigationCells.size();
	_ulong		dwByte;

	WriteFile(hFile, &size, sizeof(_int), &dwByte, nullptr);
	for (auto& Cell : pNavigationCells)
	{
		_float3 Points;
		XMStoreFloat3(&Points , Cell->GetPoint(Cell::POINT::POINT_1st));
		WriteFile(hFile, &Points, sizeof(_float3), &dwByte, nullptr);

		XMStoreFloat3(&Points, Cell->GetPoint(Cell::POINT::POINT_2nd));
		WriteFile(hFile, &Points, sizeof(_float3), &dwByte, nullptr);

		XMStoreFloat3(&Points, Cell->GetPoint(Cell::POINT::POINT_3rd));
		WriteFile(hFile, &Points, sizeof(_float3), &dwByte, nullptr);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT TapMap::SaveTerrainLayer(HANDLE& hFile)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> pGameObjectList = pGameInstance->GetObjectList(1, TEXT("Terrain"));

	ToolTerrain* pTerrain = static_cast<ToolTerrain*>(pGameObjectList.front());
	DWORD dwByte;

	_int x = pTerrain->GetX();
	_int z = pTerrain->GetZ();
	void* pVertices = pTerrain->GetVertices();

	_int count = 0;

	for (_int i = 0; i <z ; i++)
	{
		for (_int j = 0; j < x; j++)
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


	for (_int i = 0; i <z; i++)
	{
		for (_int j = 0; j < x; j++)
		{
			_int		iIndex = i * x + j;

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
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	DWORD dwByte = 0;
	_int  size = 0;

	while (TRUE)
	{
		_matrix pTransformMatrix = XMMatrixIdentity();
		CGameObject::TOOLOBJDESC pToolObjDesc;
		
		ZeroMemory(&pToolObjDesc, sizeof(pToolObjDesc));
		ReadFile(hFile, &pToolObjDesc, sizeof(CGameObject::TOOLOBJDESC), &dwByte, nullptr);
		pToolObjDesc.loadCheck = TRUE;

		if (0 == dwByte)
			break;

		pGameInstance->Add_GameObjectToLayer(1, TEXT("Object"), pToolObjDesc.m_ObjTag, &pToolObjDesc);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT TapMap::LoadNavigation(HANDLE & hFile)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	DWORD dwByte = 0;
	_int size = 0;
	ReadFile(hFile, &size, sizeof(_int), &dwByte, nullptr);

	for (_int i = 0; i < size; i++)
	{
		_float3 Points[3];
		_float3 Point;
		ReadFile(hFile,  &Point, sizeof(_float3), &dwByte, nullptr);
		Points[0] = Point;

		ReadFile(hFile, &Point, sizeof(_float3), &dwByte, nullptr);
		Points[1] = Point;

		ReadFile(hFile, &Point, sizeof(_float3), &dwByte, nullptr);
		Points[2] = Point;

		static_cast<Navigation*>(pGameInstance->GetObjectList(1, TEXT("Terrain")).front()->GetComponent(TEXT("Com_Navigation")))->AddCell(Points, TEXT("../Client/Bin/ShaderFiles/Shader_TriangleToLine.hlsl"));
	}
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT TapMap::SaveObjectLayer(HANDLE& hFile)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> pGameObjectList = pGameInstance->GetObjectList(1, TEXT("Object"));

	DWORD dwByte;
	for (auto iter : pGameObjectList)
	{
		_matrix pTransformMatrix = static_cast<ToolObject*>(iter)->GetTransformMatrix();
		CGameObject::TOOLOBJDESC pToolObjDesc = static_cast<ToolObject*>(iter)->GetObjectDesc();
		XMStoreFloat4x4(&pToolObjDesc.m_pTransformMatrix , pTransformMatrix);

		WriteFile(hFile, &pToolObjDesc, sizeof(CGameObject::TOOLOBJDESC), &dwByte, nullptr);
/*
		WriteFile(hFile, &pTransformMatrix, sizeof(_matrix), &dwByte, nullptr);

		_int size = wcslen(pToolObjDesc.m_BufferTag);
		WriteFile(hFile, &size, sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &pToolObjDesc.m_BufferTag, size * sizeof(_tchar), &dwByte, nullptr);
		
		size = wcslen(pToolObjDesc.m_ObjTag);
		WriteFile(hFile, &size, sizeof(_int), &dwByte, nullptr);
		WriteFile(hFile, &pToolObjDesc.m_ObjTag, size * sizeof(_tchar), &dwByte, nullptr);*/
	}


	RELEASE_INSTANCE(CGameInstance);

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

#ifdef _DEBUG
		cout << "Save Terrain....." << endl;
#endif // _DEBUG
		if (FAILED(SaveTerrainLayer(hFile)))
			return;
		
#ifdef _DEBUG
		cout << "Save Navigation....." << endl;
#endif // _DEBUG
		if (FAILED(SaveNavigation(hFile)))
			return;

#ifdef _DEBUG
		cout << "Save Object....." << endl;
#endif // _DEBUG
		if (FAILED(SaveObjectLayer(hFile)))
			return;
		

#ifdef _DEBUG
		cout << "Complited!!!" << endl;
#endif // _DEBUG
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
		m_Navigation.SetCheck(FALSE);
	}
	Invalidate(FALSE);
}


void TapMap::OnBnClickedCheck3()
{
	if (m_Modify.GetCheck()) {
		m_BatchObject.SetCheck(FALSE);
		m_Navigation.SetCheck(FALSE);
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

#ifdef _DEBUG
		cout << "Load Terrain....." << endl;
#endif // _DEBUG
		if (FAILED(LoadTerrainLayer(hFile)))
			return;

#ifdef _DEBUG
		cout << "Load Navigation....." << endl;
#endif // _DEBUG
		if (FAILED(LoadNavigation(hFile)))
			return;

#ifdef _DEBUG
		cout << "Load Object....." << endl;
#endif // _DEBUG
		if (FAILED(LoadObjectLayer(hFile)))
			return;

#ifdef _DEBUG
		cout << "Complited!!!" << endl;
#endif // _DEBUG
		CloseHandle(hFile);
	}
	
}


void TapMap::OnBnClickedCheck4()
{
	if (m_Navigation.GetCheck()) {
		m_BatchObject.SetCheck(FALSE);
		m_Modify.SetCheck(FALSE);
	}
	Invalidate(FALSE);
}


void TapMap::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
