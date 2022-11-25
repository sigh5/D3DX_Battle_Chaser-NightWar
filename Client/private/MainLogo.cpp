#include "stdafx.h"
#include "..\public\MainLogo.h"

#include "GameInstance.h"
#include "BackGround.h"
CMainLogo::CMainLogo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CMainLogo::CMainLogo(const CMainLogo & rhs)
	:CUI(rhs)
{
}

HRESULT CMainLogo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainLogo::Initialize(void * pArg)
{

	CUI::UIDESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	m_fSizeX = (_float)g_iWinSizeX/2;
	m_fSizeY = (_float)g_iWinSizeY/2;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - m_fSizeX * 0.5f , -m_fY + m_fSizeY * 0.5f+ 50.f , 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	if (FAILED(CUI::SetUp_UI()))
		return E_FAIL;

	return S_OK;
}

void CMainLogo::Tick(_double TimeDelta)
{
	Last_Initialize();

	__super::Tick(TimeDelta);
}

void CMainLogo::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMainLogo::Render()
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

HRESULT CMainLogo::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pBackGround = pGameInstance->Get_GameObject(LEVEL_LOGO, TEXT("Layer_BackGround"), TEXT("BackGround"));


	m_pTransformCom->Set_ParentTransform(dynamic_cast<CBackGround*>(pBackGround)->Get_Transform());

	RELEASE_INSTANCE(CGameInstance);

	m_bLast_Initlize = true;
}



HRESULT CMainLogo::SetUp_Components()
{
	/* For.Com_Renderer */
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
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;

}

HRESULT CMainLogo::SetUp_ShaderResources()
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



CMainLogo * CMainLogo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMainLogo*		pInstance = new CMainLogo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMainLogo::Clone(void * pArg)
{
	CMainLogo*		pInstance = new CMainLogo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainLogo::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
