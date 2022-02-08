#include "stdafx.h"
#include "..\public\SMG.h"
#include "Player.h"
#include "HierarchyNode.h"
#include "GameInstance.h"
#include "UI.h"

#include <iostream>

SMG::SMG(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

SMG::SMG(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT SMG::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT SMG::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_Type = CGameObject::OBJTYPE_ID::PLAYER_WEAPONE;

	return S_OK;
}

_int SMG::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	KeyCheck();
	m_BarPercent = (_float)m_Ammo / (_float)m_MaxAmmo;


	list<CGameObject*> ObjectList = pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
	for (auto& Object : ObjectList)
	{
		if (CGameObject::UITYPE_ID::AMMO == static_cast<UI*>(Object)->GetUIDesc().m_UITypeID)
		{
			static_cast<UI*>(Object)->SetLength(m_BarPercent, FALSE);
		}
	}


	if (m_pModelCom->GetAnimationFinished())
		m_AnimationPlay = FALSE;

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int SMG::LateTick(_double TimeDelta)
{
	m_FireFrame = FALSE;
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (TRUE == m_AnimationPlay)
		m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta * m_FrameSpeed);
	else
		m_pModelCom->Update_CombinedTransformationMatrix(0.0);

	if (m_pModelCom->GetAnimationFinished())
	{
		if ((_int)ANIMATION_STATE::RE_HYPERION == m_pModelCom->GetCurrentAnimation())
		{
			m_Reload = FALSE;
			Reloading();
		}
	}

	return _int();
}

HRESULT SMG::Render()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pModelCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	if (FAILED(m_pModelCom->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshContainer(); ++i)
	{
		m_pModelCom->SetUp_TextureOnShader("g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pModelCom->Render(i, 1);
	}

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void SMG::SetUpWeapon(_fmatrix WeaponeBoneMatrix, _fmatrix PlayerWorldMatrix)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector		vPosition = WeaponeBoneMatrix.r[3];
	vPosition = XMVectorSetW(vPosition, 1.f);

	_vector		vLook = WeaponeBoneMatrix.r[2];
	vLook = XMVector3Normalize(vLook);

	_vector		vRight = XMVector3Cross(WeaponeBoneMatrix.r[1], vLook);
	vRight = XMVector3Normalize(vRight);

	_vector		vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

}

void SMG::KeyCheck()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80 &&
		m_Ammo <= 25)
	{
		m_AnimationPlay = TRUE;
		m_FrameSpeed = 1.0;
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RE_HYPERION);

		m_Reload = TRUE;
	}

	if (pGameInstance->Get_MouseButtonState(CInput_Device::MBS_LBUTTON) && 
		m_Ammo > 0)
	{
		m_FrameSpeed = 5.f;
		m_AnimationPlay = TRUE;

		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::FIRE_HYPERION);

		if (0 == m_pModelCom->GetCurrentAnimationFrame())
		{
			m_Ammo--;
			m_FireFrame = TRUE;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void SMG::Reloading()
{
	_int Ammo = m_MaxAmmo - m_Ammo;

	m_Magazine -= Ammo;
	m_Ammo = m_MaxAmmo;
}


HRESULT SMG::SetUp_Components()
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

	/* Com_Model */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SMG"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

SMG * SMG::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	SMG*	pInstance = new SMG(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * SMG::Clone(void * pArg)
{
	SMG*	pInstance = new SMG(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void SMG::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
