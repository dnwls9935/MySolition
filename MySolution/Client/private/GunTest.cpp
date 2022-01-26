#include "stdafx.h"
#include "..\public\GunTest.h"
#include "Player.h"
#include "HierarchyNode.h"
#include "GameInstance.h"

GunTest::GunTest(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

GunTest::GunTest(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT GunTest::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT GunTest::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::IDLE);

	m_OriginMatrix = m_pTransformCom->Get_WorldMatrix();

	m_Type = CGameObject::OBJTYPE_ID::PLAYER_WEAPONE;

	return S_OK;
}

_int GunTest::Tick(_double TimeDelta)
{
	KeyCheck();
	m_pModelCom->Update_CombinedTransformationMatrix(TimeDelta);
	return _int();
}

_int GunTest::LateTick(_double TimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);

	if (m_pModelCom->GetAnimationFinished())
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::IDLE);


	return _int();
}

HRESULT GunTest::Render()
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

void GunTest::SetUpWeapon(_fmatrix WeaponeBoneMatrix, _fmatrix PlayerWorldMatrix)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
 	//_matrix WeaponBoneMatrix = static_cast<CPlayer*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player")).front())->SetUpWeapon();
	//_matrix WorldMatrix = static_cast<CTransform*>(pGameInstance->GetObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Player")).front()->GetComponent(TEXT("Com_Transform")))->Get_WorldMatrix();
	RELEASE_INSTANCE(CGameInstance);
	
	_vector		vPosition = WeaponeBoneMatrix.r[3];
	vPosition = XMVectorSetW(vPosition, 1.f);

	_vector		vLook = PlayerWorldMatrix.r[2];
	vLook = XMVector3Normalize(vLook);

	_vector		vRight = XMVector3Cross(PlayerWorldMatrix.r[1], vLook);
	vRight = XMVector3Normalize(vRight);

	_vector		vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
}

void GunTest::KeyCheck()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
	{
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::RELOAD);
	}

	if (pGameInstance->Get_MouseButtonState(CInput_Device::MOUSEBUTTONSTATE::MBS_LBUTTON))
	{
		m_pModelCom->SetUp_AnimationIndex((_int)ANIMATION_STATE::FIRE);
	}

	RELEASE_INSTANCE(CGameInstance);
}


HRESULT GunTest::SetUp_Components()
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
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GunTest"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

GunTest * GunTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	GunTest*	pInstance = new GunTest(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * GunTest::Clone(void * pArg)
{
	GunTest*	pInstance = new GunTest(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void GunTest::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
