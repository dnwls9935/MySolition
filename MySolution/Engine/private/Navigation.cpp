#include "..\public\Navigation.h"

#include "Cell.h"

Navigation::Navigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

Navigation::Navigation(const Navigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
{
	for (auto& cell : m_Cells)
		Safe_AddRef(cell);
}

HRESULT Navigation::NativeConstruct_Prototype(const _tchar* _NavigationFilePath)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	if (nullptr != _NavigationFilePath)
	{
		if (FAILED(LoadNavigationCells(_NavigationFilePath)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT Navigation::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool Navigation::MoveOnNavigation(_fvector _Position)
{
	Cell* pNeighbor = nullptr;

	if (TRUE == m_Cells[m_CurrentCellIndex]->IsIn(_Position, &pNeighbor))
	{
		return TRUE;
	}
	else {

		if (nullptr != pNeighbor)
		{

			while (TRUE) 
			{

				if (TRUE == m_Cells[m_CurrentCellIndex]->IsIn(_Position, &pNeighbor))
				{
					m_CurrentCellIndex = pNeighbor->GetIndex();
					return TRUE;
				}

			}

		}

		else
			return FALSE;
	}
}

void Navigation::AddCell(_float3 * _Points)
{
	Cell* cell = Cell::Create(m_pDevice, m_pDeviceContext, _Points, m_Cells.size());
	m_Cells.push_back(cell);
}

#ifdef _DEBUG

HRESULT Navigation::Render(_fmatrix _WorldMatrix)
{
	for (auto& Cell : m_Cells)
		Cell->Render(_WorldMatrix, m_CurrentCellIndex);

	return S_OK;
}
#endif // _DEBUG


HRESULT Navigation::LoadNavigationCells(const _tchar * _NavigationFilePath)
{
	HANDLE hFile = CreateFile(_NavigationFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (0 == hFile)
		return E_FAIL;

	_ulong dwByte = 0;

	while (TRUE)
	{
		_float3 pPoints[3];

		ReadFile(hFile, pPoints, sizeof(_float3) * 3, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		Cell* cell = Cell::Create(m_pDevice, m_pDeviceContext, pPoints, m_Cells.size());
		if (nullptr == cell)
			return E_FAIL;

		m_Cells.push_back(cell);
	}
	CloseHandle(hFile);

	if (FAILED(SetUpNeighbor()))
		return E_FAIL;

	return S_OK;
}

HRESULT Navigation::SetUpNeighbor()
{
	for (auto& SourCell : m_Cells)
	{
		for (auto& DestCell : m_Cells)
		{
			if (SourCell == DestCell)
				continue;

			if (TRUE == DestCell->Compare(SourCell->GetPoint(Cell::POINT::POINT_1st), SourCell->GetPoint(Cell::POINT::POINT_2nd)))
			{
				SourCell->SetNeighbor(Cell::LINE::LINE_12, DestCell);
			}

			if (TRUE == DestCell->Compare(SourCell->GetPoint(Cell::POINT::POINT_2nd), SourCell->GetPoint(Cell::POINT::POINT_3rd)))
			{
				SourCell->SetNeighbor(Cell::LINE::LINE_23, DestCell);
			}

			if (TRUE == DestCell->Compare(SourCell->GetPoint(Cell::POINT::POINT_3rd), SourCell->GetPoint(Cell::POINT::POINT_1st)))
			{
				SourCell->SetNeighbor(Cell::LINE::LINE_31, DestCell);
			}
		}
	}
	return S_OK;
}

Navigation * Navigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext,const _tchar* _NavigationFilePath)
{
	Navigation*		pInstance = new Navigation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(_NavigationFilePath)))
	{
		MSGBOX("Failed to Creating Navigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * Navigation::Clone(void * pArg)
{
	Navigation*		pInstance = new Navigation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning Navigation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Navigation::Free()
{
	__super::Free();

	for (auto& Cell : m_Cells)
		Safe_Release(Cell);

	m_Cells.clear();
}
