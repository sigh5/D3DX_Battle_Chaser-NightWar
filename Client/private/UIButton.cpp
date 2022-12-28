#include "stdafx.h"
#include "..\public\UIButton.h"

#include "GameInstance.h"

CUIButton::CUIButton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUIButton::CUIButton(const CUIButton & rhs)
	: CUI(rhs)
{
}

void CUIButton::Set_ButtonImage(CUI * pUI)
{
	m_pButtonImage = pUI;
	pUI->Set_parent(this);

}

void CUIButton::Set_ChildFsmButton(CUIButton * pNodeButton)
{
	m_ChildFsmButton.push_back(pNodeButton);
}



HRESULT CUIButton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton::Initialize(void * pArg)
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

HRESULT CUIButton::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	if (!lstrcmp(m_ObjectName, TEXT("UIButton3")))
	{
		FirstDungeonUIInit();
		m_bRenderActive = true;
	}

	Button_RenderSetActive(m_bRenderActive);


	m_bLast_Initlize = true;
	return S_OK;
}

void CUIButton::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	Click_This_Button();
}

void CUIButton::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUIButton::Render()
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

void CUIButton::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
	Safe_Release(m_pTextureCom);
	Remove_component(TEXT("Com_Texture"));

	lstrcpy(m_UIDesc.m_pTextureTag, NewComPonentTag.c_str());

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iLevel, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		assert("CUIButton Change_Texture");
}

void CUIButton::Set_HighRightUIDesc(HIGHLIGHT_UIDESC & HighLightDesc)
{
	_float4	 vPos;

	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.y = HighLightDesc.yPos;
	
	m_pTransformCom->Set_Scaled(_float3(HighLightDesc.iNumSizeX, HighLightDesc.iNumSizeY,0.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
	m_pTextureCom->Set_SelectTextureIndex(HighLightDesc.iTextureIndex);
}

void CUIButton::FirstDungeonUIInit()
{
	_float4	 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.y = 5.f;

	m_pTransformCom->Set_Scaled(_float3(133.f, 134.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
	m_pTextureCom->Set_SelectTextureIndex(8);
}

void	CUIButton::State_Image_Change(BUTTON_STATE eType)
{
	// 이미지랑 현재 상태만 설정한다.
	size_t ChildFsmButtonSize = m_ChildFsmButton.size();

	if (ChildFsmButtonSize == 0)
		return ;

	for (size_t i = 0; i < ChildFsmButtonSize; ++i)
	{
		m_ChildFsmButton[i]->Set_RenderActive(true);
		
		if(eType == BUTTON_STATE_ACTION)
			 m_ChildFsmButton[i]->Change_ICON_Active();
		else if (eType == BUTTON_STATE_ABLILTY)
			m_ChildFsmButton[i]->Change_ICON_Ablity();
		else if (eType == BUTTON_STATE_ITEM)
			m_ChildFsmButton[i]->Change_ICON_Item();
	}


}

void CUIButton::Change_ICON_Active()
{
	// 이미지랑 현재 상태만 설정한다.
	CTexture* pTexture = static_cast<CTexture*>(m_pButtonImage->Get_Component(TEXT("Com_Texture")));

	if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button0")))
	{
		pTexture->Set_SelectTextureIndex(0);
		m_eFsmState = BUTTON_FSM_NORMALATTACK;
	}
	else if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button1")))
	{
		pTexture->Set_SelectTextureIndex(1);
		m_eFsmState = BUTTON_FSM_DEFENCE;
	}
	else
		Set_RenderActive(false);;

}

void CUIButton::Change_ICON_Ablity()
{
	// 이미지랑 현재 상태만 설정한다.
	CTexture* pTexture = static_cast<CTexture*>(m_pButtonImage->Get_Component(TEXT("Com_Texture")));

	if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button0")))
	{
		pTexture->Set_SelectTextureIndex(2);
		m_eFsmState = BUTTON_FSM_SKILL1;
	}
	else if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button1")))
	{
		pTexture->Set_SelectTextureIndex(3);
		m_eFsmState = BUTTON_FSM_SKILL2;
	}
	else if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button2")))
	{
		pTexture->Set_SelectTextureIndex(4);
		m_eFsmState = BUTTON_FSM_ULTIMATE;
	}
	else if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button3")))
	{
		pTexture->Set_SelectTextureIndex(5);
		m_eFsmState = BUTTON_FSM_BUFF;
	}
	else if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button4")))
	{
		pTexture->Set_SelectTextureIndex(6);
		m_eFsmState = BUTTON_FSM_WIDEBUFF;
	}
	else
		return;


	// 아마여기에 많이 추개해서 버프랑 와이드 버프 넣어야함
}

void CUIButton::Change_ICON_Item()
{
	// 이미지랑 현재 상태만 설정한다.
	CTexture* pTexture = static_cast<CTexture*>(m_pButtonImage->Get_Component(TEXT("Com_Texture")));
	if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button0")))
	{
		pTexture->Set_SelectTextureIndex(7);
		m_eFsmState = BUTTON_FSM_USE_HP_ITEM;		// 체력
	}
	else if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button1")))
	{
		pTexture->Set_SelectTextureIndex(8);
		m_eFsmState = BUTTON_FSM_USE_MP_ITEM;		// 마나

	}
	else if (!lstrcmp(m_ObjectName, TEXT("UI_State_Action_Button2")))
	{						//도망
		pTexture->Set_SelectTextureIndex(9);
		m_eFsmState = BUTTON_FSM_FLEE;
	}
	else
		Set_RenderActive(false);;
}

_bool CUIButton::Click_This_Button()
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

void CUIButton::Shaking()
{
	
	//_float4		vPos;

	//XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	//m_ShakeTimer += _float(CClient_Manager::TimeDelta * 1.f);

	//if (m_ShakeTimer >= m_fCoolTime)
	//{
	//	m_ShakeTimer = 0.f;
	//	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vOriginPos));
	//}

	//vPos.y += 1.f*  400.f * CClient_Manager::TimeDelta;

	//m_iSwitching *= -1;

	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
}

void CUIButton::Button_RenderSetActive(_bool bRenderActive)
{
	if (m_pButtonImage != nullptr)
		m_pButtonImage->Set_RenderActive(bRenderActive);

}

HRESULT CUIButton::SetUp_Components()
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

HRESULT CUIButton::SetUp_ShaderResources()
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

CUIButton * CUIButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUIButton*		pInstance = new CUIButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTurnCharcterUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUIButton::Clone(void * pArg)
{
	CUIButton*		pInstance = new CUIButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTurnCharcterUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIButton::Set_ParentLoad(CUI * pUI)
{
	Set_ButtonImage(pUI);
}

void CUIButton::Set_RenderActive(_bool bActive)
{
	m_bRenderActive = bActive;

	if (nullptr != m_pButtonImage)
		m_pButtonImage->Set_RenderActive(bActive);
	else
		return;
}

void CUIButton::Change_ButtonIcon(const wstring & TextureTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pButtonImage->Change_Texture(LEVEL_GAMEPLAY, TextureTag);	//GamePlayerLoader

	RELEASE_INSTANCE(CGameInstance);
}

void CUIButton::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
