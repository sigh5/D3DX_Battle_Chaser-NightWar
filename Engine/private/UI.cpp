#include "..\public\UI.h"



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
		memcpy(&m_UIDesc, pArg, sizeof(m_UIDesc));

	if (FAILED(__super::Initialize(&m_UIDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Last_Initialize()
{
	return S_OK;
}

void CUI::Tick(_double TimeDelta)
{
}

void CUI::Late_Tick(_double TimeDelta)
{
}

HRESULT CUI::Render()
{
	return S_OK;
}

HRESULT CUI::SetUp_UI()
{
	// 이거나중에 GameInstance 에서 뺴야될듯
	m_pContext->OMGetDepthStencilState(&m_OldDepthStencilState, &m_StencilRef);
	Safe_AddRef(m_OldDepthStencilState);

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

	Safe_Release(m_UIdepthStencilState);
	Safe_Release(m_OldDepthStencilState);
}
