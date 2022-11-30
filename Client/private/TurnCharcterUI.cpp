#include "stdafx.h"
#include "..\public\TurnCharcterUI.h"

#include "GameInstance.h"
#include "Canvas.h"

#include "Hero_Gully.h"


CTurnCharcterUI::CTurnCharcterUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice,pContext)
{
}

CTurnCharcterUI::CTurnCharcterUI(const CTurnCharcterUI & rhs)
	: CUI(rhs)
{

}

HRESULT CTurnCharcterUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTurnCharcterUI::Initialize(void * pArg)
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

	if (FAILED(CUI::SetUp_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTurnCharcterUI::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	
	CGameObject* pGameObject = pInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Hero_Gully"));

	dynamic_cast<CHero_Gully*>(pGameObject)->m_Hero_GullyHPDelegater.bind(this,&CTurnCharcterUI::UI_Event);

	RELEASE_INSTANCE(CGameInstance);

	m_bLast_Initlize = true;
	return S_OK;
}

void CTurnCharcterUI::Tick(_double TimeDelta)
{
	Last_Initialize();

	if(m_bMoveCheck)
		m_CheckFunction(m_pTransformCom, m_vLimitPos, OBJ_UI);

	
	__super::Tick(TimeDelta);
}

void CTurnCharcterUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTurnCharcterUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	CUI::Begin_UI();

	m_pShaderCom->Begin(1);		// UI 1번 알파블랜딩
	m_pVIBufferCom->Render();

	CUI::End_UI();

	return S_OK;
}

void CTurnCharcterUI::UI_Event(_double TimeDelta,_uint iMoveSpeed)
{
	
	XMStoreFloat4(&m_vLimitPos,m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	m_vLimitPos.y += 50.f;

	m_CheckFunction = std::bind(&CClient_Manager::distance_Limit_UP, m_pTransformCom, m_vLimitPos,OBJ_UI);
	m_bMoveCheck = true;
}



HRESULT CTurnCharcterUI::SetUp_Components()
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

HRESULT CTurnCharcterUI::SetUp_ShaderResources()
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


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTurnCharcterUI * CTurnCharcterUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTurnCharcterUI*		pInstance = new CTurnCharcterUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTurnCharcterUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTurnCharcterUI::Clone(void * pArg)
{
	CTurnCharcterUI*		pInstance = new CTurnCharcterUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTurnCharcterUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTurnCharcterUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}