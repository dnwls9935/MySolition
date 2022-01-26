#include "stdafx.h"
#include "ToolTerrain.h"

#include "GameInstance.h"
#include "ToolDefines.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Form.h"
#include "TapMap.h"
#include "ToolObject.h"
#include "NaviPoint.h"
#include "Cell.h"

ToolTerrain::ToolTerrain(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: CGameObject(_dx11Device, _dx11DeviceContext)
{
}

ToolTerrain::ToolTerrain(const ToolTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT ToolTerrain::NativeConstruct_Prototype()
{
	if(FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT ToolTerrain::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));

	m_Type = OBJTYPE_ID::TERRAIN;

	return S_OK;
}

_int ToolTerrain::Tick(_double TimeDelta)
{
	if (FAILED(__super::Tick(TimeDelta)))
		return -1;


	if (m_form->tapMap->m_BatchObject.GetCheck())
	{
		BatchingObject(TimeDelta);
	}
	else if (m_form->tapMap->m_Navigation.GetCheck())
	{
		Navigationing(TimeDelta);
	}
	else {
		m_mouseBrushType = m_form->tapMap->m_radioValue;
		if (0 != m_mouseBrushType)
			PickHeightTerrain(TimeDelta);
	}

	m_RenderID = (RENDER_ID)m_form->tapMap->m_CullMode.GetCheck();
	
	for (auto& pNaviPoint : m_NaviPoints)
		pNaviPoint->Update();

	return _int();
}

_int ToolTerrain::LateTick(_double TimeDelta)
{
	if (FAILED(__super::LateTick(TimeDelta)))
		return -1;

	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form* m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));

	if (nullptr != m_pRendererCom && 0 == m_form->m_tabCtrl.GetCurSel())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	for (auto& pNaviPoint : m_NaviPoints)
		pNaviPoint->LateUpdate();

	m_MouseLButton = TRUE;

	return _int();
}

HRESULT ToolTerrain::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseSourTexture", m_pTextureCom, 0);
	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseDestTexture", m_pTextureCom, 0);

	m_pVIBufferCom->SetUp_ValueOnShader("g_vCamPosition", (void*)&pGameInstance->Get_CamPosition(), sizeof(_vector));

	m_pVIBufferCom->SetUp_ValueOnShader("g_vMousePosition", &m_mousePos, sizeof(_vector));
	m_pVIBufferCom->SetUp_ValueOnShader("g_vMouseBrushType", &m_mouseBrushType, sizeof(_int));
	m_pVIBufferCom->SetUp_ValueOnShader("g_vMouseBrushRadius", &m_mouseBrushRadius, sizeof(_int));

	m_pVIBufferCom->Render((_uint)m_RenderID);

	RELEASE_INSTANCE(CGameInstance);

	for (auto& pNaviPoint : m_NaviPoints)
		pNaviPoint->Render();

	m_NavigationCom->Render();
	return S_OK;
}

void * ToolTerrain::GetVertices()
{
	return m_pVIBufferCom->GetVertices();
}

_uint ToolTerrain::GetX()
{
	return m_pVIBufferCom->GetVerticesX();
}

_uint ToolTerrain::GetZ()
{
	return m_pVIBufferCom->GetVerticesZ();
}
HRESULT ToolTerrain::SetVertexY(_int _index, _float _y)
{
	m_pVIBufferCom->SetVerticeY(_index, _y);

	return S_OK;
}

void ToolTerrain::SetNormalVector()
{
	m_pVIBufferCom->SetNormalVector();
}

void ToolTerrain::PickHeightTerrain(_double TimeDelta)
{
	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	m_mouseBrushRadius = m_form->tapMap->m_brushRadius;

	XMStoreFloat4(&m_mousePos, CalcMousePos());
	if ((gameInstance->Get_MouseButtonState(CInput_Device::MBS_LBUTTON)) &&
		(gameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80))
	{
		m_pVIBufferCom->SetVerticeY(m_mousePos, -5.f * (_float)TimeDelta, m_mouseBrushRadius, m_mouseBrushType);
	}
	else if (gameInstance->Get_MouseButtonState(CInput_Device::MBS_LBUTTON)) {
		m_pVIBufferCom->SetVerticeY(m_mousePos, 5.f * (_float)TimeDelta, m_mouseBrushRadius, m_mouseBrushType);
	}
	RELEASE_INSTANCE(CGameInstance);
}

void ToolTerrain::BatchingObject(_double _timeDelta)
{
	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);
	_uint count = m_form->tapMap->m_objectListBox.GetCount();
	_uint focus = m_form->tapMap->m_objectListBox.GetCurSel();

	if (gameInstance->Get_MouseButtonState(CInput_Device::MBS_LBUTTON) &&
		count >= focus)
	{
		OBJTYPE_ID enumValue = (OBJTYPE_ID)m_form->tapMap->m_BatchObjectEnumArr[focus];
		_vector calcVec = CalcMousePos();
		_bool b = XMVector4Equal(_vector{ 0.f,0.f,0.f,0.f }, calcVec);
		if (b)
			return;

		XMStoreFloat4(&m_mousePos, calcVec);

		CString str = TEXT("");
		m_form->tapMap->m_objectListBox.GetText(focus, str);

		ToolObject::TOOLOBJDESC objDesc;

		CString componentText = TEXT("Prototype_Component_Model_");
		componentText += str;
		lstrcpy(objDesc.m_BufferTag, componentText);

		CString gameObjectText = TEXT("Prototype_GameObject_Model_");
		gameObjectText += str;
		lstrcpy(objDesc.m_ObjTag, gameObjectText);

		objDesc.m_Position = { m_mousePos.x, m_mousePos.y, m_mousePos.z };

		objDesc.m_Type = enumValue;

		gameInstance->Add_GameObjectToLayer(1, TEXT("Object"), objDesc.m_ObjTag, &objDesc);

		m_form->tapMap->m_BatchObject.SetCheck(FALSE);
	}

	RELEASE_INSTANCE(CGameInstance);
}

_fvector ToolTerrain::CalcMousePos()
{
	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);
	Calculator::CALCDESC calDesc;
	ZeroMemory(&calDesc, sizeof(calDesc));
	calDesc._hWnd = g_hWnd;
	calDesc._width = g_WIN_WIDHT;
	calDesc._height = g_WIN_HEIGHT;
	calDesc._transformCom = m_pTransformCom;
	calDesc._rayPos = { 0.f, 0.f, 0.f, 0.f };
	calDesc._rayDir = { 0.f, 0.f, 0.f, 0.f };

	gameInstance->CalcMousePos(&calDesc);

	_vector pPos = calDesc._rayPos;
	_vector pDir = calDesc._rayDir;
/*
	_vector pPos = RayPos;
	_vector pDir = RayDir;*/

	_ulong		dwCntX = m_pVIBufferCom->GetVerticesX();
	_ulong		dwCntZ = m_pVIBufferCom->GetVerticesZ();
	VTXNORTEX*	terrainVtxPos = (VTXNORTEX*)m_pVIBufferCom->GetVertices();

	_ulong	dwVtxIdx[3];
	_float	dist;

	for (_ulong i = 0; i < dwCntZ - 1; i++)
	{
		for (_ulong j = 0; j < dwCntX - 1; j++)
		{
			_ulong	dwIdx = i * dwCntX + j;

			dwVtxIdx[0] = dwIdx + dwCntX;
			dwVtxIdx[1] = dwIdx + dwCntX + 1;
			dwVtxIdx[2] = dwIdx + 1;

			if (TriangleTests::Intersects(pPos, pDir,
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[0]].vPosition),
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[1]].vPosition),
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[2]].vPosition),
				dist)
				)
			{
				_vector revec = pPos + pDir* dist;
				return revec;
			}

			dwVtxIdx[0] = dwIdx + dwCntX;
			dwVtxIdx[1] = dwIdx + 1;
			dwVtxIdx[2] = dwIdx;

			if (TriangleTests::Intersects(pPos, pDir,
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[0]].vPosition),
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[1]].vPosition),
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[2]].vPosition),
				dist)
				)
			{
				_vector revec = pPos + pDir* dist;
				return revec;
			}

		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return XMVectorSet(0.f, 0.f, 0.f, 0.f);
}

void ToolTerrain::Navigationing(_double TimeDelta)
{
	cout << m_NaviPointArr[0] << " / " << m_NaviPointArr[1] << " / " << m_NaviPointArr[2] << " / " << m_NavigationCom->CellsSize() << " / " << m_NavigationCom->GetCurrentCellIndex() << endl;
	NavigationKeyChecking(TimeDelta);
}

void ToolTerrain::NavigationKeyChecking(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_ESCAPE) & 0x80)
	{
		for (_int i = 0; i < 3; i++)
			m_NaviPointArr[i] = nullptr;

		m_NavigationCom->SetCurrentCellIndex();
	}

	if (pGameInstance->Get_DIKeyState(DIK_DELETE) & 0x80)
	{
		m_NavigationCom->DeleteCell(m_NavigationCom->GetCurrentCellIndex());
		m_form->tapMap->m_Navigation.SetCheck(FALSE);
	}


	_bool LeftControlCheck = (pGameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80);

	if (FALSE == LeftControlCheck)
	{
		if (pGameInstance->Get_MouseButtonState(CInput_Device::MOUSEBUTTONSTATE::MBS_LBUTTON))
		{
			_vector pCalculatingMousePosition = CalcMousePos();

			if (XMVector4Equal(XMVectorSet(0.f, 0.f, 0.f, 0.f), pCalculatingMousePosition))
				return;

			NaviPoint* pNaviPoint = NaviPoint::Create(m_pDevice, m_pDeviceContext, (_int)m_NaviPoints.size(), pCalculatingMousePosition);
			if (nullptr == pNaviPoint)
				return;
			m_NaviPoints.push_back(pNaviPoint);
			m_form->tapMap->m_Navigation.SetCheck(FALSE);
		}
	}
	else {
		SettingNavigation();
	}
	RELEASE_INSTANCE(CGameInstance);
}

void ToolTerrain::SettingNavigation()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	Calculator::CALCDESC CalDesc;
	CalDesc._height = g_WIN_HEIGHT;
	CalDesc._width = g_WIN_WIDHT;
	CalDesc._hWnd = g_hWnd;
	CalDesc._transformCom = m_pTransformCom;

	pGameInstance->CalcMousePos(&CalDesc);
	_vector Ray = CalDesc._rayPos;
	_vector Dir = CalDesc._rayDir;

	Ray = XMVector3TransformCoord(Ray, m_pTransformCom->Get_WorldMatrix());
	Dir = XMVector3TransformNormal(Dir, m_pTransformCom->Get_WorldMatrix());
	Dir = XMVector3Normalize(Dir);

	if (pGameInstance->Get_MouseButtonState(CInput_Device::MOUSEBUTTONSTATE::MBS_LBUTTON))
	{
		NaviPoint* NaviPoint = nullptr;
		for (auto& pNaviPoint : m_NaviPoints)
			pNaviPoint->CollisionSphere(Ray, Dir, &NaviPoint);

		if (nullptr == m_NaviPointArr[0])
		{
			if(FALSE == CheckNaviPoint(NaviPoint))
				m_NaviPointArr[0] = NaviPoint;
		}
		else if (nullptr == m_NaviPointArr[1])
		{
			if (FALSE == CheckNaviPoint(NaviPoint))
				m_NaviPointArr[1] = NaviPoint;
		}
		else if (nullptr == m_NaviPointArr[2])
		{
			if (FALSE == CheckNaviPoint(NaviPoint))
			{
				m_NaviPointArr[2] = NaviPoint;
				_float3		CellPosition[3];
				CellPosition[0] = m_NaviPointArr[0]->GetPosition();
				CellPosition[1] = m_NaviPointArr[1]->GetPosition();
				CellPosition[2] = m_NaviPointArr[2]->GetPosition();
				m_NavigationCom->AddCell(CellPosition, TEXT("../Client/Bin/ShaderFiles/Shader_TriangleToLine.hlsl"));

				for (_int i = 0; i < 3; i++)
					m_NaviPointArr[i] = nullptr;

				m_form->tapMap->m_Navigation.SetCheck(FALSE);
			}
		}
	}
	else if (pGameInstance->Get_MouseButtonState(CInput_Device::MBS_RBUTTON))
	{
		m_NavigationCom->CollisionRayToCell(Ray, Dir);
	}
	RELEASE_INSTANCE(CGameInstance);
}

_bool ToolTerrain::CheckNaviPoint(NaviPoint * NaviPoint)
{
	for (_int i = 0; i < 3; i++)
	{
		if (m_NaviPointArr[i] == NaviPoint)
			return TRUE;
	}
	return FALSE;
}

HRESULT ToolTerrain::SetUp_Components()
{
	/* Com_Transform */
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Navigation */
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_NavigationCom)))
		return E_FAIL;

	return S_OK;
}

ToolTerrain * ToolTerrain::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
{
	ToolTerrain* pInstance = new ToolTerrain(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX(L"Failed to Create ToolTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * ToolTerrain::Clone(void * pArg)
{
	ToolTerrain* pInstance = new ToolTerrain(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(L"Failed to Clone ToolTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void ToolTerrain::Free()
{
	__super::Free();

	for (auto& pNaviPoint : m_NaviPoints)
		Safe_Release(pNaviPoint);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
