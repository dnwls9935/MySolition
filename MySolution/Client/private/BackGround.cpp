#include "stdafx.h"
#include "..\public\BackGround.h"

#include "GameInstance.h"

CBackGround::CBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBackGround::CBackGround(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBackGround::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);
	m_WorldMatrix = XMMatrixIdentity();

	return S_OK;
}

HRESULT CBackGround::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_WorldMatrix.r[0] = XMVectorSet((_float)g_iWinCX, 0.f, 0.f, 0.f);
	m_WorldMatrix.r[1] = XMVectorSet(0.0f, (_float)g_iWinCY, 0.f, 0.f);
	m_WorldMatrix.r[2] = XMVectorSet(0.0f, 0.f, 1.f, 0.f);

	return S_OK;
}

_int CBackGround::Tick(_double TimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (LEVEL_GAMEPLAY == pGameInstance->GetNextLevelID())
	{
		m_Size = 1.f;
		RELEASE_INSTANCE(CGameInstance)
		return _int();
	}

	m_Size = pGameInstance->Lerp(m_Size, 1.5, TimeDelta);

	m_WorldMatrix.r[0] = XMVectorSet((_float)g_iWinCX * m_Size, 0.f, 0.f, 0.f);
	m_WorldMatrix.r[1] = XMVectorSet(0.0f, (_float)g_iWinCY * m_Size, 0.f, 0.f);

	if (1.3 <= m_Size)
		m_Size = 1.f;


	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CBackGround::LateTick(_double TimeDelta)
{	
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);

	return _int();
}

HRESULT CBackGround::Render()
{
	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_WorldMatrix), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);	
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, pGameInstance->GetNextLevelID());

	RELEASE_INSTANCE(CGameInstance);

	m_pVIBufferCom->Render(0);

	return S_OK;
}

HRESULT CBackGround::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CBackGround * CBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBackGround*	pInstance = new CBackGround(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CBackGround");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBackGround::Clone(void * pArg)
{
	CBackGround*	pInstance = new CBackGround(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
