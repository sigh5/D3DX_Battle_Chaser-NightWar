#include "stdafx.h"
#include "..\public\DamageFont.h"
#include "GameInstance.h"
#include "Client_Manager.h"


CDamageFont::CDamageFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice,pContext)
{
}

CDamageFont::CDamageFont(const CDamageFont& rhs)
	: CHitBoxObject(rhs)
{
}

HRESULT CDamageFont::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamageFont::Initialize(void * pArg)
{
	m_ObjectName = TEXT("DamgaeFont");
	
	ZeroMemory(&m_fontDesc, sizeof(m_fontDesc));

	if (pArg != nullptr)
	{
		memcpy(&m_fontDesc, pArg, sizeof(m_fontDesc));
	}

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	if (FAILED(Ready_Font()))
		return E_FAIL;
	
	
	return S_OK;
}

void CDamageFont::Tick(_double TimeDelta)
{
	
	__super::Tick(TimeDelta);

	/*if (m_bShaking)
	{
		_float ShakefX = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;
		m_pVIBufferCom->Tick_Shaking(20.f*TimeDelta, ShakefX);
	}
	else*/
	{
		m_pVIBufferCom->Tick(25.f*TimeDelta);
	}
}

void CDamageFont::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CDamageFont::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();


	return S_OK;
}

HRESULT CDamageFont::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	
	m_bLast_Initlize = true;
	return S_OK;
}


HRESULT CDamageFont::Ready_Font()
{
	CVIBuffer_Point_Instancing::VIBUffer_Point_TextureDesc Desc;
	ZeroMemory(&Desc, sizeof(Desc));

	Desc.m_iFrameCnt = 0;
	Desc.m_iTextureMax_Width_Cnt = 5.f;
	Desc.m_iTextureMax_Height_Cnt = 3.f;

	m_pVIBufferCom->Set_Point_Desc(Desc);
	m_pVIBufferCom->Set_Point_Instancing_MainTain();
	m_pVIBufferCom->Set_Point_Instancing_Num(1);
	m_pVIBufferCom->Set_Point_Instancing_Scale(_float3(1.f, 1.f, 1.f));

	switch (m_fontDesc.iRepresentNum)
	{
	case 0:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(2.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(0.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
		break;

	case 1:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(4.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(1.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
		break;
	case 2:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(6.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(2.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
		break;
	case 3:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(8.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(3.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
		break;
	case 4:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(10.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(4.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
		break;
	case 5:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(12.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(0.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
		break;
	case 6:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(14.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(1.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
		break;
	case 7:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(16.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(2.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
		break;

	case 8:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(18.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(3.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
		break;
	case 9:
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.0f, 0.f, 0.f, 1.f));
		m_pVIBufferCom->Set_WidthTextureCnt(4.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
		break;


	default:
		break;
	}



	return S_OK;
}

void CDamageFont::Render_Font(_float4 vPos, _float3 vScale)
{
	m_pVIBufferCom->Move_Up_Position(_float4(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
	m_pVIBufferCom->Set_Point_Instancing_Scale(vScale);

}

HRESULT CDamageFont::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Shader_VtxTexIncCount"), TEXT("Com_Shader"),
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Font"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDamageFont::SetUp_ShaderResources()
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

	if (FAILED(m_pVIBufferCom->Set_UV_RawValue(m_pShaderCom)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	return S_OK;

}

CDamageFont * CDamageFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDamageFont*		pInstance = new CDamageFont(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Point_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDamageFont::Clone(void * pArg)
{
	CDamageFont*		pInstance = new CDamageFont(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDamageFont");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDamageFont::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
