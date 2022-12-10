#include "stdafx.h"
#include "..\public\TurnUICanvas.h"
#include "GameInstance.h"

#include "Hero_Knolan.h"
#include "TurnCharcterUI.h"

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
	CGameObject* pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Gully"));

	
	dynamic_cast<CHero_Knolan*>(pGameObject)->m_Hero_GullyHPDelegater.bind(this, &CTurnUICanvas::ChildrenMoveCheck);
	dynamic_cast<CHero_Knolan*>(pGameObject)->m_Hero_GullyTestShakingDelegater.bind(this, &CTurnUICanvas::ChildrenShakingCheck);

	SetUp_ChildrenPosition();
	SetUp_MatchingOption();
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

	if (pGameInstance->Key_Down(DIK_M))
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

HRESULT CTurnUICanvas::SetUp_ChildrenPosition()
{
	CUI* pTopUI = nullptr;
	CUI* pBottomUI = nullptr;
	
	_float fTopY = 0.f;
	_float fBottomY = 0.f;

	pTopUI = CClient_Manager::Get_MaxValue_Pointer(m_ChildrenVec, fTopY, COMPARE_UI_POS_Y);
	pBottomUI = CClient_Manager::Get_SmallValue_Pointer(m_ChildrenVec, fBottomY, COMPARE_UI_POS_Y);

	if (pTopUI == nullptr || nullptr == pBottomUI)
		assert("CTurnUICanvas::SetUp_ChildrenPosition");

	Safe_AddRef(pTopUI);
	Safe_AddRef(pBottomUI);

	for (auto& pUI : m_ChildrenVec)
	{
		dynamic_cast<CTurnCharcterUI*>(pUI)->Set_Top_BottomYPos(fTopY, fBottomY);
	}

	Safe_Release(pTopUI);
	Safe_Release(pBottomUI);

	return S_OK;
}

HRESULT CTurnUICanvas::SetUp_MatchingOption()
{
	/* 나중에 삭제할때를 위해서 일단 UI들당 각자에 맞는 캐릭터들에 매칭시켜야됌*/
	return S_OK;
}

void CTurnUICanvas::Move_Children()
{
	CUI*					pTopUI = nullptr;
	_float					fTopY = 0.f;

	pTopUI = CClient_Manager::Get_MaxValue_Pointer(m_ChildrenVec, fTopY, COMPARE_UI_POS_Y);
	
	if (nullptr == pTopUI)
		assert("Move_Children");


	for (auto &iter : m_ChildrenVec)
	{
		if (iter == nullptr)
			continue;
		if (iter == pTopUI)
			dynamic_cast<CTurnCharcterUI*>(iter)->MoveControl(0);
		else
			dynamic_cast<CTurnCharcterUI*>(iter)->MoveControl(1);
	}

}

void CTurnUICanvas::ChildrenMoveCheck(_double TimeDelta, _uint iMoveSpeed)
{
	Move_Children();
}

void CTurnUICanvas::ChildrenShakingCheck(_uint iShakingTime)
{
	for (auto &pUI : m_ChildrenVec)
	{
		dynamic_cast<CTurnCharcterUI*>(pUI)->ShakingControl(iShakingTime);
	}

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
		
		dynamic_cast<CTurnCharcterUI*>(m_ChildrenVec[iNumber-1])->RightMove();
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
