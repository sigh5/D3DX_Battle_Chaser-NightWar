#include "stdafx.h"
#include "..\public\DungeonMaps.h"

#include "GameInstance.h"

CDungeonMaps::CDungeonMaps(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnvironment_Object(pDevice, pContext)
{
}

CDungeonMaps::CDungeonMaps(const CDungeonMaps & rhs)
	: CEnvironment_Object(rhs)
{
}

HRESULT CDungeonMaps::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeonMaps::Initialize(void * pArg)
{
	//m_ObjectName = TEXT("Dungeon_Maps");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeonMaps::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;


	m_bLast_Initlize = true;

	return S_OK;
}

void CDungeonMaps::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (GetKeyState('Z') & 0x8000)
	{
		m_iShaderPass = 0;
	}
	if (GetKeyState('X') & 0x8000)
	{
		m_iShaderPass = 1;
	}

}

void CDungeonMaps::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CDungeonMaps::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;



	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, m_iShaderPass);
	}

	return S_OK;
}

_bool CDungeonMaps::Piciking_GameObject()
{
	return false;
}


void CDungeonMaps::Change_Model(_uint iLevel, const wstring & NewComPonentTag)
{
	if (nullptr == m_pModelCom)
	{
		Safe_Release(m_pModelCom);
		Remove_component(TEXT("Com_Model"));
	}
	lstrcpy(m_EnviromentDesc.m_pModelTag, NewComPonentTag.c_str());

	if (FAILED(__super::Add_Component(iLevel, m_EnviromentDesc.m_pModelTag, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		assert("Error");
}

HRESULT CDungeonMaps::SetUp_Components()
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

HRESULT CDungeonMaps::SetUp_ShaderResources()
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

CDungeonMaps * CDungeonMaps::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDungeonMaps*		pInstance = new CDungeonMaps(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDungeonMaps");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDungeonMaps::Clone(void * pArg)
{
	CDungeonMaps*		pInstance = new CDungeonMaps(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDungeonMaps");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDungeonMaps::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
