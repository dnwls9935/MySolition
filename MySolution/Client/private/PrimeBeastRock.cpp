#include "stdafx.h"
#include "..\public\PrimeBeastRock.h"
#include "GameInstance.h"
#include "Player.h"
#include "HierarchyNode.h"
#include "PrimeBeast.h"

PrimeBeastRock::PrimeBeastRock(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

PrimeBeastRock::PrimeBeastRock(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT PrimeBeastRock::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT PrimeBeastRock::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	ZeroMemory(&m_PrbDesc, sizeof(PRBDESC));
	memcpy(&m_PrbDesc, (PRBDESC*)pArg, sizeof(PRBDESC));
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_Type = CGameObject::OBJTYPE_ID::ENEMY_OBJECT;

	return S_OK;
}

_int PrimeBeastRock::Tick(_double TimeDelta)
{
	_matrix BoneMatrix = static_cast<PrimeBeast*>(m_PrbDesc.m_PrimeBeast)->SetRockPosition();
/*
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, BoneMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, BoneMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, BoneMatrix.r[2]);*/
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, BoneMatrix.r[3]);


	return _int();
}

_int PrimeBeastRock::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	return _int();
}

HRESULT PrimeBeastRock::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (TYPE_ROCK::BIG == m_PrbDesc.m_TypeRock)
	{
		m_RockModel->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
		m_RockModel->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
		m_RockModel->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

		if (FAILED(m_RockModel->Bind_Buffers()))
			return E_FAIL;

		for (_uint i = 0; i < m_RockModel->Get_NumMeshContainer(); ++i)
		{
			m_RockModel->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

			m_RockModel->Render(i, 0);
		}
	}
	else if (TYPE_ROCK::SMALL == m_PrbDesc.m_TypeRock)
	{
		m_SnowModel->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
		m_SnowModel->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
		m_SnowModel->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

		if (FAILED(m_SnowModel->Bind_Buffers()))
			return E_FAIL;

		for (_uint i = 0; i < m_SnowModel->Get_NumMeshContainer(); ++i)
		{
			m_SnowModel->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

			m_SnowModel->Render(i, 0);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT PrimeBeastRock::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 13.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);

	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PrimeBeastBig"), TEXT("Com_RockModel"), (CComponent**)&m_RockModel)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_PrimeBeastSmall"), TEXT("Com_SnowModel"), (CComponent**)&m_SnowModel)))
		return E_FAIL;

	return S_OK;
}

PrimeBeastRock * PrimeBeastRock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	PrimeBeastRock*	pInstance = new PrimeBeastRock(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * PrimeBeastRock::Clone(void * pArg)
{
	PrimeBeastRock*	pInstance = new PrimeBeastRock(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void PrimeBeastRock::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_RockModel);
	Safe_Release(m_SnowModel);
	Safe_Release(m_pRendererCom);
}
