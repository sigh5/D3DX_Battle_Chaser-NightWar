#include "stdafx.h"
#include "..\public\TurnStateCanvas.h"

#include "GameInstance.h"
#include "UIButton.h"

#include "Hero_Knolan.h"
#include "Hero_Calibretto.h"
#include "Hero_Garrison.h"



CTurnStateCanvas::CTurnStateCanvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CTurnStateCanvas::CTurnStateCanvas(const CTurnStateCanvas & rhs)
	: CCanvas(rhs)
{
}

void CTurnStateCanvas::Set_RenderActive(_bool bActive)
{
	if (CClient_Manager::m_bCombatWin == true)
		Control_ChildRender(false);
	else
		Control_ChildRender(bActive);
}

HRESULT CTurnStateCanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CTurnStateCanvas::Initialize(void * pArg)
{
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	if (nullptr != pArg)
		memcpy(&CanvasDesc, pArg, sizeof(CanvasDesc));

	if (FAILED(__super::Initialize(&CanvasDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	m_fSizeX = (_float)g_iWinSizeX/1.5f ;
	m_fSizeY = (_float)g_iWinSizeY/4.f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f , -m_fY + g_iWinSizeY * 0.5f , 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());


	return S_OK;
}

HRESULT CTurnStateCanvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
#ifdef NOMODLES

#else
	CGameObject* pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Gully"));
	dynamic_cast<CHero_Knolan*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.bind(this, &CTurnStateCanvas::Set_RenderActive);

	pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Calibretto"));
	dynamic_cast<CHero_Calibretto*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.bind(this, &CTurnStateCanvas::Set_RenderActive);

	pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Alumon"));
	dynamic_cast<CHero_Garrison*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.bind(this, &CTurnStateCanvas::Set_RenderActive);

	Control_ChildRender(false);
	StateButton_Child();
#endif

	RELEASE_INSTANCE(CGameInstance);

	m_bLast_Initlize = true;

	return S_OK;
}

void CTurnStateCanvas::Tick(_double TimeDelta)
{

	Last_Initialize();
	__super::Tick(TimeDelta);

	PickingChild();
}

void CTurnStateCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTurnStateCanvas::Render()
{
	
	//if (FAILED(__super::Render()))
	//	return E_FAIL;

	//if (FAILED(SetUp_ShaderResources()))
	//	return E_FAIL;


	//m_pShaderCom->Begin(1);		// UI 1번 알파블랜딩
	//m_pVIBufferCom->Render();


	return S_OK;
}

void CTurnStateCanvas::PickingChild()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_bool	bIsPicking = false;

	if((pGameInstance->Mouse_Down(CInput_Device::DIM_LB)))
	{ 
		for (auto& pChild : m_ChildrenVec)
		{
			if (nullptr == dynamic_cast<CUIButton*>(pChild))
				continue;

			if (static_cast<CUIButton*>(pChild)->Click_This_Button())
			{
				pCurPickingButton = pChild;
				bIsPicking = true;
				break;
			}
		}
		
		if (bIsPicking)
		{
			auto Pair = find_if(m_pCurState.begin(), m_pCurState.end(), [&](auto MyPair)->bool
			{
				return !lstrcmp(MyPair.first.c_str(), pCurPickingButton->Get_ObjectName());
			});

			if (Pair != m_pCurState.end())
			{
				pCurPickingState = Pair->second;
				pCurPickingButton = nullptr;
			}
			
			CurState_Image_Change();
			CurState_Fsm_ButtonClick();
		}
	}


	RELEASE_INSTANCE(CGameInstance);
}

void CTurnStateCanvas::CurState_Image_Change()
{
	if (nullptr == pCurPickingState)
		return;

	if (!lstrcmp(pCurPickingState->Get_ObjectName(), STATE_ACTION))
	{
		m_StateType = BUTTON_STATE_ACTION;
		static_cast<CUIButton*>(pCurPickingState)->State_Image_Change(BUTTON_STATE_ACTION);
	}
	else if (!lstrcmp(pCurPickingState->Get_ObjectName(), STATE_ABLILTY))
	{
		m_StateType = BUTTON_STATE_ABLILTY;
		static_cast<CUIButton*>(pCurPickingState)->State_Image_Change(BUTTON_STATE_ABLILTY);
	}
	else if (!lstrcmp(pCurPickingState->Get_ObjectName(), STATE_ITEM))
	{
		m_StateType = BUTTON_STATE_ITEM;
		static_cast<CUIButton*>(pCurPickingState)->State_Image_Change(BUTTON_STATE_ITEM);
	}
	else
		return;
}

void CTurnStateCanvas::CurState_Fsm_ButtonClick()
{
	if (nullptr == pCurPickingState || nullptr == pCurPickingButton)
		return;

	m_ButtonFsmType = static_cast<CUIButton*>(pCurPickingButton)->Get_ButtonFsmState();
}

void CTurnStateCanvas::CurState_Fsm_ButtonICon(const wstring& NameTag)
{

	wstring CurTexturTag = L"";
	if (NameTag == TEXT("Hero_Alumon"))
		CurTexturTag = TEXT("Prototype_Component_Texture_UI_State_Icon_Garrison");
	else if (NameTag == TEXT("Hero_Calibretto"))
		CurTexturTag = TEXT("Prototype_Component_Texture_UI_State_Icon_Calibretto");
	else if (NameTag == TEXT("Hero_Gully"))
		CurTexturTag = TEXT("Prototype_Component_Texture_UI_State_Icon_Knolan");
	else
		return;		// 몬스터가 현재 턴일때

	for (auto& pFsmButton : m_pCurFsmButton)
	{
		static_cast<CUIButton*>(pFsmButton)->Change_ButtonIcon(CurTexturTag);
	}
}

void CTurnStateCanvas::Player_SceneChane(_bool bEvent)
{
	for (auto& pChild : m_ChildrenVec)
	{
		pChild->Set_RenderActive(bEvent);
	}

}

void CTurnStateCanvas::Control_ChildRender(_bool bRenderActive)
{

	for (auto& pChild : m_ChildrenVec)
	{
		/*if (dynamic_cast<CUIButton*>(pChild) != nullptr)
			continue;*/
		pChild->Set_RenderActive(bRenderActive);
	}
	
	for (auto& pState : m_pCurState)
	{
		pState.second->Set_RenderActive(bRenderActive);
	}


	for (auto& pFsmBtn : m_pCurFsmButton)
	{
		pFsmBtn->Set_RenderActive(false);
	}

	


}

void CTurnStateCanvas::StateButton_Child()
{
	for (auto& pChild : m_ChildrenVec)
	{
		if (!lstrcmp(pChild->Get_ObjectName(), TEXT("UI_State_Action")))
		{
			m_pCurState.emplace(TEXT("UI_State_Action"), pChild);
			static_cast<CUIButton*>(pChild)->Set_FontDesc(TEXT("Normal"), _float2(560.f, 575.f), _float2(0.5f, 0.5f), _float4(1.f, 1.f, 1.f, 1.f));
		}
		else if (!lstrcmp(pChild->Get_ObjectName(), TEXT("UI_State_Ablity")))
		{
			m_pCurState.emplace(TEXT("UI_State_Ablity"), pChild);
			static_cast<CUIButton*>(pChild)->Set_FontDesc(TEXT("Skill"), _float2(560.f, 625.f), _float2(0.5f, 0.5f), _float4(0.f, 0.f, 1.f, 1.f));
		}
		else if (!lstrcmp(pChild->Get_ObjectName(), TEXT("UI_State_Item")))
		{
			m_pCurState.emplace(TEXT("UI_State_Item"), pChild);
			static_cast<CUIButton*>(pChild)->Set_FontDesc(TEXT("Utills"), _float2(560.f, 675.f), _float2(0.5f, 0.5f), _float4(1.f, 0.f, 0.f, 1.f));
		}
		else if (nullptr != dynamic_cast<CUIButton*>(pChild))
		{
			m_pCurFsmButton.push_back(pChild);
		}
		else
			continue;
	}

	for (auto& pStateButton : m_pCurState)
	{
		for (auto& pCurFsmButton : m_pCurFsmButton)
		{
			static_cast<CUIButton*>(pStateButton.second)->Set_ChildFsmButton(static_cast<CUIButton*>(pCurFsmButton));
		}
	}

}

void CTurnStateCanvas::Delete_Delegate()
{
	//CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	//CGameObject* pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Gully"));
	//dynamic_cast<CHero_Knolan*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.unbind(this);

	//pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Calibretto"));
	//dynamic_cast<CHero_Calibretto*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.unbind(this);

	//pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Alumon"));
	//dynamic_cast<CHero_Garrison*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.unbind(this);

	//RELEASE_INSTANCE(CGameInstance);
}

HRESULT CTurnStateCanvas::SetUp_Components()
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

HRESULT CTurnStateCanvas::SetUp_ShaderResources()
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

CTurnStateCanvas * CTurnStateCanvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTurnStateCanvas*		pInstance = new CTurnStateCanvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTurnStateCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTurnStateCanvas::Clone(void * pArg)
{
	CTurnStateCanvas*		pInstance = new CTurnStateCanvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTurnStateCanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTurnStateCanvas::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

