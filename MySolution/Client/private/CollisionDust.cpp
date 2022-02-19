#include "stdafx.h"
#include "..\public\CollisionDust.h"
#include "GameInstance.h"

CollisionDust::CollisionDust(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CollisionDust::CollisionDust(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CollisionDust::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CollisionDust::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	if (nullptr != pArg)
	{
		_vector Position = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		memcpy(&Position, pArg, sizeof(_vector));

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Position);
	}
	
	m_Dead = FALSE;
	return S_OK;
}

_int CollisionDust::Tick(_double TimeDelta)
{
	m_Dead = m_VIBufferCom->Update(TimeDelta);

	m_AnimationTimeAcc += TimeDelta * 10.f;

	m_X++;

	if (8 <= m_X)
	{
		m_Y++;
		m_X = 0.f;
		m_AnimationTimeAcc = 0.0;
	}

	return _int();
}

_int CollisionDust::LateTick(_double TimeDelta)
{
	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (4 <= m_Y)
	{
		m_X = 0.f;
		m_Y = 0.f;
	}

	return m_Dead;
}

HRESULT CollisionDust::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_VIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_VIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_VIBufferCom->SetUp_ValueOnShader("g_X", &m_X, sizeof(_float));
	m_VIBufferCom->SetUp_ValueOnShader("g_Y", &m_Y, sizeof(_float));

	m_VIBufferCom->SetUp_ValueOnShader("g_vCamPosition", (void*)&pGameInstance->Get_CamPosition(), sizeof(_vector));

	m_VIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_TextureCom, 0);
	m_VIBufferCom->SetUp_TextureOnShader("g_BlendTexture", m_TextureCom, 1);

	m_VIBufferCom->Render(2);

	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT CollisionDust::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DustA"), TEXT("Com_Texture"), (CComponent**)&m_TextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_PointInstace_CollisionDust"), TEXT("Com_Buffer"), (CComponent**)&m_VIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CollisionDust * CollisionDust::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CollisionDust*	pInstance = new CollisionDust(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CollisionDust");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CollisionDust::Clone(void * pArg)
{
	CollisionDust*	pInstance = new CollisionDust(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning CollisionDust");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CollisionDust::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_VIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_TextureCom);
}
