#include "stdafx.h"
#include "..\public\HpMpBar.h"
#include "GameInstance.h"
#include "Client_Manager.h"


CHpMpBar::CHpMpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CHpMpBar::CHpMpBar(const CHpMpBar & rhs)
	:CUI(rhs)
	,m_iOption(rhs.m_iOption)
{
}

void CHpMpBar::Set_HpBarStatus(CStatus * pStatus)
{
	assert(pStatus != nullptr && "Set_HpBarStatus");

	m_pTaskStatus = pStatus;
	m_iOption = 0;
}

void CHpMpBar::Set_MpBarStatus(CStatus * pStatus)
{
	assert(pStatus != nullptr && "Set_MpBarStatus");

	m_pTaskStatus = pStatus;
	m_iOption = 1;
}

HRESULT CHpMpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpMpBar::Initialize(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UIDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX / 16;
	m_fSizeY = (_float)g_iWinSizeY / 16;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - m_fSizeX * 0.5f, -m_fY + m_fSizeY * 0.5f + 50.f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	m_bRenderActive = true;

	return S_OK;
}

HRESULT CHpMpBar::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
	m_fRatio = 1.f;
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	m_fX = vPos.x;
	m_fY = vPos.y;
	_float3 vScale;
	vScale = m_pTransformCom->Get_Scaled();

	m_fSizeX = vScale.x;
	m_fSizeY = vScale.y;


	

	m_bLast_Initlize = true;
	return S_OK;
}

void CHpMpBar::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
	
	if (m_pTaskStatus != nullptr && m_bHit)
	{
		if (m_iOption == 0)
		{
			m_fRatio = m_pTaskStatus->Get_CurStatusHpRatio();
			
			m_fRatioX = m_fSizeX - (m_fRatio*m_fSizeX);
		
		}
		else
		{
			m_fRatio = m_pTaskStatus->Get_CurStatusMpRatio();
			m_fRatioX = m_fSizeX - (m_fRatio*m_fSizeX);
		}
	

		if (m_fRatio != 1)
		{
			m_pTransformCom->Set_Scaled(_float3(m_fSizeX*m_fRatio, m_fSizeY, 1.f));
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - (m_fRatioX*0.5f), vPos.y, 0.1f, 1.f));
		}

		m_bHit = false;
	}
	
	Shake_Move(TimeDelta);

}

void CHpMpBar::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHpMpBar::Render()
{
	if (!m_bRenderActive)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(1);		// UI 1번 알파블랜딩
	m_pVIBufferCom->Render();


	return S_OK;
}

void CHpMpBar::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
	Safe_Release(m_pTextureCom);
	Remove_component(TEXT("Com_Texture"));

	lstrcpy(m_UIDesc.m_pTextureTag, NewComPonentTag.c_str());

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iLevel, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		assert("CTurnCharcterUI Change_Texture");
}

void CHpMpBar::ShakingControl(_float fCoolTime)
{
	m_bMove = true;
	XMStoreFloat4(&m_vOriginPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

void CHpMpBar::Shake_Move(_double TimeDelta)
{
	if (m_bMove)
	{
		/*	_float4		vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));*/

		if (m_fShakeTime > m_fCurShakeTime)
		{
			m_fCurShakeTime += (_float)TimeDelta;
			_float ShakefX = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;
			_float ShakefY = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;

			_matrix matLocal = XMMatrixTranslation(ShakefX, ShakefY, 0.f);

			_matrix matWorld = m_pTransformCom->Get_LocalMatrix();

			matWorld = matLocal*matWorld;
			
			_float4x4	ShakingMat;
			XMStoreFloat4x4(&ShakingMat, matWorld);
			m_pTransformCom->Set_WorldMatrix(ShakingMat);
		}
		else
		{
			m_bMove = false;
			m_pTransformCom->Set_Scaled(_float3(m_fSizeX*m_fRatio, m_fSizeY, 1.f));
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - (m_fRatioX*0.5f), m_vOriginPos.y, 0.1f, 1.f));

			//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_vOriginPos));
		}

	}
	
		

}


HRESULT CHpMpBar::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpMpBar::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTH))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Ratio", &m_fRatio, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CHpMpBar * CHpMpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHpMpBar*		pInstance = new CHpMpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHpMpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHpMpBar::Clone(void * pArg)
{
	CHpMpBar*		pInstance = new CHpMpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHpMpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHpMpBar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
