#include "stdafx.h"
#include "..\public\SnowScreen.h"
#include "GameInstance.h"
#include "Player.h"
#include "Camera_Dynamic.h"

SnowScreen::SnowScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

SnowScreen::SnowScreen(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT SnowScreen::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT SnowScreen::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_Player = static_cast<CPlayer*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player")).front());
	m_Camera = static_cast<CCamera_Dynamic*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Camera")).front());
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int SnowScreen::Tick(_double TimeDelta)
{
	if (TRUE == static_cast<CPlayer*>(m_Player)->GetIsIn())
		return _int();

	m_AlphaValue += TimeDelta;

	if (FALSE == m_Show)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_float X = 0.f, Y = 0.f;
		_float2 TextureSize = m_pTextureCom->GetTextureInfo(0);

		X = _float(rand() % g_iWinCX);
		Y = _float(rand() % g_iWinCY);

		_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, X , Y);

		X = TextureSize.x * 0.5f;
		Y = TextureSize.y * 0.5f;

		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(X, 0.f, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, Y, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

		m_Show = TRUE;	
		RELEASE_INSTANCE(CGameInstance);
	}


	return _int();
}

_int SnowScreen::LateTick(_double TimeDelta)
{	
	if (TRUE == static_cast<CPlayer*>(m_Player)->GetIsIn())
		return _int();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	if (1.5 <= m_AlphaValue)
	{
		m_AlphaValue = 0.f;
		m_Show = FALSE;
	}

	return _int();
}

HRESULT SnowScreen::Render()
{
	if (TRUE == m_Camera->GetFocus())
		return S_OK;

	if (FALSE == m_Show && TRUE == static_cast<CPlayer*>(m_Player)->GetIsIn())
		return S_OK;

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	_float X = rand() %  4, Y = rand() % 4;

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_AlphaValue", &m_AlphaValue, sizeof(_float));


	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);
	m_pVIBufferCom->Render(7);

	return S_OK;
}


HRESULT SnowScreen::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Transfom */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SnowFlakesScreenDif"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

SnowScreen * SnowScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	SnowScreen*	pInstance = new SnowScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CrossSight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * SnowScreen::Clone(void * pArg)
{
	SnowScreen*	pInstance = new SnowScreen(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CrossSight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SnowScreen::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
