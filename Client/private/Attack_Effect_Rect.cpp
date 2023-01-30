#include "stdafx.h"
#include "..\public\Attack_Effect_Rect.h"
#include "GameInstance.h"
#include "Bone.h"

CAttack_Effect_Rect::CAttack_Effect_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice, pContext)
{
}

CAttack_Effect_Rect::CAttack_Effect_Rect(const CAttack_Effect_Rect & rhs)
	: CHitBoxObject(rhs)
{
}

void CAttack_Effect_Rect::Set_Client_BuffDesc(Attack_Effec_Client & Desc)
{
	memcpy(&m_Client_AttackEffect_Desc, &Desc, sizeof(m_Client_AttackEffect_Desc));
	Reset_CurMartirx();

}

void CAttack_Effect_Rect::Set_Glow(_bool bUseGlow, wstring GlowTag, _int iGlowTextureNumber)
{
	m_bUseGlow = bUseGlow;
	m_GlowstrTag = GlowTag;
	m_iGlowTextureNum = iGlowTextureNumber;

	if (true == m_bUseGlow)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, GlowTag.c_str(), TEXT("Com_GlowTexture"),
			(CComponent**)&m_pGlowTextureCom)))
			assert(!" CBuff_Effect::Set_Glow");

		m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass = 1; //Glow
	}
}



HRESULT CAttack_Effect_Rect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAttack_Effect_Rect::Initialize(void * pArg)
{
	ZeroMemory(&m_Client_AttackEffect_Desc, sizeof(m_Client_AttackEffect_Desc));


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
#ifdef  NOMODLES
	Set_Glow(true, TEXT("Prototype_Component_Texture_bretto_Lazer_Ultimate_Rect"), 0);
#endif //  NOMODLES


	return S_OK;
}

HRESULT CAttack_Effect_Rect::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bLast_Initlize = true;

	return S_OK;
}

void CAttack_Effect_Rect::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);



	m_iPlayOnFrameCnt++;

	if (m_iPlayOnFrameCnt == m_Client_AttackEffect_Desc.iFrameCnt)
	{

		m_iPlayOnFrameCnt = 0;
		++m_iTextureNum;

		if (m_iTextureNum >= 7)
		{
			m_iTextureNum = 0;

			/*	m_bIsFinsishBuffer = true;
				m_bGlowEnd = true;*/
		}
	}

	if (m_bUseGlow)
	{
		if (m_fGlowStrength >= 1.f)
			m_bIsChange = true;
		else if (m_fGlowStrength <= 0)
			m_bIsChange = false;

		if (m_bIsChange == true)
			m_fGlowStrength += (_float)TimeDelta * -1.f;
		else
			m_fGlowStrength += (_float)TimeDelta;
	}

}

void CAttack_Effect_Rect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CAttack_Effect_Rect::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(4);
	m_pVIBufferCom->Render();
	return S_OK;
}

HRESULT CAttack_Effect_Rect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_HitBoxDesc.HitBoxOrigin_Desc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAttack_Effect_Rect::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	
	RELEASE_INSTANCE(CGameInstance);
	
	if (m_bUseGlow == true)
	{
		if (m_bGlowEnd == true)
		{
			_float MaxPower = 1.f;
			if (FAILED(m_pShaderCom->Set_RawValue("G_Power", &MaxPower, sizeof(_float))))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Set_RawValue("G_Power", &m_fGlowStrength, sizeof(_float))))
				return E_FAIL;
		}

		if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_GlowTexture", m_iTextureNum)))
			return E_FAIL;
	}


	return S_OK;
}

void CAttack_Effect_Rect::Reset_CurMartirx()
{
	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	matScale = XMMatrixScaling(140.f, 50.f, 1.f);
	matRotX = XMMatrixRotationX(XMConvertToRadians(-18.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(47.f));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(-24.f));
	matTrans = XMMatrixTranslation(53.6f,-2.77f,-2.72f);
	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;

	XMStoreFloat4x4(&m_SocketMatrix, m_OriginMatrix);

	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	
}

CAttack_Effect_Rect * CAttack_Effect_Rect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAttack_Effect_Rect*		pInstance = new CAttack_Effect_Rect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCombatMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAttack_Effect_Rect::Clone(void * pArg)
{
	CAttack_Effect_Rect*		pInstance = new CAttack_Effect_Rect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCombatMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAttack_Effect_Rect::Free()
{
	__super::Free();

	if (m_bUseGlow)
	{
		Safe_Release(m_pGlowTextureCom);
	}

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}