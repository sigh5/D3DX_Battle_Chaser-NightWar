#include "stdafx.h"
#include "..\public\MapOne2D.h"
#include "GameInstance.h"

CMapOne2D::CMapOne2D(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnvironment_Object(pDevice, pContext)
{
}

CMapOne2D::CMapOne2D(const CMapOne2D & rhs)
	: CEnvironment_Object(rhs)
{
}

HRESULT CMapOne2D::Initialize_Prototype()
{
	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapOne2D::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Map_One2D");
	m_ProtoName = TEXT("Prototype_GameObject_2D_MapOne");
	CEnvironment_Object::ENVIRONMENTDESC Desc;

	ZeroMemory(&Desc, sizeof(Desc));

	lstrcpy(Desc.m_pModelTag, TEXT("Prototype_Component_MapOne"));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	

	

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(7.6f, -4.38f, 17.f,1.f));
	return S_OK;

}

HRESULT CMapOne2D::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bLast_Initlize = true;

	return S_OK;
}

void CMapOne2D::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMapOne2D::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		
	}
}

HRESULT CMapOne2D::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, 0);
	}

	return S_OK;
}

_bool CMapOne2D::Piciking_GameObject()
{
	return false;
}

HRESULT CMapOne2D::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_EnviromentDesc.m_pModelTag, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMapOne2D::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CMapOne2D * CMapOne2D::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapOne2D*		pInstance = new CMapOne2D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapOne2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMapOne2D::Clone(void * pArg)
{
	CMapOne2D*		pInstance = new CMapOne2D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapOne2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapOne2D::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}