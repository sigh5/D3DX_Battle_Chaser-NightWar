#include "stdafx.h"
#include "..\public\Tile.h"
#include "GameInstance.h"

CTile::CTile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTile::CTile(const CTile & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTile::Initialize(void * pArg)
{
	m_ObjectName = TEXT("CTile");

	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT CTile::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_ProtoName = TEXT("Prototype_GameObject_Tile");

	m_bLast_Initlize = true;

	return S_OK;
}

void CTile::Tick(_double TimeDelta)
{
	Last_Initialize();

	__super::Tick(TimeDelta);
}

void CTile::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CTile::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}

_float4 CTile::Get_Position() const
{
	return _float4();
}

_bool CTile::Piciking_GameObject()
{
	return false;
}

HRESULT CTile::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex_Tile"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tile"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Brush*/
	/*if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TileBrush"), TEXT("Com_Brush"),
		(CComponent**)&m_pTextureCom[TYPE_BRUSH])))
		return E_FAIL;*/

	/* For.Com_Filter */
	/*if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TileFilter"), TEXT("Com_Filter"),
		(CComponent**)&m_pTextureCom[TYPE_FILTER])))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CTile::SetUp_ShaderResources()
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

	_uint	m_iDiffuseTextureIndex = 1;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iDiffuseTextureIndex", &m_iDiffuseTextureIndex, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	/*if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;*/
	/*if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResource(m_pShaderCom, "g_FilterTexture", 0)))
		return E_FAIL;
	*/if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &_float4(15.f, 0.f, 15.f, 1.f), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CTile * CTile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTile*		pInstance = new CTile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTile::Clone(void * pArg)
{
	CTile*		pInstance = new CTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTile::Free()
{
	__super::Free();

	for (auto& pTextureCom : m_pTextureCom)
		Safe_Release(pTextureCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
