#include "..\public\UI.h"

#include "Object_Manager.h"
#include "Canvas.h"
#include "Level_Manager.h"
#include "GameInstance.h"
CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
	
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Initialize(void * pArg)
{
	ZeroMemory(&m_UIDesc, sizeof(m_UIDesc));

	if (nullptr != pArg)
	{
		memcpy(&m_UIDesc, pArg, sizeof(m_UIDesc));
	}


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Last_Initialize()
{
	return S_OK;
}

void CUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::Set_parentName(_uint iCulLevel, const _tchar * pParentTag)
{
	CObject_Manager*pObjMgr = GET_INSTANCE(CObject_Manager);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pCanvas = pObjMgr->Get_GameObject(iCulLevel, TEXT("Layer_UI"), pParentTag);
	dynamic_cast<CCanvas*>(pCanvas)->Add_ChildUI(this);

	RELEASE_INSTANCE(CGameInstance);
	RELEASE_INSTANCE(CObject_Manager);
}

HRESULT CUI::SetUp_UI()
{
	

	return S_OK;
}

void CUI::Begin_UI()
{
	//m_pContext->OMSetDepthStencilState(m_UIdepthStencilState, 1);
	
}

void CUI::End_UI()
{
	//m_pContext->OMSetDepthStencilState(m_OldDepthStencilState, m_StencilRef);
}

void CUI::Free()
{
	__super::Free();

	
	
}
