#include "stdafx.h"
#include "..\public\Debuff_Font.h"
#include "GameInstance.h"
#include "Client_Manager.h"

CDebuff_Font::CDebuff_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice, pContext)
{
}

CDebuff_Font::CDebuff_Font(const CDebuff_Font & rhs)
	: CHitBoxObject(rhs)
{
}

HRESULT CDebuff_Font::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDebuff_Font::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Explain_Font");

	ZeroMemory(&m_fontDesc, sizeof(m_fontDesc));

	if (pArg != nullptr)
	{
		memcpy(&m_fontDesc, pArg, sizeof(m_fontDesc));
	}

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Last_Initialize();
	return S_OK;
}

void CDebuff_Font::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pVIBufferCom->Tick(-10.f*TimeDelta);
}

void CDebuff_Font::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bIsRendering  && nullptr != m_pRendererCom)	
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CDebuff_Font::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CDebuff_Font::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	CVIBuffer_Point_Instancing::VIBUffer_Point_TextureDesc Desc;
	ZeroMemory(&Desc, sizeof(Desc));

	Desc.m_iFrameCnt = 0;
	Desc.m_iTextureMax_Width_Cnt = 5.f;
	Desc.m_iTextureMax_Height_Cnt = 6.f;

	m_pVIBufferCom->Set_Point_Desc(Desc);
	m_pVIBufferCom->Set_Point_Instancing_MainTain();
	m_pVIBufferCom->Set_Point_Instancing_Num(1);
	m_pVIBufferCom->Set_Point_Instancing_Scale(_float3(1.f, 1.f, 1.f));
	m_bLast_Initlize = true;
	return S_OK;
}

void CDebuff_Font::Render_Font(_float4 vPos, _float3 vScale, _tchar szChar)
{
	m_bIsRendering = true;
	m_pVIBufferCom->Move_Up_Position(_float4(0.f, 0.f, 0.f, 1.f));		//000으로 초기화


	if (szChar == L'a')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(0.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
	}
	else if (szChar == L'b')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(1.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
	}
	else if (szChar == L'c')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(2.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
	}
	else if (szChar == L'd')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(3.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
	}
	else if (szChar == L'e')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(4.f);
		m_pVIBufferCom->Set_HeightTextureCnt(0.f);
	}
	else if (szChar == L'f')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(0.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
	}
	else if (szChar == L'g')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(1.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
	}
	else if (szChar == L'h')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(2.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
	}
	else if (szChar == L'i')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(3.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
	}
	else if (szChar == L'j')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(4.f);
		m_pVIBufferCom->Set_HeightTextureCnt(1.f);
	}
	else if (szChar == L'k')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(0.f);
		m_pVIBufferCom->Set_HeightTextureCnt(2.f);
	}
	else if (szChar == L'l')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(1.f);
		m_pVIBufferCom->Set_HeightTextureCnt(2.f);
	}
	else if (szChar == L'm')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(2.f);
		m_pVIBufferCom->Set_HeightTextureCnt(2.f);
	}
	else if (szChar == L'n')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(3.f);
		m_pVIBufferCom->Set_HeightTextureCnt(2.f);
	}
	
	else if (szChar == L'o')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(4.f);
		m_pVIBufferCom->Set_HeightTextureCnt(2.f);
	}
	else if (szChar == L'p')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(0.f);
		m_pVIBufferCom->Set_HeightTextureCnt(3.f);
	}
	else if (szChar == L'q')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(1.f);
		m_pVIBufferCom->Set_HeightTextureCnt(3.f);
	}

	else if (szChar == L'r')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));;
		m_pVIBufferCom->Set_WidthTextureCnt(2.f);
		m_pVIBufferCom->Set_HeightTextureCnt(3.f);
	}
	else if (szChar == L's')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(3.f);
		m_pVIBufferCom->Set_HeightTextureCnt(3.f);
	}
	else if (szChar == L't')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(4.f);
		m_pVIBufferCom->Set_HeightTextureCnt(3.f);
	}
	else if (szChar == L'u')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(0.f);
		m_pVIBufferCom->Set_HeightTextureCnt(4.f);
	}
	else if (szChar == L'v')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(1.f);
		m_pVIBufferCom->Set_HeightTextureCnt(4.f);
	}
	else if (szChar == L'w')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(2.f);
		m_pVIBufferCom->Set_HeightTextureCnt(4.f);
	}
	else if (szChar == L'x')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(3.f);
		m_pVIBufferCom->Set_HeightTextureCnt(4.f);
	}
	else if (szChar == L'y')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(4.f);
		m_pVIBufferCom->Set_HeightTextureCnt(4.f);
	}
	else if (szChar == L'z')
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pVIBufferCom->Set_WidthTextureCnt(2.f);
		m_pVIBufferCom->Set_HeightTextureCnt(5.f);
	}
	else if (szChar == L' ')
	{
		m_bIsRendering = false;
	}
	else
		m_bIsRendering = false;


	m_pVIBufferCom->Set_Point_Instancing_Scale(vScale);


}

HRESULT CDebuff_Font::SetUp_Components()
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


HRESULT CDebuff_Font::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 3)))
		return E_FAIL;

	return S_OK;
}

CDebuff_Font * CDebuff_Font::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDebuff_Font*		pInstance = new CDebuff_Font(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDebuff_Font");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDebuff_Font::Clone(void * pArg)
{
	CDebuff_Font*		pInstance = new CDebuff_Font(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDebuff_Font");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDebuff_Font::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}