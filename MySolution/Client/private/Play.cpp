#include "stdafx.h"
#include "..\public\Play.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "Level_Loading.h"

Play::Play(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

Play::Play(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT Play::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT Play::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float X = 0.f, Y = 0.f;
	_float2 TextureSize = m_pTextureCom->GetTextureInfo(0);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, 300 , 200);

	X = TextureSize.x * 0.15f;
	Y = TextureSize.y * 0.1f;

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSet(X, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSet(0.f, Y, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int Play::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	if (200 <= pt.x && 400 >= pt.x &&
		100 <= pt.y && 300 >= pt.y)
		m_Picked = TRUE;
	else 
		m_Picked = FALSE;

	m_X++;

	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

_int Play::LateTick(_double TimeDelta)
{	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	if (10 < m_X)
		m_X = 0;

	return _int();
}

HRESULT Play::Render()
{
	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_Picked", &m_Picked, sizeof(_bool));
	
	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);
	m_pVIBufferCom->SetUp_TextureOnShader("g_AlphaTexture", m_pTextureCom, 1);
	m_pVIBufferCom->SetUp_TextureOnShader("g_GlassTexture", m_pTextureCom, 2);

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &m_X, sizeof(_bool));

	m_pVIBufferCom->Render(1);

	return S_OK;
}

HRESULT Play::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Transfom */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Play"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

Play * Play::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Play*	pInstance = new Play(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating Play");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * Play::Clone(void * pArg)
{
	Play*	pInstance = new Play(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating Play");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Play::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
