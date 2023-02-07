#include "stdafx.h"
#include "..\public\CombatMap.h"
#include "GameInstance.h"

CCombatMap::CCombatMap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CCombatMap::CCombatMap(const CCombatMap & rhs)
	: CCanvas(rhs)
{
}

HRESULT CCombatMap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCombatMap::Initialize(void * pArg)
{
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	if (nullptr != pArg)
		memcpy(&CanvasDesc, pArg, sizeof(CanvasDesc));

	if (FAILED(__super::Initialize(&CanvasDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	/*m_fSizeX = (_float)g_iWinSizeX / 1.5f;
	m_fSizeY = (_float)g_iWinSizeY / 4.f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
*/
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());


	return S_OK;
}

HRESULT CCombatMap::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bLast_Initlize = true;

	return S_OK;
}

void CCombatMap::Tick(_double TimeDelta)
{
	Last_Initialize();
	/*
	char Name[MAX_PATH] = "";

	WideCharToMultiByte(CP_ACP, 0, m_ObjectName, MAX_PATH, Name, MAX_PATH, NULL, NULL);

	ImGui::Text("%s", Name);*/

		

	__super::Tick(TimeDelta);
}

void CCombatMap::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CCombatMap::Render()
{
	if (!m_bRenderActive)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;
	
	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CCombatMap::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_CanvasDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCombatMap::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CCombatMap::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
	Safe_Release(m_pTextureCom);
	Remove_component(TEXT("Com_Texture"));

	lstrcpy(m_UIDesc.m_pTextureTag, NewComPonentTag.c_str());

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iLevel, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		assert("CTurnCharcterUI Change_Texture");
}

void CCombatMap::Set_RenderActive(_bool bActive)
{
	__super::Set_RenderActive(bActive);
	m_bRenderActive = bActive;
}



CCombatMap * CCombatMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCombatMap*		pInstance = new CCombatMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCombatMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCombatMap::Clone(void * pArg)
{
	CCombatMap*		pInstance = new CCombatMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCombatMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCombatMap::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}