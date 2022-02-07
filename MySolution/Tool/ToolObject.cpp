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

	_matrix Transform = XMLoadFloat4x4(&m_ToolObjDesc.m_pTransformMatrix);

	if (m_ToolObjDesc.loadCheck)
	{
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, Transform.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, Transform.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, Transform.r[2]);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Transform.r[3]);
	}
	else {
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_ToolObjDesc.m_Position.x, m_ToolObjDesc.m_Position.y, m_ToolObjDesc.m_Position.z, 1.f));
	}
	m_Type = m_ToolObjDesc.m_Type;

	return S_OK;
}

_int ToolObject::Tick(_double TimeDelta)
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form * m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
	if (0 != m_form->m_tabCtrl.GetCurSel())
		return _int();
	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	if (m_form->tapMap->m_Modify.GetCheck())
	{
		if (gameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80 && 
			m_PickChecking)
		{
			KeyCheck(TimeDelta);
		}

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

				if (gameInstance->Get_DIKeyState(DIK_LALT) & 0x80)
				{
					if (MouseMove = gameInstance->Get_MouseMoveState(CInput_Device::MMS_Y))
					{
						_float4 position;
						XMStoreFloat4(&position, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

						if (0 < MouseMove)
							TimeDelta *= -1.f;

						m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(position.x, position.y + ((_float)TimeDelta * 15), position.z, position.w));
					}
				}
			}
		}
		_long mouseCheck = 0;
		if ((gameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80) &&
			(mouseCheck = gameInstance->Get_MouseMoveState(CInput_Device::MMS_WHEEL)) &&
			m_PickChecking)
		{
			_float Right = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
			_float Up = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
			_float Look = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);

			if (0 > mouseCheck)
				TimeDelta *= -1.f;

			m_pTransformCom->Set_Scale(XMVectorSet((_float)(Right + TimeDelta), (_float)(Up + TimeDelta), (_float)(Look + TimeDelta), 0.f));
		}

		if (gameInstance->Get_DIKeyState(DIK_DELETE) & 0x80 &&
			m_PickChecking)
		{
			m_Dead = TRUE;
		}
	}

	if (gameInstance->Get_DIKeyState(DIK_ESCAPE) & 0x80)
	{
		m_PickChecking = FALSE;
	}
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int ToolObject::LateTick(_double TimeDelta)
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form * m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
	if (0 != m_form->m_tabCtrl.GetCurSel())
		return _int();

	if (nullptr != m_pRendererCom && 0 == m_form->m_tabCtrl.GetCurSel())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if(CModel::TYPE::TYPE_ANIM == m_pModelCom->GetMeshType())
		m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);

	return m_Dead;
}

HRESULT ToolObject::Render()
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form * m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
	if (0 != m_form->m_tabCtrl.GetCurSel())
		return _int();
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

		m_pModelCom->Render(i, m_pModelCom->GetMeshType());
	}

	//if (m_PickChecking)
		//RenderThreeWaySystem();

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void ToolObject::KeyCheck(_double TimeDelta)
{
	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);
	if (gameInstance->Get_DIKeyState(DIK_W) & 0x80)
		m_pTransformCom->Go_Straight(TimeDelta * 2.f);
	if (gameInstance->Get_DIKeyState(DIK_S) & 0x80)
		m_pTransformCom->Go_BackWard(TimeDelta * 2.f);
	if (gameInstance->Get_DIKeyState(DIK_D) & 0x80)
		m_pTransformCom->Go_Right(TimeDelta * 2.f);
	if (gameInstance->Get_DIKeyState(DIK_A) & 0x80)
		m_pTransformCom->Go_Left(TimeDelta * 2.f);

	RELEASE_INSTANCE(CGameInstance);
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
