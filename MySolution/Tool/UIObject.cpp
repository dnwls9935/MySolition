#include "stdafx.h"
#include "UIObject.h"

#include "GameInstance.h"
#include "ToolDefines.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "Form.h"
#include "TapUI.h"

UIObject::UIObject(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
	: CGameObject(_dx11Device, _dx11DeviceContext)
{
}

UIObject::UIObject(const UIObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT UIObject::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT UIObject::NativeConstruct(void * pArg)
{
	m_ProjMatrix = XMMatrixOrthographicLH((_float)1280, (_float)720, 0.f, 1.f);

	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_UIOBJDesc, pArg, (sizeof(UIOBJDESC)));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FALSE == m_UIOBJDesc.m_LoadCheck)
	{
		_float X = 0.f, Y = 0.f;
		_float2 TextureSize = m_pTextureCom->GetTextureInfo(0);

		X = TextureSize.x/* / 2.f * 4.f*/;
		Y = TextureSize.y/* / 2.f * 4.f*/;

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(X, 0.f, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, Y, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_UIOBJDesc.m_Position);
	}
	else {
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, m_UIOBJDesc.m_LoadMatrix.r[0]);
		m_pTransformCom->Set_State(CTransform::STATE_UP, m_UIOBJDesc.m_LoadMatrix.r[1]);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, m_UIOBJDesc.m_LoadMatrix.r[2]);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_UIOBJDesc.m_LoadMatrix.r[3]);
	}
	return S_OK;
}

_int UIObject::Tick(_double TimeDelta)
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form * m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
	if (2 != m_form->m_tabCtrl.GetCurSel())
		return _int();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);


	if (FALSE == m_PickCheck)
		return _int();

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Get_DIKeyState(DIK_DELETE) & 0x80)
	{
		m_Dead = TRUE;
	}

	_long mouseMove = 0;
	_bool LControl = pGameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80;
	if (TRUE == LControl &&
		TRUE == m_PickCheck)
	{
		if (pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
		{
			_float Right = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
			_float Up = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
			_float Look = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
			if (0 > mouseMove)
				TimeDelta *= -1.f;

			m_pTransformCom->Set_Scale(XMVectorSet((_float)(Right + -TimeDelta * 30.f), (_float)Up, (_float)(Look + TimeDelta * 30.f), 0.f));
		}
		else if (pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
		{
			_float Right = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
			_float Up = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
			_float Look = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
			if (0 > mouseMove)
				TimeDelta *= -1.f;

			m_pTransformCom->Set_Scale(XMVectorSet((_float)(Right + TimeDelta * 30.f), (_float)Up, (_float)(Look + TimeDelta * 30.f), 0.f));
		}
		else if (pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
		{
			_float Right = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
			_float Up = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
			_float Look = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
			if (0 > mouseMove)
				TimeDelta *= -1.f;

			m_pTransformCom->Set_Scale(XMVectorSet((_float)Right, (_float)(Up + TimeDelta * 30.f), (_float)(Look + TimeDelta * 30.f), 0.f));
		}
		else if (pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
		{
			_float Right = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
			_float Up = m_pTransformCom->Get_Scale(CTransform::STATE_UP);
			_float Look = m_pTransformCom->Get_Scale(CTransform::STATE_LOOK);
			if (0 > mouseMove)
				TimeDelta *= -1.f;

			m_pTransformCom->Set_Scale(XMVectorSet((_float)Right, (_float)(Up + -TimeDelta * 30.f), (_float)(Look + TimeDelta * 30.f), 0.f));
		}
		
	}
	else if (pGameInstance->Get_DIKeyState(DIK_LSHIFT)) {
		_vector Position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		{
			_float Y = XMVectorGetY(Position);
			Y += TimeDelta * 300.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(Position, Y));
		}else if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		{
			_float Y = XMVectorGetY(Position);
			Y -= TimeDelta * 300.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(Position, Y));
		}else if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			_float X = XMVectorGetX(Position);
			X -= TimeDelta * 300.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetX(Position, X));
		}
		else if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			_float X = XMVectorGetX(Position);
			X += TimeDelta * 300.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetX(Position, X));
		}
	}

	if (FALSE == LControl)
	{
		if (pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
			m_pTransformCom->Rotation_Axis(XMVectorSet(0.f, 0.f, 1.f, 0.f), 0.3f);
		else if (pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
			m_pTransformCom->Rotation_Axis(XMVectorSet(0.f, 0.f, 1.f, 0.f), -0.3f);
	}
	
	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		m_pTransformCom->Rotation_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	}

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int UIObject::LateTick(_double TimeDelta)
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form * m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
	if (2 != m_form->m_tabCtrl.GetCurSel())
		return _int();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);



	return m_Dead;
}

HRESULT UIObject::Render()
{
	CMainFrame* m_mainFrm = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	Form * m_form = dynamic_cast<Form*>(m_mainFrm->m_MainSpliiter.GetPane(0, 0));
	if (2 != m_form->m_tabCtrl.GetCurSel())
		return _int();


	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));
	m_pVIBufferCom->SetUp_ValueOnShader("g_PickCheck",&m_PickCheck, sizeof(_bool));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);


	m_pVIBufferCom->Render(1);

	return S_OK;
}

_bool UIObject::PickCheck(POINT _PT)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector pt = pGameInstance->GetWindowPos(m_pDeviceContext, 1280, 720, (_float)_PT.x, (_float)_PT.y);
	RELEASE_INSTANCE(CGameInstance);

	RECT rect1{
		XMVectorGetX(pt) - 10,
		XMVectorGetY(pt) - 10,
		XMVectorGetX(pt) + 10,
		XMVectorGetY(pt) + 10
	};


	_vector Position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	RECT rect2{
		XMVectorGetX(Position) - 10,
		XMVectorGetY(Position) - 10,
		XMVectorGetX(Position) + 10,
		XMVectorGetY(Position) + 10,
	};

	RECT rect3 = {};

	if (TRUE == IntersectRect(&rect3, &rect1, &rect2))
		return TRUE;
	
	return FALSE;
}

HRESULT UIObject::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(5.0f);
	if (FAILED(__super::SetUp_Components(0, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(0, m_UIOBJDesc.m_TextureTag, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

UIObject * UIObject::Create(ID3D11Device * _dx11Device, ID3D11DeviceContext * _dx11DeviceContext)
{
	UIObject* pInstance = new UIObject(_dx11Device, _dx11DeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX(L"Failed to Create UIObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * UIObject::Clone(void * pArg)
{
	UIObject* pInstance = new UIObject(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX(L"Failed to Clone UIObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void UIObject::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
}
