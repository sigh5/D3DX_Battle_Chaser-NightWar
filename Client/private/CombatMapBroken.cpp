#include "stdafx.h"
#include "..\public\CombatMapBroken.h"
#include "GameInstance.h"
#include "CombatMap.h"
#include "CombatController.h"

CCombatMapBroken::CCombatMapBroken(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnvironment_Object(pDevice, pContext)
{
}

CCombatMapBroken::CCombatMapBroken(const CCombatMapBroken & rhs)
	: CEnvironment_Object(rhs)
{
}

void CCombatMapBroken::Reset_Anim()
{
	m_pModelCom->InitChannel();
	m_pModelCom->Set_Finished(0, false);
	m_pModelCom->Set_Finished(1, false);
	m_pModelCom->Set_Finished(2, false);
}

HRESULT CCombatMapBroken::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCombatMapBroken::Initialize(void * pArg)
{
	m_ObjectName = L"Broken_CombatMapImage";
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	_float4x4	m_WorldMat;
	matScale = XMMatrixScaling(20.f, 20.f, 20.f);

	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(-5.883f, 21.412f, -10.5f);
	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	XMStoreFloat4x4(&m_WorldMat, m_matWorld);
	m_pTransformCom->Set_WorldMatrix(m_WorldMat);
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	m_pModelCom->Set_AnimIndex(2);
	Reset_Anim();
	m_pModelCom->Set_AnimTickTime(0.75);


	return S_OK;
}

HRESULT CCombatMapBroken::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	CGameInstance::GetInstance()->Play_Sound(TEXT("Common_Broken_Image.wav"), 1.0f, false, SOUND_TYPE_HIT);

	m_bLast_Initlize = true;

	return S_OK;
}

void CCombatMapBroken::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
	
	if (ImGui::Button("Reset"))
	{
		Reset_Anim();
	}

	if (m_pModelCom->Get_Finished(1) && !m_bAnimChange[0])
	{
		m_pModelCom->Set_AnimIndex(0);
		m_bAnimChange[0] = true;
		m_pModelCom->Set_AnimTickTime(0.75);
	}

	if (m_pModelCom->Get_Finished(0) && !m_bAnimChange[1])
	{
		m_pModelCom->Set_AnimIndex(2);
		m_bAnimChange[1] = true;
		m_pModelCom->Set_AnimTickTime(0.75);
	}

	if (m_pModelCom->Get_Finished(2) && !m_bAnimChange[2])
	{
		m_bAnimChange[2] = true;
	}


	if (!m_bAnimChange[2])
		m_pModelCom->Play_Animation_2(TimeDelta);
	else
	{
		CCombatController::GetInstance()->Debuff_Stun();
	}
	//m_pModelCom->Play_Animation(TimeDelta,false);
}

void CCombatMapBroken::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CCombatMapBroken::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/*for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Render_BG(m_pShaderCom, i, "g_BoneMatrices", "BlackBG");
	}*/

	m_pModelCom->Set_NoRenderMeshIndex(99);
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if(i==0)
			continue;
		m_pModelCom->Render(m_pShaderCom, i, 2, "g_BoneMatrices", "BlackBG");
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

HRESULT CCombatMapBroken::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Broken_Image"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Battle_start_Shatter"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CombatMapTwo"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCombatMapBroken::SetUp_ShaderResources()
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

	m_pShaderCom->Set_ShaderResourceView("g_DiffuseTexture", pGameInstance->Get_DiffuseTargetSRV());
	m_pShaderCom->Set_ShaderResourceView("g_ShadeTexture", pGameInstance->Get_ShadeTargetSRV());
	m_pShaderCom->Set_ShaderResourceView("g_SpecularTexture", pGameInstance->Get_SpecularTargetSRV());
	m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV());
	m_pShaderCom->Set_ShaderResourceView("g_ShadowDepthTexture", pGameInstance->Get_ShadowTargetSRV());
	m_pShaderCom->Set_Matrix("g_LightViewMatrix", &CGameInstance::GetInstance()->Get_Light_Matrix(TEXT("Level_Combat_Directional")));
	m_pShaderCom->Set_Matrix("g_LightProjMatrix", &CGameInstance::GetInstance()->Get_Light_ProjMatrix(TEXT("Level_Combat_Directional")));

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BGTexture", 7)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BGTexture2", 2)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CCombatMapBroken * CCombatMapBroken::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCombatMapBroken*		pInstance = new CCombatMapBroken(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCombatMapBroken");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCombatMapBroken::Clone(void * pArg)
{
	CCombatMapBroken*		pInstance = new CCombatMapBroken(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroken_Image");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCombatMapBroken::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
