#include "stdafx.h"
#include "..\public\MeshGround.h"
#include "GameInstance.h"

CMeshGround::CMeshGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CMeshGround::CMeshGround(const CMeshGround & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeshGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshGround::Initialize(void * pArg)
{
	 m_ObjectName = L"Test_Ground";


	 if (FAILED(__super::Initialize(pArg)))
		 return E_FAIL;

	 if (FAILED(SetUp_Components()))
		 return E_FAIL;

	 m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(52.f, -1.5f, 55.f, 1.f));

	 m_pTransformCom->Set_Scaled(_float3(5.f, 1.f, 5.f));


	 _matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

	 matScale = XMMatrixScaling(5.f, 1.f, 5.f);

	 matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	 matRotY = XMMatrixRotationY(XMConvertToRadians(45.f));
	 matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	 matTrans = XMMatrixTranslation(52.f, -1.5f, 55.f);

	 m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;
	 _float4x4 m_WorldMat;

	 XMStoreFloat4x4(&m_WorldMat, m_matWorld);
	 m_pTransformCom->Set_WorldMatrix(m_WorldMat);

	 return S_OK;
}

void CMeshGround::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

}

void CMeshGround::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}

}

HRESULT CMeshGround::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (i == 0)
			continue;
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices", "DN_FR_FishingRod");
	}
	return S_OK;
}


HRESULT CMeshGround::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bLast_Initlize = true;
	return S_OK;
}

HRESULT CMeshGround::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModelUVMove"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

#ifdef NOMODLES
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Ground"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CombatMapTwo"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;
#else
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_COMBAT, TEXT("Prototype_Component_Ground"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
#endif
	

	return S_OK;
}

HRESULT CMeshGround::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", 7)))
		return E_FAIL;

	return S_OK;
}

CMeshGround * CMeshGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMeshGround*		pInstance = new CMeshGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMeshGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMeshGround::Clone(void * pArg)
{
	CMeshGround*		pInstance = new CMeshGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMeshGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshGround::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}