#include "..\public\Canvas.h"


CCanvas::CCanvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice,pContext)
{
}

CCanvas::CCanvas(const CCanvas & rhs)
	: CUI(rhs)
{
}

HRESULT CCanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas::Initialize(void * pArg)
{
	ZeroMemory(&m_CanvasDesc, sizeof(m_CanvasDesc));

	if (nullptr != pArg)
		memcpy(&m_CanvasDesc, pArg, sizeof(m_CanvasDesc));

	if (FAILED(__super::Initialize(&m_CanvasDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	for (auto &pChildUI : m_ChildrenVec)
		pChildUI->Set_parent(this);
	


	m_bLast_Initlize = true;
	return S_OK;
}

void CCanvas::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CCanvas::Render()
{
	return S_OK;
}

HRESULT CCanvas::Add_ChildUI(CUI * pUI)
{
	if (nullptr == pUI)
		return E_FAIL;

	m_ChildrenVec.push_back(pUI);
	Safe_AddRef(pUI);
	
	return S_OK;
}

void CCanvas::Free()
{
	__super::Free();

	for (auto &pChildUI : m_ChildrenVec)
		Safe_Release(pChildUI);
}
