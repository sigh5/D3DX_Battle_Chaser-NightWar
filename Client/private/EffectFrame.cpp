#include "stdafx.h"
#include "..\public\EffectFrame.h"
#include "GameInstance.h"



CEffectFrame::CEffectFrame(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectFrame::CEffectFrame(const CEffectFrame & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectFrame::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectFrame::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(1.f, 1.0f, 1.0f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(5.f, 0.f, 0.f, 1.f));


	return S_OK;
}

HRESULT CEffectFrame::Last_Initialize()
{
	return S_OK;
}

void CEffectFrame::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	//m_pVIBufferCom->Tick(TimeDelta);

}

void CEffectFrame::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);



	m_iPlayOnFrameCnt++;

	if (m_iPlayOnFrameCnt == m_iFrameCnt)
	{
		m_iWidthTextureCnt++;

		if (m_iWidthTextureCnt == m_iTextureCnt_W)
		{
			m_iWidthTextureCnt = 1;
			m_iHeightTextureCnt++;
			m_fOldX = m_fCurX;
		}

		if (m_iHeightTextureCnt == m_iTextureCnt_H)
		{
			m_iHeightTextureCnt = 1;
			m_fOldY = m_fCurY;
		}

		m_iPlayOnFrameCnt = 0;
		m_fCurX =  (m_iWidthTextureCnt % m_iTextureCnt_W) *0.25f;
		m_fCurY = (m_iHeightTextureCnt % m_iTextureCnt_H) *0.25f;


		

	}



	

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CEffectFrame::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}

_bool CEffectFrame::Piciking_GameObject()
{
	return false;
}

HRESULT CEffectFrame::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Shader_VtxTexIncCount"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Single"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireTexture"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectFrame::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	// m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_WorldMatrix);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositon(), sizeof(_float4))))
		return E_FAIL;


	if (m_fCurY == 0)
		m_fCurY = 0.25f;

	if (m_fCurX == 0)
		m_fCurX = 0.25f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UV_XCurRatio", &m_fCurX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UV_YCurRatio", &m_fCurY, sizeof(_float))))
		return E_FAIL;




	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CEffectFrame * CEffectFrame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectFrame*		pInstance = new CEffectFrame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffectFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffectFrame::Clone(void * pArg)
{
	CEffectFrame*		pInstance = new CEffectFrame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffectFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffectFrame::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}