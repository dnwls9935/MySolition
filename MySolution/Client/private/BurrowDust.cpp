#include "stdafx.h"
#include "..\public\BurrowDust.h"
#include "GameInstance.h"
#include "BugMorph.h"

BurrowDust::BurrowDust(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

BurrowDust::BurrowDust(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT BurrowDust::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT BurrowDust::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	if (nullptr != pArg)
	{
		BurrowDust::BURROWDUST BurrowDust;
		ZeroMemory(&BurrowDust, sizeof(BurrowDust::BURROWDUST));

		memcpy(&BurrowDust, pArg, sizeof(BurrowDust::BURROWDUST));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, BurrowDust.Position);

		m_Boss = static_cast<BugMorph*>(BurrowDust.Parent);
		Safe_AddRef(m_Boss);
	}
	
	m_Dead = FALSE;
	return S_OK;
}

_int BurrowDust::Tick(_double TimeDelta)
{
	m_Dead = m_VIBufferCom->Update(TimeDelta);
	
	return _int();
}

_int BurrowDust::LateTick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return m_Dead;
}

HRESULT BurrowDust::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	m_VIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_VIBufferCom->SetUp_ValueOnShader("g_vCamPosition", (void*)&pGameInstance->Get_CamPosition(), sizeof(_vector));

	m_VIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_TextureCom, 0);
	m_VIBufferCom->Render(1);

	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT BurrowDust::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 7.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(30.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow_Decal_Dif"), TEXT("Com_Texture"), (CComponent**)&m_TextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstace_Dust"), TEXT("Com_Buffer"), (CComponent**)&m_VIBufferCom)))
		return E_FAIL;

	return S_OK;
}

BurrowDust * BurrowDust::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	BurrowDust*	pInstance = new BurrowDust(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating BurrowDust");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * BurrowDust::Clone(void * pArg)
{
	BurrowDust*	pInstance = new BurrowDust(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning BurrowDust");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void BurrowDust::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_VIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_TextureCom);
	Safe_Release(m_Boss);

}
