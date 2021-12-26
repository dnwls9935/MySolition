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

	return S_OK;
}

_int ToolObject::Tick(_double TimeDelta)
{
	CGameInstance* gameInstance = GET_INSTANCE(CGameInstance);

	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form* m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
/*
	_vector vec = { 0.f, 0.f, 0.f, 0.f };
	XMStoreFloat3(&m_ToolObjDesc.m_Position, vec);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,vec);
	???????????????????????????????????
*/
	
	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int ToolObject::LateTick(_double TimeDelta)
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form* m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));

	if (nullptr != m_pRendererCom && 0 == m_form->m_tabCtrl.GetCurSel())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);


	return _int();
}

HRESULT ToolObject::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_pModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshContainer(); ++i)
	{
		m_pModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Render(i, 0);
	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
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
