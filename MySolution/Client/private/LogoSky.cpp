#include "stdafx.h"
#include "..\public\LogoSky.h"

#include "GameInstance.h"
#include "Camera_Dynamic.h"

LogoSky::LogoSky(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

LogoSky::LogoSky(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT LogoSky::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT LogoSky::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int LogoSky::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pGameInstance->Get_CamPosition());

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int LogoSky::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);


	return _int();
}

HRESULT LogoSky::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);
	m_pVIBufferCom->Render(0);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT LogoSky::SetUp_Components()
{
	/* Com_Transform */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Cube"), TEXT("Com_SourTexture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	
	return S_OK;
}

LogoSky * LogoSky::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	LogoSky*	pInstance = new LogoSky(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating Sky");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * LogoSky::Clone(void * pArg)
{
	LogoSky* pInstance = new LogoSky(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pInstance);
		MSGBOX("Failed to Clone Sky");
	}

	return pInstance;
}

void LogoSky::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);

}
