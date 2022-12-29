#include "stdafx.h"
#include "..\public\HpMpBuffCanvas.h"
#include "GameInstance.h"
#include "HpMpBar.h"

CHpMpBuffCanvas::CHpMpBuffCanvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CHpMpBuffCanvas::CHpMpBuffCanvas(const CHpMpBuffCanvas & rhs)
	: CCanvas(rhs)
{
}

void CHpMpBuffCanvas::Set_StatusHpMpBar(map<const wstring, CStatus*>& StatusMap)
{
	m_StatusMap = StatusMap;

	if (static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar0"))) != nullptr)
	{
		static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar0")))->Set_HpBarStatus(Find_Status(TEXT("Hero_Gully")));
		static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar0")))->Set_MpBarStatus(Find_Status(TEXT("Hero_Gully")));

		static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar1")))->Set_HpBarStatus(Find_Status(TEXT("Hero_Alumon")));
		static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar1")))->Set_MpBarStatus(Find_Status(TEXT("Hero_Alumon")));

		static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar2")))->Set_HpBarStatus(Find_Status(TEXT("Hero_Calibretto")));
		static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar2")))->Set_MpBarStatus(Find_Status(TEXT("Hero_Calibretto")));
	}

	if(static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar3"))) != nullptr)
	{
		static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar3")))->Set_HpBarStatus(Find_Status(TEXT("Skeleton_Naked")));
		static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar3")))->Set_MpBarStatus(Find_Status(TEXT("Skeleton_Naked")));

		static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar4")))->Set_HpBarStatus(Find_Status(TEXT("Monster_SlimeKing")));
		static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar4")))->Set_MpBarStatus(Find_Status(TEXT("Monster_SlimeKing")));

		static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar5")))->Set_HpBarStatus(Find_Status(TEXT("Spider_Mana")));
		static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar5")))->Set_MpBarStatus(Find_Status(TEXT("Spider_Mana")));
	}

	m_StatusMap.clear();
}

void CHpMpBuffCanvas::Set_HitEvent(CGameObject* pHiter,_bool bHit)
{
	for (auto& pChild : m_ChildrenVec)
	{
		if(dynamic_cast<CHpMpBar*>(pChild) ==nullptr )
			continue;
		
		if (static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar0"))) != nullptr)
		{
			if (!lstrcmp(pHiter->Get_ObjectName(), TEXT("Hero_Gully")))
			{
				static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar0")))->Set_Hit(bHit);
			}
			else if (!lstrcmp(pHiter->Get_ObjectName(), TEXT("Hero_Alumon")))
			{
				static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar1")))->Set_Hit(bHit);
			}
			else if (!lstrcmp(pHiter->Get_ObjectName(), TEXT("Hero_Calibretto")))
			{
				static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar2")))->Set_Hit(bHit);
			}
			else
				continue;
		}
	

		else if (static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar3"))) != nullptr)
		{
			if (!lstrcmp(pHiter->Get_ObjectName(), TEXT("Skeleton_Naked")))
			{
				static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar3")))->Set_Hit(bHit);
			}
			else if (!lstrcmp(pHiter->Get_ObjectName(), TEXT("Monster_SlimeKing")))
			{
				static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar4")))->Set_Hit(bHit);
			}
			else if (!lstrcmp(pHiter->Get_ObjectName(), TEXT("Spider_Mana")))
			{
				static_cast<CHpMpBar*>(Find_UI(TEXT("HpBar5")))->Set_Hit(bHit);
			}
			else
				continue;
		}
		else
			continue;
	}
}

void CHpMpBuffCanvas::Set_MpEvent(_bool bMp)
{
	for (auto& pChild : m_ChildrenVec)
	{
		if (dynamic_cast<CHpMpBar*>(pChild) == nullptr)
			continue;

		if (static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar0"))) != nullptr)
		{
			static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar0")))->Set_Hit(bMp);
			static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar1")))->Set_Hit(bMp);
			static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar2")))->Set_Hit(bMp);
		}


		else if (static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar3"))) != nullptr)
		{
			static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar3")))->Set_Hit(bMp);
			static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar4")))->Set_Hit(bMp);
			static_cast<CHpMpBar*>(Find_UI(TEXT("MpBar5")))->Set_Hit(bMp);
		}
		else
			continue;
	}
}

HRESULT CHpMpBuffCanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpMpBuffCanvas::Initialize(void * pArg)
{
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	if (nullptr != pArg)
		memcpy(&CanvasDesc, pArg, sizeof(CanvasDesc));

	if (FAILED(__super::Initialize(&CanvasDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	m_fSizeX = (_float)g_iWinSizeX / 1.5f;
	m_fSizeY = (_float)g_iWinSizeY / 4.f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	

	return S_OK;
}

HRESULT CHpMpBuffCanvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	for (auto& pChild : m_ChildrenVec)
	{
		pChild->Set_RenderActive(false);
	}

	RELEASE_INSTANCE(CGameInstance);
	m_bLast_Initlize = true;
	return S_OK;
}

void CHpMpBuffCanvas::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

}

void CHpMpBuffCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHpMpBuffCanvas::Render()
{
	//if (FAILED(__super::Render()))
	//	return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//	return E_FAIL;

	//m_pShaderCom->Begin(1);		// UI 1번 알파블랜딩
	//m_pVIBufferCom->Render();
	return S_OK;
}

void CHpMpBuffCanvas::Set_RenderActive(_bool bTrue)
{
	for (auto& pChild : m_ChildrenVec)
	{
		pChild->Set_RenderActive(bTrue);
	}
}



HRESULT CHpMpBuffCanvas::SetUp_Components()
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


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_CanvasDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CHpMpBuffCanvas::SetUp_ShaderResources()
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

CStatus * CHpMpBuffCanvas::Find_Status(const wstring & pNameTag)
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

CUI * CHpMpBuffCanvas::Find_UI(const _tchar* pNameTag)
{
	for (auto pChild : m_ChildrenVec)
	{
		if (!lstrcmp(pChild->Get_ObjectName(), pNameTag))
			return pChild;
	}

	return nullptr;
}

CHpMpBuffCanvas * CHpMpBuffCanvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHpMpBuffCanvas*		pInstance = new CHpMpBuffCanvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHpMpBuffCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHpMpBuffCanvas::Clone(void * pArg)
{
	CHpMpBuffCanvas*		pInstance = new CHpMpBuffCanvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTurnStateCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHpMpBuffCanvas::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}