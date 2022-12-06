#include "stdafx.h"
#include "..\public\WaterTile.h"
#include "GameInstance.h"

CWaterTile::CWaterTile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnvironment_Object(pDevice, pContext)
{
}

CWaterTile::CWaterTile(const CWaterTile & rhs)
	: CEnvironment_Object(rhs)
{
}

HRESULT CWaterTile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWaterTile::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));
	
	return S_OK;
}

HRESULT CWaterTile::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;


	m_bLast_Initlize = true;

	return S_OK;
}

void CWaterTile::Tick(_double TimeDelta)
{
	Last_Initialize();

	m_TimeDelta += (_float)TimeDelta;
	__super::Tick(TimeDelta);
}

void CWaterTile::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CWaterTile::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_EnviromentDesc.m_iShaderPass)))
			return E_FAIL;
	}

	return S_OK;

}

_float4 CWaterTile::Get_Position() const
{
	return _float4();
}

_bool CWaterTile::Piciking_GameObject()
{
	return false;
}

HRESULT CWaterTile::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex_Tile"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_EnviromentDesc.m_pModelTag, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_EnviromentDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CWaterTile::SetUp_ShaderResources()
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

	// For_Lights
	const	LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositon(), sizeof(_float4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_TimeDelta", &m_TimeDelta, sizeof(_float))))
		return E_FAIL;
	_float	Speed = 2.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVSpeed", &Speed, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_HeightTexture",0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_UVTexture",1)))
		return E_FAIL;
	return S_OK;
}

CWaterTile * CWaterTile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWaterTile*		pInstance = new CWaterTile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWaterTile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWaterTile::Clone(void * pArg)
{
	CWaterTile*		pInstance = new CWaterTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWaterTile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWaterTile::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
