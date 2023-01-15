#include "stdafx.h"
#include "..\public\CombatMapBG.h"
#include "GameInstance.h"
#include "Client_Manager.h"
#include "CombatController.h"
#include "CombatActors.h"

CCombatMapBG::CCombatMapBG(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CCombatMapBG::CCombatMapBG(const CCombatMapBG & rhs)
	: CUI(rhs)
{
}

HRESULT CCombatMapBG::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCombatMapBG::Initialize(void * pArg)
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

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CCombatMapBG::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	return S_OK;
}

void CCombatMapBG::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
}

void CCombatMapBG::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCombatMapBG::Render()
{
	return E_NOTIMPL;
}

void CCombatMapBG::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
}

void CCombatMapBG::Set_RenderActive(_bool bRenderActive)
{
}

HRESULT CCombatMapBG::SetUp_Components()
{
	return E_NOTIMPL;
}

HRESULT CCombatMapBG::SetUp_ShaderResources()
{
	return E_NOTIMPL;
}

CCombatMapBG * CCombatMapBG::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	return nullptr;
}

CGameObject * CCombatMapBG::Clone(void * pArg)
{
	return nullptr;
}

void CCombatMapBG::Free()
{
}
