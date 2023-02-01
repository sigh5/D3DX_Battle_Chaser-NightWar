#include "stdafx.h"
#include "..\public\Traile_Effect_Child.h"
#include "GameInstance.h"

CTraile_Effect_Child::CTraile_Effect_Child(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice, pContext)
{
}

CTraile_Effect_Child::CTraile_Effect_Child(const CTraile_Effect_Child & rhs)
	: CHitBoxObject(rhs)
{
}

void CTraile_Effect_Child::Set_ChildDesc(Trail_Child_Pos_Desc & Desc)
{
	ZeroMemory(&m_Desc, sizeof(Trail_Child_Pos_Desc));
	memcpy(&m_Desc, &Desc, sizeof(Trail_Child_Pos_Desc));

	m_iTextureNum = Desc.iTextureNum;
	m_UVFixRatio = Desc.fFixRatio;

}

HRESULT CTraile_Effect_Child::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTraile_Effect_Child::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	
	m_pVIBufferCom->Set_Point_Instancing_MainTain();
	m_pVIBufferCom->Set_Point_Instancing_Scale(_float3(1.f, 1.f, 1.f));
	m_pVIBufferCom->Set_Point_Instancing_Num(1);
	return S_OK;
}

HRESULT CTraile_Effect_Child::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
	m_bLast_Initlize = true;
	return S_OK;
}
void CTraile_Effect_Child::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
	
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToDegrees(90.f));

	m_pTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));
	m_pVIBufferCom->Set_Point_Instancing_Scale(_float3(10.f, 10.f, 10.f));
	if (m_pVIBufferCom->UV_Move_Tick(TimeDelta))
	{

	}

}

void CTraile_Effect_Child::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CTraile_Effect_Child::Render()
{
	if(m_bIsRenderTrue)
	{ 
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		m_pShaderCom->Begin(0);
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CTraile_Effect_Child::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTrail"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CVIBuffer_Point_Instancing::VIBUffer_Point_TextureDesc PointDesc;
	ZeroMemory(&PointDesc, sizeof(PointDesc));

	memcpy(&PointDesc, &m_HitBoxDesc.Poing_Desc, sizeof(PointDesc));

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Single"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom, &PointDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_HitBoxDesc.HitBoxOrigin_Desc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_HitBoxDesc.HitBoxOrigin_Desc.m_pTextureTag, TEXT("Com_TextureColor"),
		(CComponent**)&m_pColorTextureCom)))
		return E_FAIL;
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_HitBoxDesc.HitBoxOrigin_Desc.m_pTextureTag, TEXT("Com_TextureColor2"),
		(CComponent**)&m_pColorTextureCom2)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTraile_Effect_Child::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositon(), sizeof(_float4))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	/*if (FAILED(m_pVIBufferCom->Set_UV_RawValue(m_pShaderCom)))
		return E_FAIL;*/

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	if (FAILED(m_pColorTextureCom->Bind_ShaderResource(m_pShaderCom, "g_ColorTexture", m_Desc.ColorTexture0Num)))
		return E_FAIL;
	if (FAILED(m_pColorTextureCom2->Bind_ShaderResource(m_pShaderCom, "g_ColorTexture2", m_Desc.ColorTexture1Num)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldTrailPos", &m_Desc.m_Trail_Pos, sizeof(_vector) * 4)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fUV_X_Ratio", &m_UVXRatioX, sizeof(_float))))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Set_RawValue("g_fFixRatio", &m_UVFixRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CTraile_Effect_Child * CTraile_Effect_Child::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTraile_Effect_Child*		pInstance = new CTraile_Effect_Child(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTraile_Effect_Child");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTraile_Effect_Child::Clone(void * pArg)
{
	CTraile_Effect_Child*		pInstance = new CTraile_Effect_Child(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTraile_Effect_Child");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTraile_Effect_Child::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColorTextureCom);
	Safe_Release(m_pColorTextureCom2);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
