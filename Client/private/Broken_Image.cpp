#include "stdafx.h"
#include "..\public\Broken_Image.h"
#include "GameInstance.h"
#include "CombatMap.h"

CBroken_Image::CBroken_Image(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnvironment_Object(pDevice, pContext)
{
}

CBroken_Image::CBroken_Image(const CBroken_Image & rhs)
	: CEnvironment_Object(rhs)
{
}

void CBroken_Image::Reset_Anim()
{
	m_pModelCom->InitChannel();
	m_pModelCom->Set_Finished(0,false);
	m_pModelCom->Set_Finished(1, false);
	m_pModelCom->Set_Finished(2, false);

}

HRESULT CBroken_Image::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBroken_Image::Initialize(void * pArg)
{
	m_ObjectName = L"Broken_Image";

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	_float4x4	m_WorldMat;
	matScale = XMMatrixScaling(13.f, 13.f, 13.f);

	matRotX = XMMatrixRotationX(XMConvertToRadians(45.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(0.f));
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(-0.f, 0.f, -0.f);
	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	XMStoreFloat4x4(&m_WorldMat, m_matWorld);
	m_pTransformCom->Set_WorldMatrix(m_WorldMat);


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());


	//Reset_Anim();

	//m_pModelCom->Set_AnimIndex(1);
	m_pModelCom->Set_AnimIndex(2);
	m_pModelCom->Set_AnimTickTime(0.75);

	return S_OK;
}

HRESULT CBroken_Image::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bLast_Initlize = true;

	return S_OK;
}

void CBroken_Image::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	if (m_pModelCom->Get_Finished(1)&& !m_bAnimChange[0])
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
	

	if(!m_bAnimChange[2])
		m_pModelCom->Play_Animation_2(TimeDelta);
	
	//m_pModelCom->Play_Animation(TimeDelta,false);

}

void CBroken_Image::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	_float4	vImagePos;
	XMStoreFloat4(&vImagePos, XMLoadFloat4(&pGameInstance->Get_CamPositon()));

	/*static _float vPos[3] = { 0.f,0.f,0.f};

	ImGui::InputFloat3("BrokenImage_Pos", vPos);*/

	vImagePos.y += -0.6f;
	vImagePos.z += 0.4f;
	
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vImagePos));

	RELEASE_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBroken_Image::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Render_BG(m_pShaderCom, i, "g_BoneMatrices", "BlackBG");
	}

	m_pModelCom->Set_NoRenderMeshIndex(99);
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices", "BlackBG");
	}



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CBroken_Image::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Broken_Image"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_Shader2 */
	//if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader2"),
	//	(CComponent**)&m_pShaderCom2)))
	//	return E_FAIL;


	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY,TEXT("Prototype_Component_Battle_start_Shatter"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Ultimate_Banner"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	/*if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CBroken_Image::SetUp_ShaderResources()
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
	
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BGTexture", 4)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CBroken_Image * CBroken_Image::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBroken_Image*		pInstance = new CBroken_Image(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroken_Image");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBroken_Image::Clone(void * pArg)
{
	CBroken_Image*		pInstance = new CBroken_Image(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroken_Image");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBroken_Image::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}