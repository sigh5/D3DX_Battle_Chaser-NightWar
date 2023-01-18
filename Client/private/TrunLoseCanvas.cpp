#include "stdafx.h"
#include "..\public\TrunLoseCanvas.h"
#include "GameInstance.h"

#include "MyImage.h"

CTrunLoseCanvas::CTrunLoseCanvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CTrunLoseCanvas::CTrunLoseCanvas(const CTrunLoseCanvas & rhs)
	: CCanvas(rhs)
{
}

HRESULT CTrunLoseCanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CTrunLoseCanvas::Initialize(void * pArg)
{
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	lstrcpy(CanvasDesc.m_pTextureTag, TEXT("Prototype_Component_Texture_TrunBattle_LoseCanvas"));

	if (nullptr != pArg)
		memcpy(&CanvasDesc, pArg, sizeof(CanvasDesc));

	if (FAILED(__super::Initialize(&CanvasDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX / 11;
	m_fSizeY = (_float)g_iWinSizeY * 1.1f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - m_fSizeX * 0.5f - (585.f), -m_fY + m_fSizeY * 0.5f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTrunLoseCanvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
#ifdef NOMODLES


#else
	for (auto &pChild : m_ChildrenVec)
	{
		if (nullptr == dynamic_cast<CMyImage*>(pChild))
			continue;;

		static_cast<CMyImage*>(pChild)->Set_MaxTextureNum(7);
	}


#endif

	m_bLast_Initlize = true;

	return S_OK;
}

void CTrunLoseCanvas::Tick(_double TimeDelta)
{
	Last_Initialize();

	__super::Tick(TimeDelta);


#ifdef NOMODLES


#else
	for (auto &pChild : m_ChildrenVec)
	{
		if (nullptr == dynamic_cast<CMyImage*>(pChild))
			continue;;

		static_cast<CMyImage*>(pChild)->Move_Lose_Texture(TimeDelta);
	}
#endif	
}

void CTrunLoseCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTrunLoseCanvas::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();

	return S_OK;
}

void CTrunLoseCanvas::Set_RenderActive(_bool bTrue)
{
}

HRESULT CTrunLoseCanvas::SetUp_Components()
{
	/*For.Com_Renderer */
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_CanvasDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrunLoseCanvas::SetUp_ShaderResources()
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

CTrunLoseCanvas * CTrunLoseCanvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrunLoseCanvas*		pInstance = new CTrunLoseCanvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrunLoseCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrunLoseCanvas::Clone(void * pArg)
{
	CTrunLoseCanvas*		pInstance = new CTrunLoseCanvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrunWinCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrunLoseCanvas::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);


}