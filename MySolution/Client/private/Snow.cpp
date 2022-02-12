#include "stdafx.h"
#include "..\public\Snow.h"
#include "GameInstance.h"

Snow::Snow(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

Snow::Snow(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT Snow::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Snow::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	

	return S_OK;
}

_int Snow::Tick(_double TimeDelta)
{
	m_VIBufferCom->Update(TimeDelta);

	return _int();
}

_int Snow::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHA, this);

	return _int();
}

HRESULT Snow::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_int		X = rand() % 16 + 1;
	_int		Y = rand() % 16 + 1;

	m_VIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_VIBufferCom->SetUp_ValueOnShader("g_vCamPosition", (void*)&pGameInstance->Get_CamPosition(), sizeof(_vector));

	m_VIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_int));
	m_VIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_int));

	m_VIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_TextureCom, 0);
	m_VIBufferCom->Render(0);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Snow::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SnowFlakesDif"), TEXT("Com_Texture"), (CComponent**)&m_TextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstace"), TEXT("Com_Buffer"), (CComponent**)&m_VIBufferCom)))
		return E_FAIL;

	return S_OK;
}

Snow * Snow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Snow*	pInstance = new Snow(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating Snow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * Snow::Clone(void * pArg)
{
	Snow*	pInstance = new Snow(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning Snow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Snow::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_VIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_TextureCom);

}
