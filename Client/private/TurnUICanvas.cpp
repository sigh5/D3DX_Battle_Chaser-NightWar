#include "stdafx.h"
#include "..\public\TurnUICanvas.h"
#include "GameInstance.h"

#include "CombatController.h"
#include "Hero_Knolan.h"
#include "Hero_Calibretto.h"
#include "Hero_Garrison.h"

#include "Spider_Mana.h"
#include "SlimeKing.h"
#include "Skeleton_Naked.h"


#include "TurnCharcterUI.h"

CTurnUICanvas::CTurnUICanvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CTurnUICanvas::CTurnUICanvas(const CTurnUICanvas & rhs)
	: CCanvas(rhs)
{
}

const UI_REPRESENT CTurnUICanvas::Get_CurrentActor() const
{
	assert(m_pTopUI != nullptr&&"CTurnUICanvas::Get_CurrentActor()");

	return static_cast<CTurnCharcterUI*>(m_pTopUI)->Get_Represent_Char();
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

	lstrcpy(CanvasDesc.m_pTextureTag , TEXT("Prototype_Component_UI_Turn_canvas"));

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

	return S_OK;
}

HRESULT CTurnUICanvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
#ifdef NOMODLES


#else
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Gully"));
	dynamic_cast<CHero_Knolan*>(pGameObject)->m_Hero_CombatTurnDelegeter.bind(this, &CTurnUICanvas::ChildrenMoveCheck);

	pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Calibretto"));
	dynamic_cast<CHero_Calibretto*>(pGameObject)->m_Hero_CombatTurnDelegeter.bind(this, &CTurnUICanvas::ChildrenMoveCheck);

	pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Alumon"));
	dynamic_cast<CHero_Garrison*>(pGameObject)->m_Hero_CombatTurnDelegeter.bind(this, &CTurnUICanvas::ChildrenMoveCheck);

	pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Monster"), TEXT("Skeleton_Naked"));
	dynamic_cast<CSkeleton_Naked*>(pGameObject)->m_Monster_CombatTurnDelegeter.bind(this, &CTurnUICanvas::ChildrenMoveCheck);

	if (pInstance->Get_Setting_MonsterScene() == 2 || pInstance->Get_Setting_MonsterScene() == 1)
	{
		pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Monster"), TEXT("Spider_Mana"));
		dynamic_cast<CSpider_Mana*>(pGameObject)->m_Monster_CombatTurnDelegeter.bind(this, &CTurnUICanvas::ChildrenMoveCheck);

	}

	if (pInstance->Get_Setting_MonsterScene() == 3)
	{
		pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Monster"), TEXT("Monster_SlimeKing"));
		dynamic_cast<CSlimeKing*>(pGameObject)->m_Monster_CombatTurnDelegeter.bind(this, &CTurnUICanvas::ChildrenMoveCheck);
	}
	
	

	SetUp_ChildrenPosition();
	RELEASE_INSTANCE(CGameInstance);

#endif

	m_bLast_Initlize = true;

	return S_OK;
}

void CTurnUICanvas::Tick(_double TimeDelta)
{

	Last_Initialize();

	__super::Tick(TimeDelta);

	/*static int i = 0;
	static UI_REPRESENT TempNum = REPRESENT_END;

	ImGui::InputInt("RepresentNum", &i);

	if(ImGui::Button("TestDelete"))
	{
		DeleteCharUI(UI_REPRESENT(i));
	}*/


}

void CTurnUICanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTurnUICanvas::Render()
{
#ifdef NOMODLES

#else
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();
#endif
	return S_OK;

}

void CTurnUICanvas::Delete_Delegate()
{
	//CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	//CGameObject* pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Gully"));
	//dynamic_cast<CHero_Knolan*>(pGameObject)->m_Hero_CombatTurnDelegeter.unbind(this);

	//pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Calibretto"));
	//dynamic_cast<CHero_Calibretto*>(pGameObject)->m_Hero_CombatTurnDelegeter.unbind(this);

	//pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Player"), TEXT("Hero_Alumon"));
	//dynamic_cast<CHero_Garrison*>(pGameObject)->m_Hero_CombatTurnDelegeter.unbind(this);

	//pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Monster"), TEXT("Skeleton_Naked"));
	//dynamic_cast<CSkeleton_Naked*>(pGameObject)->m_Monster_CombatTurnDelegeter.unbind(this);

	//pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Monster"), TEXT("Spider_Mana"));
	//dynamic_cast<CSpider_Mana*>(pGameObject)->m_Monster_CombatTurnDelegeter.unbind(this);

	//pGameObject = pInstance->Get_GameObject(pInstance->GetCurLevelIdx(), TEXT("Layer_Monster"), TEXT("Monster_SlimeKing"));
	//dynamic_cast<CSlimeKing*>(pGameObject)->m_Monster_CombatTurnDelegeter.unbind(this);

	//RELEASE_INSTANCE(CGameInstance);
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
	_float fTopY = 0.f;
	_float fBottomY = 0.f;

	m_pTopUI = CClient_Manager::Get_MaxValue_Pointer(m_ChildrenVec, fTopY, COMPARE_UI_POS_Y);
	m_pBottomUI = CClient_Manager::Get_SmallValue_Pointer(m_ChildrenVec, fBottomY, COMPARE_UI_POS_Y);

	if (m_pTopUI == nullptr || nullptr == m_pBottomUI)
		assert("CTurnUICanvas::SetUp_ChildrenPosition");

	for (auto& pUI : m_ChildrenVec)
	{
		dynamic_cast<CTurnCharcterUI*>(pUI)->Set_Top_BottomYPos(fTopY, fBottomY);
	}

	

	return S_OK;
}



void CTurnUICanvas::Move_Children()
{
	_float					fTopY = 0.f;
	m_pTopUI = CClient_Manager::Get_MaxValue_Pointer(m_ChildrenVec, fTopY, COMPARE_UI_POS_Y);
	
	if (nullptr == m_pTopUI)
		assert("Move_Children");

	for (auto &iter : m_ChildrenVec)
	{
		if (iter == nullptr)
			continue;
		if (iter == m_pTopUI)
			dynamic_cast<CTurnCharcterUI*>(iter)->MoveControl(0);
		else
			dynamic_cast<CTurnCharcterUI*>(iter)->MoveControl(1);
	}

	m_pTopUI = CClient_Manager::Get_SecondMaxValue_Pointer(m_ChildrenVec, fTopY);

	CCombatController::GetInstance()->Refresh_CurActor();

}

void CTurnUICanvas::ChildrenMoveCheck(UI_REPRESENT iRepesentNum, _uint iOpiton)
{
	if (iOpiton == 0)
		Move_Children();
	else
		DeleteCharUI(iRepesentNum);
}

void CTurnUICanvas::ChildrenShakingCheck(_uint iShakingTime)
{
	for (auto &pUI : m_ChildrenVec)
	{
		dynamic_cast<CTurnCharcterUI*>(pUI)->ShakingControl(iShakingTime);
	}

}
 
void CTurnUICanvas::DeleteCharUI(UI_REPRESENT UiRepresentNum)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	vector<CUI*> OldCharImage;
	
	for (auto iter = m_ChildrenVec.begin(); iter != m_ChildrenVec.end(); )
	{
		if (nullptr == dynamic_cast<CTurnCharcterUI*>(*iter))
			continue;

		if (static_cast<CTurnCharcterUI*>(*iter)->Get_Represent_Char() == UiRepresentNum)
		{
			OldCharImage.push_back(*iter);
			Safe_Release(*iter);
			iter = m_ChildrenVec.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	size_t OldCharVecSize = OldCharImage.size();
	for (auto iter = m_ChildrenVec.begin(); iter != m_ChildrenVec.end();)
	{
		_int iLimitCount = 0;
		for (size_t i = 0; i < OldCharVecSize; ++i)
		{
			if (static_cast<CTurnCharcterUI*>(OldCharImage[i])->Get_PosY() > 
				static_cast<CTurnCharcterUI*>(*iter)->Get_PosY())
			{
				++iLimitCount;
			}
			static_cast<CTurnCharcterUI*>(*iter)->MoveControl(CTurnCharcterUI::UI_POS_QUICK);
			static_cast<CTurnCharcterUI*>(*iter)->Set_LimitYPos(iLimitCount);
		}
		++iter;
	}

	_float fBackLimitYPos = -250.f;

	_float fSmallY = 0.f;
	_uint iRandomNum = 999;
	for (size_t i = 0; i < OldCharVecSize; ++i)
	{
		_uint iTemp = rand() % m_ChildrenVec.size();
		while (iRandomNum != iTemp)
		{
			iTemp = rand() % m_ChildrenVec.size();
			iRandomNum = iTemp;
		}
		CTurnCharcterUI* pNewUI = static_cast<CTurnCharcterUI*>(pGameInstance->
			Clone_UI(pGameInstance->GetCurLevelIdx(), LAYER_UI, m_ChildrenVec[iRandomNum]));
		
		//_float fBackLimitYPos= static_cast<CTurnCharcterUI*>(m_ChildrenVec.back())->Get_LimitYPos();
		pNewUI->MoveControl(CTurnCharcterUI::UI_POS_QUICK);
		pNewUI->Set_LimitYPos_Float(_float(fBackLimitYPos));
		m_ChildrenVec.push_back(pNewUI);
		Safe_AddRef(pNewUI);
		fBackLimitYPos += 50.f;
	}

	for (auto iter : OldCharImage)
	{
		pGameInstance->DeleteGameObject(pGameInstance->GetCurLevelIdx(), (iter)->Get_ObjectName());
	}
	OldCharImage.clear();

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
