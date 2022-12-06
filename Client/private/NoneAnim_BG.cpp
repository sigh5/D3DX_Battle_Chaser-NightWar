#include "stdafx.h"
#include "..\public\NoneAnim_BG.h"
#include "GameInstance.h"


CNoneAnim_BG::CNoneAnim_BG(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnvironment_Object(pDevice, pContext)
{
}

CNoneAnim_BG::CNoneAnim_BG(const CNoneAnim_BG & rhs) 
	: CEnvironment_Object(rhs)
{
}

HRESULT CNoneAnim_BG::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNoneAnim_BG::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));
	return S_OK;
}

HRESULT CNoneAnim_BG::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;



	m_bLast_Initlize = true;

	return S_OK;
}

void CNoneAnim_BG::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CNoneAnim_BG::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CNoneAnim_BG::Render()
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	m_pVIBufferCom->Render();
	return S_OK;
}

_bool CNoneAnim_BG::Piciking_GameObject()
{
	_float4 Temp = { 0.f,0.f,0.f,0.f };

	if (ImGui::IsMouseClicked(0))
	{
		if (m_pVIBufferCom->PickingBuffer(g_hWnd, m_pTransformCom, Temp))
		{
			return true;
		}

	}

	return false;
}

void CNoneAnim_BG::Change_Model(_uint iLevel, const wstring & NewComPonentTag)
{
	Safe_Release(m_pModelCom);
	Remove_component(TEXT("Com_Model"));

	lstrcpy(m_EnviromentDesc.m_pModelTag, NewComPonentTag.c_str());

	if (FAILED(__super::Add_Component(iLevel, m_EnviromentDesc.m_pModelTag, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		assert("Error");

}

HRESULT CNoneAnim_BG::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */ //TEXT("Prototype_Component_ScrollingCloud")
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY,m_EnviromentDesc.m_pModelTag , TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_Buffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CNoneAnim_BG::SetUp_ShaderResources()
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

CNoneAnim_BG * CNoneAnim_BG::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNoneAnim_BG*		pInstance = new CNoneAnim_BG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNoneAnim_BG::Clone(void * pArg)
{
	CNoneAnim_BG*		pInstance = new CNoneAnim_BG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNoneAnim_BG::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);

}