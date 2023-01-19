#include "stdafx.h"
#include "..\public\TrunWinCanvas.h"
#include "GameInstance.h"

#include "MyImage.h"
#include "Exp_Bar.h"

CTrunWinCanvas::CTrunWinCanvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CTrunWinCanvas::CTrunWinCanvas(const CTrunWinCanvas & rhs)
	: CCanvas(rhs)
{
}

HRESULT CTrunWinCanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CTrunWinCanvas::Initialize(void * pArg)
{
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	lstrcpy(CanvasDesc.m_pTextureTag, TEXT("Prototype_Component_Texture_TrunBattle_WinCanvas"));

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

HRESULT CTrunWinCanvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
#ifdef NOMODLES
	

#else
	static_cast<CExp_Bar*>(Find_UI(TEXT("ExpBar_0")))->Set_ExpBarStatus(Find_Status(TEXT("Hero_Gully")));
	static_cast<CExp_Bar*>(Find_UI(TEXT("ExpBar_1")))->Set_ExpBarStatus(Find_Status(TEXT("Hero_Alumon")));
	static_cast<CExp_Bar*>(Find_UI(TEXT("ExpBar_2")))->Set_ExpBarStatus(Find_Status(TEXT("Hero_Calibretto")));
#endif

	m_bLast_Initlize = true;

	return S_OK;
}

void CTrunWinCanvas::Tick(_double TimeDelta)
{
	Last_Initialize();

	__super::Tick(TimeDelta);


#ifdef NOMODLES


#else
	/*for (auto &pChild : m_ChildrenVec)
	{
		if (nullptr == dynamic_cast<CMyImage*>(pChild))
			continue;;

		static_cast<CMyImage*>(pChild)->Move_Lose_Texture(TimeDelta);
	}*/
#endif	
}

void CTrunWinCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTrunWinCanvas::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render(); 

	return S_OK;
}

void CTrunWinCanvas::Exp_Set_Status(map<const wstring, CStatus*>& pStatusMap)
{
	for (auto& pChild : m_ChildrenVec)
	{
		if (dynamic_cast<CExp_Bar*>(pChild) == nullptr)
			continue;
		
		CStatus* pStatus = nullptr;
		
		for (auto StatusValue : pStatusMap)
		{
			if (StatusValue.first == TEXT("Hero_Gully"))
			{
				pStatus = StatusValue.second;
				m_StatusMap.emplace(TEXT("Hero_Gully"), pStatus);
			}
			else if (StatusValue.first == TEXT("Hero_Alumon"))
			{
				pStatus = StatusValue.second;
				m_StatusMap.emplace(TEXT("Hero_Alumon"), pStatus);
			}
			else if (StatusValue.first == TEXT("Hero_Calibretto"))
			{
				pStatus = StatusValue.second;
				m_StatusMap.emplace(TEXT("Hero_Calibretto"), pStatus);
			}
			else
				continue;
		}
	}
	_bool b = false;
}

void CTrunWinCanvas::Set_RenderActive(_bool bTrue)
{
}

HRESULT CTrunWinCanvas::SetUp_Components()
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

HRESULT CTrunWinCanvas::SetUp_ShaderResources()
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

CUI * CTrunWinCanvas::Find_UI(const _tchar * pNameTag)
{
	for (auto pChild : m_ChildrenVec)
	{
		if (!lstrcmp(pChild->Get_ObjectName(), pNameTag))
			return pChild;
	}

	return nullptr;
}

CStatus * CTrunWinCanvas::Find_Status(const wstring & pNameTag)
{
	auto Pair = find_if(m_StatusMap.begin(), m_StatusMap.end(), [&](auto MyPair)->bool
	{
		if (MyPair.first == pNameTag)
			return true;
		return	false;
	});

	if (Pair == m_StatusMap.end())
		assert(!"CHpMpBuffCanvas_Find_CurActor_issue");

	return Pair->second;
}

CTrunWinCanvas * CTrunWinCanvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrunWinCanvas*		pInstance = new CTrunWinCanvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTrunWinCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrunWinCanvas::Clone(void * pArg)
{
	CTrunWinCanvas*		pInstance = new CTrunWinCanvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTrunWinCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrunWinCanvas::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);


}