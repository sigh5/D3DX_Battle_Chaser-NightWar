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

	CGameObject* pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Gully"));
	dynamic_cast<CHero_Knolan*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.bind(this, &CTurnStateCanvas::Set_RenderActive);

	pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Calibretto"));
	dynamic_cast<CHero_Calibretto*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.bind(this, &CTurnStateCanvas::Set_RenderActive);

	pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Alumon"));
	dynamic_cast<CHero_Garrison*>(pGameObject)->m_Hero_CombatStateCanvasDelegeter.bind(this, &CTurnStateCanvas::Set_RenderActive);

	Control_ChildRender(false);
	StateButton_Child();

	RELEASE_INSTANCE(CGameInstance);

	m_bLast_Initlize = true;

	return S_OK;
}

void CTurnStateCanvas::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


	if (ImGui::IsMouseClicked(1))			// �÷��̾� ���ʶ� ���� �����ؾ��Ѵ�.
	{
		Control_ChildRender(true);
	}


	PickingChild();
	//CurState_Image_Change();

}

void CTurnStateCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTurnStateCanvas::Render()
{
	/* �������Ұ���*/
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
}

void CTurnStateCanvas::CurState_Fsm_ButtonClick()
{
	if (nullptr == pCurPickingState || nullptr == pCurPickingButton)
		return;

	m_ButtonFsmType = static_cast<CUIButton*>(pCurPickingButton)->Get_ButtonFsmState();


}

void CTurnStateCanvas::Control_ChildRender(_bool bRenderActive)
{
	for (auto& pChild : m_ChildrenVec)
	{
		pChild->Set_RenderActive(bRenderActive);
	}
}

void CTurnStateCanvas::StateButton_Child()
{
	for (auto& pChild : m_ChildrenVec)
	{
		if (!lstrcmp(pChild->Get_ObjectName(), TEXT("UI_State_Action")))
		{
			m_pCurState.emplace(TEXT("UI_State_Action"), pChild);
		}
		else if (!lstrcmp(pChild->Get_ObjectName(), TEXT("UI_State_Ablity")))
		{
			m_pCurState.emplace(TEXT("UI_State_Ablity"), pChild);
		}
		else if (!lstrcmp(pChild->Get_ObjectName(), TEXT("UI_State_Item")))
		{
			m_pCurState.emplace(TEXT("UI_State_Item"), pChild);
		}
		else if (nullptr != dynamic_cast<CUIButton*>(pChild))
		{
			m_pCurFsmButton.push_back(pChild);
		}
	}

	size_t iFsmButtonSize = m_pCurFsmButton.size();

	for (auto& pStateButton : m_pCurState)
	{
		for (size_t i =0; i<iFsmButtonSize; ++i)
		{
			static_cast<CUIButton*>(pStateButton.second)->Set_ChildFsmButton(static_cast<CUIButton*>(m_pCurFsmButton[i]));
		}

	}


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