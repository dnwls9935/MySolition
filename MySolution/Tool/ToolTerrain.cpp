#include "stdafx.h"
#include "ToolTerrain.h"

#include "GameInstance.h"
#include "ToolDefines.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Form.h"


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

	return S_OK;
}

_int ToolTerrain::Tick(_double TimeDelta)
{
	if (FAILED(__super::Tick(TimeDelta)))
		return -1;

	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	if (gameInstance->Get_MouseButtonState(CInput_Device::MBS_LBUTTON))
	{
		PickUpOnTerrain();
	}

	RELEASE_INSTANCE(CGameInstance);

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

	m_pVIBufferCom->Render(0);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT ToolTerrain::ResetTerrainInfo(const _tchar * _shaderFile, const _tchar * _heightMapFile, _uint _x, _uint _z)
{
	//Safe_Release(m_pVIBufferCom);

	//CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(gameInstance->Add_Prototype(0, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, _shaderFile, _x, _z, _heightMapFile))))
	//	return E_FAIL;

	//if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
	//	return E_FAIL;

	//RELEASE_INSTANCE(CGameInstance);
	//return S_OK;
}

_float3 ToolTerrain::PickUpOnTerrain()
{
	/*CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	CToolView* m_form = dynamic_cast<CToolView*>(m_mainFrm->m_MainSpliiter.GetPane(0, 1));
*/
	POINT	pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	// 마우스 포스 가져옴(window 기준)
	_float3			mousePos;
	mousePos.x = pt.x / (g_WIN_WIDHT * 0.5f)- 1.f;
	mousePos.y = pt.y / -(g_WIN_HEIGHT * 0.5f) + 1.f;
	mousePos.z = 0.f;

	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	// 투영에서 뷰 스페이스로 다운
	_fmatrix projMat = gameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION);
	_matrix projMatInv = XMMatrixInverse(nullptr, projMat);

	_vector viewMousePos = XMVector3TransformCoord(XMLoadFloat3(&mousePos), projMatInv);

	// 뷰에서 월드로 다운
	_vector RayPos, RayDir;
	RayPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	RayDir = viewMousePos - RayPos;

	_fmatrix viewMat = gameInstance->Get_Transform(CPipeLine::D3DTS_VIEW);
	_matrix viewMatInv = XMMatrixInverse(nullptr, viewMat);

	RayPos = XMVector3TransformCoord(RayPos, viewMatInv);
	RayDir = XMVector3TransformNormal(RayDir, viewMatInv);
	
	// 월드에서 뷰로 다운
	_fmatrix worldMatInv = m_pTransformCom->Get_WorldMatrixInverse();
	//
	RayPos = XMVector3TransformCoord(RayPos, worldMatInv);
	RayDir = XMVector3TransformNormal(RayDir, worldMatInv);
	RayDir = XMVector3Normalize(RayDir);

	RELEASE_INSTANCE(CGameInstance);

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

			if (TriangleTests::Intersects(RayPos, RayDir,
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[0]].vPosition),
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[1]].vPosition),
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[2]].vPosition),
				dist)
				)
			{
				cout << dwIdx << endl;
				return _float3();
			}

			dwVtxIdx[0] = dwIdx + dwCntX;
			dwVtxIdx[1] = dwIdx + 1;
			dwVtxIdx[2] = dwIdx;

			if (TriangleTests::Intersects(RayPos, RayDir,
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[0]].vPosition),
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[1]].vPosition),
				XMLoadFloat3(&terrainVtxPos[dwVtxIdx[2]].vPosition),
				dist)
				)
			{
				cout << dwIdx << endl;
				return _float3();
			}

		}
	}

	return _float3();
}

HRESULT ToolTerrain::SetUp_Components()
{/* Com_Transform */
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

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
