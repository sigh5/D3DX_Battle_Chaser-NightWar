#include "stdafx.h"
#include "..\public\LoadingCircle.h"
#include "GameInstance.h"


CLoadingCircle::CLoadingCircle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CLoadingCircle::CLoadingCircle(const CLoadingCircle & rhs)
	: CUI(rhs)
{
}

HRESULT CLoadingCircle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingCircle::Initialize(void * pArg)
{
	m_ObjectName = TEXT("LoadingCircle");

	CUI::UIDESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	m_fSizeX = (_float)g_iWinSizeX / 2;
	m_fSizeY = (_float)g_iWinSizeY / 2;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	m_pTransformCom->Set_Scaled(_float3(100.f, 100.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(580.f, -300.f, 0.1f, 1.f));

	return S_OK;
}

HRESULT CLoadingCircle::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bLast_Initlize = true;
	return S_OK;
}

void CLoadingCircle::Tick(_double TimeDelta)
{
	Last_Initialize();


	if (m_iTextureNum == 0)
	{
		m_DissoivePower += 0.5f*(_float)TimeDelta;

		if (m_DissoivePower >= 1.f)
		{
			m_DissoivePower = 0.f;
		}
		m_iShaderPass = 8;
	}
	else
	{
		m_DissoivePower = 0.f;
		m_iShaderPass = 1;
	}


	__super::Tick(TimeDelta);
}

void CLoadingCircle::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);


	if (m_bRenderActive == false)
		return;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLoadingCircle::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(m_iShaderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

void CLoadingCircle::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
	m_iTextureNum = iLevel;
}

HRESULT CLoadingCircle::SetUp_Components()
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
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LoadingCircle"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingCircle::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_DissolvePower", &m_DissoivePower, sizeof(_float))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);



	return S_OK;
}

CLoadingCircle * CLoadingCircle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{

	CLoadingCircle*		pInstance = new CLoadingCircle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoadingCircle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLoadingCircle::Clone(void * pArg)
{
	CLoadingCircle*		pInstance = new CLoadingCircle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadingCircle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoadingCircle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
