#include "stdafx.h"
#include "..\public\NpcCanvas.h"
#include "GameInstance.h"
#include "OnlyFontUI.h"
#include "PlayerController.h"

CNpcCanvas::CNpcCanvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CNpcCanvas::CNpcCanvas(const CNpcCanvas & rhs)
	: CCanvas(rhs)
{
}

HRESULT CNpcCanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CNpcCanvas::Initialize(void * pArg)
{
	//m_ObjectName = TEXT("NpcCanvas");
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	lstrcpy(CanvasDesc.m_pTextureTag, TEXT("Prototype_Component_Texture_NPC"));

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

HRESULT CNpcCanvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
#ifdef NOMODLES
	CNpcCanvas::Set_RenderActive(true);
#else
	CNpcCanvas::Set_RenderActive(false);
#endif // NOMODLES



	m_bLast_Initlize = true;

	return S_OK;
}

void CNpcCanvas::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

#ifdef NOMODLES

#else
	
#endif // NOMODLES

	


}

void CNpcCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);


	
}

HRESULT CNpcCanvas::Render()
{
	if (!m_bRenderActive)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();

	/*if (m_iOption == 0)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("  Lose !!"), _float2(m_fFontPosX, m_fFontPosY), 0.f, _float2(1.f, 1.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		RELEASE_INSTANCE(CGameInstance);
	}
*/


	return S_OK;
}

void CNpcCanvas::Set_RenderActive(_bool bTrue)
{
	__super::Set_RenderActive(bTrue);

	for (auto& pUI : m_ChildrenVec)
		pUI->Set_RenderActive(bTrue);
}

void CNpcCanvas::Set_DialogOption(_int iOption)
{
	m_iOption = iOption; 
	if (m_iOption == 0)
	{
		for (auto &pChild : m_ChildrenVec)
		{
			if (!lstrcmp(pChild->Get_ObjectName(), TEXT("NpcTexture")))
			{
				CTexture* pTexture = static_cast<CTexture*>(pChild->Get_Component(L"Com_Texture"));
				pTexture->Set_SelectTextureIndex(5);
			}
			if (!lstrcmp(pChild->Get_ObjectName(), TEXT("OnlyFontUI")))
			{
				static_cast<COnlyFontUI*>(pChild)->Set_FontOption(0);
			}

		}
	}
	else if (m_iOption == 1)
	{
		for (auto &pChild : m_ChildrenVec)
		{
			if (!lstrcmp(pChild->Get_ObjectName(), TEXT("NpcTexture")))
			{
				CTexture* pTexture = static_cast<CTexture*>(pChild->Get_Component(L"Com_Texture"));
				pTexture->Set_SelectTextureIndex(0);
			}
			if (!lstrcmp(pChild->Get_ObjectName(), TEXT("OnlyFontUI")))
			{
				static_cast<COnlyFontUI*>(pChild)->Set_FontOption(1);
			}
		}
		CPlayerController::GetInstance()->Player_HP_MPUP();
	}
	else if (m_iOption == 2)
	{
		for (auto &pChild : m_ChildrenVec)
		{
			if (!lstrcmp(pChild->Get_ObjectName(), TEXT("NpcTexture")))
			{
				CTexture* pTexture = static_cast<CTexture*>(pChild->Get_Component(L"Com_Texture"));
				pTexture->Set_SelectTextureIndex(4);
			}
			if (!lstrcmp(pChild->Get_ObjectName(), TEXT("OnlyFontUI")))
			{
				static_cast<COnlyFontUI*>(pChild)->Set_FontOption(2);
			}
		}
		CPlayerController::GetInstance()->Player_HP_MPUP();
	}
}

HRESULT CNpcCanvas::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_NPC"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNpcCanvas::SetUp_ShaderResources()
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


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CNpcCanvas * CNpcCanvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNpcCanvas*		pInstance = new CNpcCanvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNpcCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNpcCanvas::Clone(void * pArg)
{
	CNpcCanvas*		pInstance = new CNpcCanvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNpcCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNpcCanvas::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);


}
