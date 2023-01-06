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
	m_ObjectName = TEXT("TestFrame");

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
	if (m_bLast_Initlize)
		return S_OK;

	m_pVIBufferCom->Set_FrameCnt(5);
	m_pVIBufferCom->Set_TextureMax_Width_Cnt(4);
	m_pVIBufferCom->Set_TextureMax_Height_Cnt(4);
	
	m_bLast_Initlize = true;
	return S_OK;
}

void CEffectFrame::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	

	if (m_fPower >= 1.f)
	{
		m_bIsChange = true;
	}
	else if (m_fPower <= 0)
		m_bIsChange = false;




	if (m_bIsChange == true)
		m_fPower += (_float)TimeDelta * -1.f;
	else
		m_fPower += (_float)TimeDelta;




	m_pVIBufferCom->UV_Move_Tick(TimeDelta);

}

void CEffectFrame::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	/*m_iPlayOnFrameCnt++;

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
*/


	

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CEffectFrame::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(1);
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Base"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;
	

	/* For.Com_Texture_Glow */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire_Glow"), TEXT("Com_Texture2"),
		(CComponent**)&m_pTextureCom2)))
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


	if (FAILED(m_pShaderCom->Set_RawValue("G_Power", &m_fPower,sizeof(_float))))
		return E_FAIL;


	//if (m_fCurY == 0)
	//	m_fCurY = 0.25f;

	//if (m_fCurX == 0)
	//	m_fCurX = 0.25f;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_UV_XCurRatio", &m_fCurX, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_UV_YCurRatio", &m_fCurY, sizeof(_float))))
	//	return E_FAIL;

	m_pVIBufferCom->Set_UV_RawValue(m_pShaderCom);





	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pTextureCom2->Bind_ShaderResource(m_pShaderCom, "g_GlowTexture")))
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