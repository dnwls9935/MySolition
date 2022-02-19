#include "stdafx.h"
#include "..\public\HitNum.h"

#include "GameInstance.h"

HitNum::HitNum(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

HitNum::HitNum(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT HitNum::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_ProjMatrix = XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f);

	return S_OK;
}

HRESULT HitNum::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	
	if (nullptr != pArg)
	{
		HUDESC HUD;
		ZeroMemory(&HUD, sizeof(HUD));
		memcpy(&HUD, pArg, sizeof(HUDESC));

		Num = HUD.Num;
		m_Parent = HUD.Parent;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, HUD.Position);

		m_pTransformCom->Set_Scale(XMVectorSet(0.5f, 0.5f, 0.5f,1.f));
	}



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int HitNum::Tick(_double TimeDelta)
{
	m_LifeTime += TimeDelta;

	Num100 = Num / 100;
	Num10 = Num / 10;
	Num1 = Num % 10;

	return _int();
}

_int HitNum::LateTick(_double TimeDelta)
{	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	if (0.5 <= m_LifeTime)
		m_Dead = TRUE;

	return m_Dead;
}

HRESULT HitNum::Render()
{
	if (FAILED(Render100()))
		return E_FAIL;

	if (FAILED(Render10()))
		return E_FAIL;

	if (FAILED(Render1()))
		return E_FAIL;

	return S_OK;
}

HRESULT HitNum::Render100()
{
	_float X = 0, Y = 0;
	if (5 <= Num100) {
		X = Num100 - 5;
		Y = 1;
	}
	else {
		X = Num100;
		Y = 0;
	}
	if (0 == X && 0 == Y)
		return S_OK;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_VIEW)), sizeof(_matrix));
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(pGameInstance->Get_Transform(CPipeLine::D3DTS_PROJECTION)), sizeof(_matrix));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT HitNum::Render10()
{
	_float X = 0, Y = 0;
	if (5 <= Num10) {
		X = Num10 - 5;
		Y = 1;
	} 
	else {
		X = Num10;
		Y = 0;
	}

	if (0 == Num100 && 0 == X && 0 == Y)
		return S_OK;
	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT HitNum::Render1()
{
	_float X = 0, Y = 0;
	if (5 <= Num1) {
		X = Num1 - 5;
		Y = 1;
	}
	else {
		X = Num1;
		Y = 0;
	}

	if (0 == Num100 && 0 == Num10 && 0 == X && 0 == Y)
		return S_OK;
/*
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector	WindowPosition = pGameInstance->GetWindowPos(m_pDeviceContext, (_float)g_iWinCX, (_float)g_iWinCY, g_iWinCX * 0.5f + 15 + 15, g_iWinCY * 0.5f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, WindowPosition);
*/
	m_pVIBufferCom->SetUp_ValueOnShader("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float) * 16);
	m_pVIBufferCom->SetUp_ValueOnShader("g_ProjMatrix", &XMMatrixTranspose(m_ProjMatrix), sizeof(XMMATRIX));

	m_pVIBufferCom->SetUp_ValueOnShader("g_X", &X, sizeof(_float));
	m_pVIBufferCom->SetUp_ValueOnShader("g_Y", &Y, sizeof(_float));

	m_pVIBufferCom->SetUp_TextureOnShader("g_DiffuseTexture", m_pTextureCom, 0);

	m_pVIBufferCom->Render(9);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT HitNum::SetUp_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer0"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* Com_Transfom */
	if (FAILED(__super::SetUp_Components(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::SetUp_Components(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tex_Font"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HitNum * HitNum::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	HitNum*	pInstance = new HitNum(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating Num");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * HitNum::Clone(void * pArg)
{
	HitNum*	pInstance = new HitNum(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating Num");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void HitNum::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}
