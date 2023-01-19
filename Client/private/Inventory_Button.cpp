#include "stdafx.h"
#include "..\public\Inventory_Button.h"

#include "GameInstance.h"
#include "CombatController.h"
#include "CombatActors.h"
#include "MyImage.h"

CInventory_Button::CInventory_Button(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CInventory_Button::CInventory_Button(const CInventory_Button & rhs)
	: CUI(rhs)
{
}

void CInventory_Button::Set_ButtonImage(CUI * pUI)
{
}

HRESULT CInventory_Button::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventory_Button::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UIDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX / 16;
	m_fSizeY = (_float)g_iWinSizeY / 16;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - m_fSizeX * 0.5f, -m_fY + m_fSizeY * 0.5f + 50.f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	m_bRenderActive = false;
	return S_OK;
}

HRESULT CInventory_Button::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	RELEASE_INSTANCE(CGameInstance);
	m_bLast_Initlize = true;
	return S_OK;

}

void CInventory_Button::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
	{

	}



	RELEASE_INSTANCE(CGameInstance);
}

void CInventory_Button::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInventory_Button::Render()
{
	if (!m_bRenderActive)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(1);		// UI 1번 알파블랜딩
	m_pVIBufferCom->Render();

	return S_OK;
}

void CInventory_Button::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
	Safe_Release(m_pTextureCom);
	Remove_component(TEXT("Com_Texture"));

	lstrcpy(m_UIDesc.m_pTextureTag, NewComPonentTag.c_str());

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iLevel, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		assert("CUIButton Change_Texture");
}

_bool CInventory_Button::Click_This_Button()
{
	if (!m_bRenderActive)
		return false;

	_bool bResult = false;

	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_long lLeft, lTop, lRight, lBottom;
	_float4	vPos;

	memcpy(&vPos, &m_pTransformCom->Get_WorldMatrix().r[3], sizeof(_float4));

	_float3 vScale;
	XMStoreFloat3(&vScale, XMLoadFloat3(&m_pTransformCom->Get_Scaled()));

	lLeft = (_long)(vPos.x + g_iWinSizeX*0.5f - vScale.x*0.5f);
	lRight = (_long)(vPos.x + g_iWinSizeX*0.5f + vScale.x*0.5f);
	lTop = (_long)(g_iWinSizeY * 0.5f - vPos.y - vScale.y*0.5f);
	lBottom = (_long)(g_iWinSizeY * 0.5f - vPos.y + vScale.y*0.5f);

	RECT rcButton = { lLeft, lTop, lRight, lBottom };

	if (PtInRect(&rcButton, ptMouse))
	{
		bResult = true;
	}

	return bResult;
}

HRESULT CInventory_Button::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}


HRESULT CInventory_Button::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTH))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CInventory_Button * CInventory_Button::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInventory_Button*		pInstance = new CInventory_Button(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInventory_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CInventory_Button::Clone(void * pArg)
{
	CInventory_Button*		pInstance = new CInventory_Button(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInventory_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventory_Button::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
