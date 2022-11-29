#include "..\public\UI.h"

#include "Object_Manager.h"
#include "Canvas.h"
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

void CUI::Set_parentName(const _tchar * pParentTag)
{
	CObject_Manager *pObjMgr = GET_INSTANCE(CObject_Manager);

	m_pParentObject = pObjMgr->Get_GameObject(2, TEXT("Layer_UI"), pParentTag);

	dynamic_cast<CCanvas*>(m_pParentObject)->Add_ChildUI(this);


	RELEASE_INSTANCE(CObject_Manager);
}

HRESULT CUI::SetUp_UI()
{
	//// 이거나중에 GameInstance 에서 뺴야될듯
	m_pContext->OMGetDepthStencilState(&m_OldDepthStencilState, &m_StencilRef);
	//Safe_AddRef(m_OldDepthStencilState);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	// Create the depth stencil state.
	if (FAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_UIdepthStencilState)))
		return E_FAIL;
	
	

	return S_OK;
}

void CUI::Begin_UI()
{
	m_pContext->OMSetDepthStencilState(m_UIdepthStencilState, 1);
	
}

void CUI::End_UI()
{
	m_pContext->OMSetDepthStencilState(m_OldDepthStencilState, m_StencilRef);
}

void CUI::Free()
{
	__super::Free();

	//if(m_UIDesc.pBroadCaster != nullptr)
	//	Safe_Release(m_UIDesc.pBroadCaster);
	//
	Safe_Release(m_OldDepthStencilState);
	Safe_Release(m_UIdepthStencilState);
	
}
