#include "stdafx.h"
#include "..\public\HITUI.h"

#include "GameInstance.h"

#include <iostream>

HITUI::HITUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

HITUI::HITUI(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT HITUI::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT HITUI::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_ID = SHOW::SHOW_END;

	if (FAILED(LeftHit()))
		return E_FAIL;
	if (FAILED(RightHit()))
		return E_FAIL;
	if (FAILED(CenterHit()))
		return E_FAIL;

	m_UIDesc.m_UITypeID = CGameObject::UITYPE_ID::HIT;

	return S_OK;
}

_int HITUI::Tick(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc >= 4.0)
	{
		m_ID = SHOW::SHOW_END;
		m_TimeAcc = 0.0;
	}


	return _int();
}

_int HITUI::LateTick(_double TimeDelta)
{	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT HITUI::Render()
{
	for (_int i = 0; i < 3; i++)
	{
		if (SHOW::HP == m_ID && i == 2) {
			m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom[i]->Get_WorldMatrix()), sizeof(_float) * 16);
			m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
			m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

			m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, i);

			m_pVIBufferCom->Render(0);
		}
		else if (SHOW::SHIELD == m_ID && i != 2) {
			m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom[i]->Get_WorldMatrix()), sizeof(_float) * 16);
			m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
			m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

			m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, i);

			m_pVIBufferCom->Render(0);
		}

	}

	return S_OK;
}

HRESULT HITUI::LeftHit()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float X = 0.f, Y = 0.f;
	_float2 TextureSize = m_pTextureCom->GetTextureInfo(0);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, 100,  g_iWinCY * 0.5f);

	X = TextureSize.x;
	Y = TextureSize.y;

	m_pTransformCom[0]->Set_State(CTransform::STATE_RIGHT, XMVectorSet(X, 0.f, 0.f, 0.f));
	m_pTransformCom[0]->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, Y, 0.f, 0.f));
	m_pTransformCom[0]->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pTransformCom[0]->Set_State(CTransform::STATE_POSITION, WindowPosition);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT HITUI::RightHit()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float X = 0.f, Y = 0.f;
	_float2 TextureSize = m_pTextureCom->GetTextureInfo(1);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, g_iWinCX - 100, g_iWinCY * 0.5f);

	X = TextureSize.x;
	Y = TextureSize.y;

	m_pTransformCom[1]->Set_State(CTransform::STATE_RIGHT, XMVectorSet(X, 0.f, 0.f, 0.f));
	m_pTransformCom[1]->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, Y, 0.f, 0.f));
	m_pTransformCom[1]->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pTransformCom[1]->Set_State(CTransform::STATE_POSITION, WindowPosition);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT HITUI::CenterHit()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float X = 0.f, Y = 0.f;
	_float2 TextureSize = m_pTextureCom->GetTextureInfo(2);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, g_iWinCX * 0.5f, 100);

	X = TextureSize.x;
	Y = TextureSize.y;

	m_pTransformCom[2]->Set_State(CTransform::STATE_RIGHT, XMVectorSet(X, 0.f, 0.f, 0.f));
	m_pTransformCom[2]->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, Y, 0.f, 0.f));
	m_pTransformCom[2]->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pTransformCom[2]->Set_State(CTransform::STATE_POSITION, WindowPosition);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT HITUI::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer0"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Transfom */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform0"), (CComponent**)&m_pTransformCom[0])))
		return E_FAIL;
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform1"), (CComponent**)&m_pTransformCom[1])))
		return E_FAIL;
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pTransformCom[2])))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hit"), TEXT("Com_LeftTexture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HITUI * HITUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	HITUI*	pInstance = new HITUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * HITUI::Clone(void * pArg)
{
	HITUI*	pInstance = new HITUI(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void HITUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	for (auto& Trasnform : m_pTransformCom)
		Safe_Release(Trasnform);

	Safe_Release(m_pRendererCom);
}
