#include "stdafx.h"
#include "..\public\PlayerHP.h"

#include "GameInstance.h"

PlayerHP::PlayerHP(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

PlayerHP::PlayerHP(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT PlayerHP::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT PlayerHP::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);
	m_WorldMatrix = XMMatrixIdentity();


	if (FAILED(SetUp_Components()))
		return E_FAIL;


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float X = 0.f, Y = 0.f;
	_float2 TextureSize = m_pTextureCom->GetTextureInfo(0);

	X = TextureSize.x * 5.f * 4.f;
	Y = TextureSize.y * 5.f * 4.f;

	m_WorldMatrix.r[0] = XMVectorSet(X, 0.f, 0.f, 0.f);
	m_WorldMatrix.r[1] = XMVectorSet(0.f, Y, 0.f, 0.f);
	m_WorldMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f);
	m_WorldMatrix.r[3] = pGameInstance->GetWindowPos(m_pDeviceContext, g_iWinCX, g_iWinCY, 100, 600.f);

	m_WorldMatrix *= XMMatrixRotationY(10.f);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int PlayerHP::Tick(_double TimeDelta)
{



	return _int();
}

_int PlayerHP::LateTick(_double TimeDelta)
{	
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT PlayerHP::Render()
{
	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_WorldMatrix), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);	
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));
	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(0);

	return S_OK;
}

HRESULT PlayerHP::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerHP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

PlayerHP * PlayerHP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	PlayerHP*	pInstance = new PlayerHP(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating PlayerHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * PlayerHP::Clone(void * pArg)
{
	PlayerHP*	pInstance = new PlayerHP(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating PlayerHP");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void PlayerHP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
