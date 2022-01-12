#include "stdafx.h"
#include "ToolObject.h"

#include "GameInstance.h"
#include "ToolDefines.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Form.h"
#include "TapMap.h"

ToolObject::ToolObject(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: CGameObject(_dx11Device, _dx11DeviceContext)
{
}

ToolObject::ToolObject(const ToolObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT ToolObject::NativeConstruct_Prototype()
{
	if(FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT ToolObject::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_ToolObjDesc, pArg, (sizeof(TOOLOBJDESC)));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_ToolObjDesc.m_Position.x, m_ToolObjDesc.m_Position.y, m_ToolObjDesc.m_Position.z, 1.f));

	m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));

	return S_OK;
}

_int ToolObject::Tick(_double TimeDelta)
{
	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form* m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));

	if (m_form->tapMap->m_Modify.GetCheck())
	{
		if (gameInstance->Get_MouseButtonState(CInput_Device::MBS_LBUTTON))
		{
			if (!m_PickChecking)
			{
				m_PickChecking = PickingObject();
			}
			else {
				_long MouseMove = 0;
				if (MouseMove = gameInstance->Get_MouseMoveState(CInput_Device::MMS_X))
				{
					m_pTransformCom->Rotation_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * -MouseMove * m_mouseSenitive);
				}

			}
		}

		// �̰� �� �ȵǳ� ���޳�~
		/*_long mouseCheck = 0;
		if ((gameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80) &&
			(mouseCheck = gameInstance->Get_MouseMoveState(CInput_Device::MMS_WHEEL)))
		{
			m_pTransformCom->Set_Scale(mouseCheck * 0.05f);
		}*/
	}

	if (gameInstance->Get_DIKeyState(DIK_ESCAPE) & 0x80)
	{
		m_PickChecking = FALSE;
	}

	if (gameInstance->Get_DIKeyState(DIK_1) & 0x80)
	{
		m_pModelCom->SetUp_AnimationIndex(0);
	}
	else 	if (gameInstance->Get_DIKeyState(DIK_2) & 0x80)
	{
		m_pModelCom->SetUp_AnimationIndex(1);
	}
	else 	if (gameInstance->Get_DIKeyState(DIK_3) & 0x80)
	{
		m_pModelCom->SetUp_AnimationIndex(2);
	}
	else 	if (gameInstance->Get_DIKeyState(DIK_4) & 0x80)
	{
		m_pModelCom->SetUp_AnimationIndex(3);
	}
	else 	if (gameInstance->Get_DIKeyState(DIK_5) & 0x80)
	{
		m_pModelCom->SetUp_AnimationIndex(4);
	}

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int ToolObject::LateTick(_double TimeDelta)
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form* m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));

	if (nullptr != m_pRendererCom && 0 == m_form->m_tabCtrl.GetCurSel())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);

	return _int();
}

HRESULT ToolObject::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_PickModel", &m_PickChecking, sizeof(_bool));

	if (FAILED(m_pModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshContainer(); ++i)
	{
		m_pModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Render(i, 1);
	}

	//if (m_PickChecking)
		//RenderThreeWaySystem();

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void ToolObject::CheckButton()
{
	m_PickChecking = TRUE;
}

_bool ToolObject::PickingObject()
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

	VTXMESH*	modelVtxPos = (VTXMESH*)m_pModelCom->GetVertices();
	_uint		modelVtxCount = m_pModelCom->GetNumVertices();
	_float		dist;

	for (_uint i = 0; i < modelVtxCount; i++)
	{
		_vector vec1 = XMLoadFloat3(&modelVtxPos[i].vPosition);
		i++;
		_vector vec2 = XMLoadFloat3(&modelVtxPos[i].vPosition);
		i++;
		_vector vec3 = XMLoadFloat3(&modelVtxPos[i].vPosition);

		if (TriangleTests::Intersects(pPos, pDir, vec1, vec2, vec3, dist))
		{
			return TRUE;
		}
	}
	RELEASE_INSTANCE(CGameInstance);
	return FALSE;
}

void ToolObject::RenderThreeWaySystem()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pThreeWaySystem->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pThreeWaySystem->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pThreeWaySystem->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pThreeWaySystem->Render(0);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT ToolObject::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(0, m_ToolObjDesc.m_BufferTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_ThreeWaySystem */
	/*if (FAILED(__super::SetUp_Components(0, TEXT("ProtoType_Component_VIBuffer_Line"), TEXT("Com_ThreeWaySystem"), (CComponent**)&m_pThreeWaySystem)))
		return E_FAIL;*/


	return S_OK;
}

ToolObject * ToolObject::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
{
	ToolObject* pInstance = new ToolObject(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX(L"Failed to Create ToolObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * ToolObject::Clone(void * pArg)
{
	ToolObject* pInstance = new ToolObject(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(L"Failed to Clone ToolObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void ToolObject::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
}