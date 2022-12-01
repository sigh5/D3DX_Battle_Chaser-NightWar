#include "stdafx.h"
#include "..\public\TurnUICanvas.h"
#include "GameInstance.h"

#include "Hero_Gully.h"
#include "TurnCharcterUI.h"
#include <random>
CTurnUICanvas::CTurnUICanvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CTurnUICanvas::CTurnUICanvas(const CTurnUICanvas & rhs)
	: CCanvas(rhs)
{
}

HRESULT CTurnUICanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CTurnUICanvas::Initialize(void * pArg)
{
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	if (nullptr != pArg)
		memcpy(&CanvasDesc, pArg, sizeof(CanvasDesc));


	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(__super::Initialize(&CanvasDesc)))
		return E_FAIL;


	m_fSizeX = (_float)g_iWinSizeX / 11;
	m_fSizeY = (_float)g_iWinSizeY * 1.1f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - m_fSizeX * 0.5f - (585.f), -m_fY + m_fSizeY * 0.5f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	if (FAILED(CUI::SetUp_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTurnUICanvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = pInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Hero_Gully"));

	dynamic_cast<CHero_Gully*>(pGameObject)->m_Hero_GullyHPDelegater.bind(this, &CTurnUICanvas::ChildrenMoveCheck);

	RELEASE_INSTANCE(CGameInstance);

	m_bLast_Initlize = true;

	return S_OK;
}

void CTurnUICanvas::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


	// Test
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_0))
	{
		DeleteCharUI(TEXT("UI_Trun_Garrison1"));
	}

	RELEASE_INSTANCE(CGameInstance);

}

void CTurnUICanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTurnUICanvas::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	CUI::Begin_UI();

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();

	CUI::End_UI();

	return S_OK;

}

HRESULT CTurnUICanvas::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_UI_Turn_canvas"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTurnUICanvas::SetUp_ShaderResources()
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

void CTurnUICanvas::Move_Children()
{
	CUI*					m_pCharUI[TURN_UI_END] = { nullptr , nullptr };
	_bool	bIsTop = false;

	for (auto &pUI : m_ChildrenVec)
	{
		if (pUI == nullptr)
			continue;

		if (dynamic_cast<CTurnCharcterUI*>(pUI)->isUITop(m_pCharUI[TURN_UI_TOP]))
		{
			bIsTop = true;
			break;
		}
	}

	for (auto &iter : m_ChildrenVec)
	{
		if (iter == nullptr)
			continue;

		if (iter != m_pCharUI[TURN_UI_TOP])
			dynamic_cast<CTurnCharcterUI*>(iter)->MoveControl(1);
		else
			dynamic_cast<CTurnCharcterUI*>(iter)->MoveControl(0);
	}

}

void CTurnUICanvas::ChildrenMoveCheck(_double TimeDelta, _uint iMoveSpeed)
{
	Move_Children();
}
 
void CTurnUICanvas::DeleteCharUI(const wstring&  pNametag)
{
	/* 나중에 죽었을떄 이 이벤트를 받아야됌*/
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	_float Temp = 0;

	CUI*	pUI = nullptr;

	int  iRandomNum =	rand()	%(m_ChildrenVec.size()-2);

	for (auto iter = m_ChildrenVec.begin(); iter != m_ChildrenVec.end(); )
	{
		if (!lstrcmp((*iter)->Get_ObjectName(),pNametag.c_str()) )
		{
			Temp = dynamic_cast<CTurnCharcterUI*>(*iter)->Get_PosY();
			Safe_Release(*iter);
			pGameInstance->DeleteGameObject(LEVEL_GAMEPLAY,pNametag);
			iter = m_ChildrenVec.erase(iter);
			pUI = *(iter+iRandomNum);
		}
		else
		{
			iter++;
		}		
	}

	for (auto &pUI : m_ChildrenVec)
	{
		if (Temp > dynamic_cast<CTurnCharcterUI*>(pUI)->Get_PosY())
			dynamic_cast<CTurnCharcterUI*>(pUI)->MoveControl(1);
	}

	
	if (nullptr != pUI)
	{
		Add_ChildUI(static_cast<CUI*>(pGameInstance->Clone_UI(LEVEL_GAMEPLAY, TEXT("Layer_UI"), (pUI))));
		size_t iNumber =	m_ChildrenVec.size();
		
		dynamic_cast<CTurnCharcterUI*>(m_ChildrenVec[iNumber-1])->IsMove();
	}

	RELEASE_INSTANCE(CGameInstance);

	

}

CTurnUICanvas * CTurnUICanvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTurnUICanvas*		pInstance = new CTurnUICanvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTurnUICanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTurnUICanvas::Clone(void * pArg)
{
	CTurnUICanvas*		pInstance = new CTurnUICanvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTurnUICanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTurnUICanvas::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
