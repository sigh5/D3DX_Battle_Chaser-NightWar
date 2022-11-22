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

void CUI::Free()
{
	__super::Free();
}
