#include "stdafx.h"
#include "..\public\Blocked.h"
#include "GameInstance.h"
#include "BossPrimeBeast.h"

Blocked::Blocked(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

Blocked::Blocked(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT Blocked::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Blocked::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	list<CGameObject*> Enemys = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));
	for (auto& Enemy : Enemys)
	{
		if (CGameObject::OBJTYPE_ID::BOSS == Enemy->GetID())
		{
			m_Boss = Enemy;
			Safe_AddRef(Enemy);
		}
	}

	m_pTransformCom->Set_Scale(XMVectorSet(2.f, 2.f, 2.f, 0.f));

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int Blocked::Tick(_double TimeDelta)
{
	_vector Position = static_cast<CTransform*>(m_Boss->GetComponent(TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(Position, XMVectorGetY(Position) + 3.f));

	_bool b= static_cast<BossPrimeBeast*>(m_Boss)->GetChargeATT();
	_bool dead = static_cast<BossPrimeBeast*>(m_Boss)->GetChargeATT();

	if (TRUE == dead)
		m_Dead = TRUE;

	if (FALSE == b)
		return _int();

	if(FALSE == m_Reverse)
		m_Y += TimeDelta * 2.f;
	else
		m_Y += -TimeDelta * 2.f;

	return _int();
}

_int Blocked::LateTick(_double TimeDelta)
{
	_bool b = static_cast<BossPrimeBeast*>(m_Boss)->GetChargeATT();
	if (FALSE == b)
		return _int();
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (FALSE == m_Reverse)
	{
		if (1 <= m_Y)
		{
			m_Reverse = TRUE;
			m_Y = 1.f;
		}
	}
	else {
		if (0 >= m_Y)
		{
			m_Reverse = FALSE;
			m_Y = 0.f;
		}
	}


	return _int();
}

HRESULT Blocked::Render()
{
	if (1 <= m_Y)
		return S_OK;

	_bool b = static_cast<BossPrimeBeast*>(m_Boss)->GetChargeATT();
	if (FALSE == b)
		return S_OK;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_ModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_ModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_ModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_ModelCom->SetUp_ValueOnShader("g_Y", &m_Y, sizeof(_int));

	if (FAILED(m_ModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_ModelCom->Get_NumMeshContainer(); ++i)
	{
		m_ModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_ModelCom->SetUp_TextureOnShader("g_BurnedTexture", i, aiTextureType_OPACITY);

		m_ModelCom->Render(i, 2);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT Blocked::SetUp_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(120.0f);
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Blocked"), TEXT("Com_Model"), (CComponent**)&m_ModelCom)))
		return E_FAIL;

	return S_OK;
}

Blocked * Blocked::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	Blocked*	pInstance = new Blocked(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating Blocked");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * Blocked::Clone(void * pArg)
{
	Blocked*	pInstance = new Blocked(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloning Blocked");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Blocked::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_ModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_Boss);
}
