#include "stdafx.h"
#include "..\public\Exp_Bar.h"
#include "GameInstance.h"
#include "Client_Manager.h"


CExp_Bar::CExp_Bar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CExp_Bar::CExp_Bar(const CExp_Bar & rhs)
	: CUI(rhs)
{
}

void CExp_Bar::Set_ExpBarStatus(CStatus * pStatus)
{
	assert(pStatus != nullptr && "Set_ExpBarStatus");

	m_pMyStatus = pStatus;

	m_fOldRatio = m_pMyStatus->Get_ExpRatio();

	_int iRandRand = rand() % 150;

	_int iRandomExp =  max(iRandRand, 40);
	pStatus->Set_Exp(iRandomExp);
	m_fCurRatio = m_pMyStatus->Get_ExpRatio();


	m_bMove = true;

}


HRESULT CExp_Bar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CExp_Bar::Initialize(void * pArg)
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

HRESULT CExp_Bar::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

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

void CExp_Bar::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	if (m_fOldRatio  <= m_fCurRatio+EPSILON)
	{
		m_fOldRatio += _float(TimeDelta *0.5f );
		m_SubXRatio = m_fSizeX - (m_fOldRatio*m_fSizeX);
		
	}
	
	if (m_fOldRatio >= 1.f)
	{
		_float NewOldRatio = m_fOldRatio - 1.f;
		m_fCurRatio = NewOldRatio;
		m_fOldRatio = 0.f;
	}
	



	if (!lstrcmp(m_ObjectName, TEXT("ExpBar_0")))
	{
		m_pTransformCom->Set_Scaled(_float3(m_fSizeX*m_fOldRatio, m_fSizeY, 1.f));
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - (m_SubXRatio*0.5f), vPos.y, 0.1f, 1.f));
		_int iLevel = m_pMyStatus->Get_Level();
		wstring	CurLevelStr = TEXT(" LEVEL ") + to_wstring(iLevel);
		m_strText = CurLevelStr;
	}
	else if (!lstrcmp(m_ObjectName, TEXT("ExpBar_1")))
	{
		m_pTransformCom->Set_Scaled(_float3(m_fSizeX*m_fOldRatio, m_fSizeY, 1.f));
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - (m_SubXRatio*0.5f), vPos.y, 0.1f, 1.f));
		_int iLevel = m_pMyStatus->Get_Level();
		wstring	CurLevelStr = TEXT(" LEVEL ") + to_wstring(iLevel);
		m_strText = CurLevelStr;

	}
	else if (!lstrcmp(m_ObjectName, TEXT("ExpBar_2")))
	{
		m_pTransformCom->Set_Scaled(_float3(m_fSizeX*m_fOldRatio, m_fSizeY, 1.f));
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - (m_SubXRatio*0.5f), vPos.y, 0.1f, 1.f));	
		_int iLevel = m_pMyStatus->Get_Level();
		wstring	CurLevelStr = TEXT(" LEVEL ") + to_wstring(iLevel);
		m_strText = CurLevelStr;
	}
}

void CExp_Bar::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CExp_Bar::Render()
{
	if (!m_bRenderActive)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;


	m_pShaderCom->Begin(m_iShaderOption);		// UI 1번 알파블랜딩
	m_pVIBufferCom->Render();



	if (!lstrcmp(m_ObjectName, TEXT("ExpBar_0")))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Render_Font(TEXT("Font_Comic"), m_strText.c_str(), _float2(785.f, 300), 0.f, _float2(0.4f, 0.4f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		RELEASE_INSTANCE(CGameInstance);
	}
	else if (!lstrcmp(m_ObjectName, TEXT("ExpBar_1")))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Render_Font(TEXT("Font_Comic"), m_strText.c_str(), _float2(785.f, 380), 0.f, _float2(0.4f, 0.4f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		RELEASE_INSTANCE(CGameInstance);
	}
	else if (!lstrcmp(m_ObjectName, TEXT("ExpBar_2")))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Render_Font(TEXT("Font_Comic"), m_strText.c_str(), _float2(785.f, 460), 0.f, _float2(0.4f, 0.4f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
		RELEASE_INSTANCE(CGameInstance);
	}
	



	return S_OK;
}

void CExp_Bar::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
	Safe_Release(m_pTextureCom);
	Remove_component(TEXT("Com_Texture"));

	lstrcpy(m_UIDesc.m_pTextureTag, NewComPonentTag.c_str());

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iLevel, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		assert("CTurnCharcterUI Change_Texture");
}

HRESULT CExp_Bar::SetUp_Components()
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

HRESULT CExp_Bar::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_Ratio", &m_fOldRatio, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CExp_Bar * CExp_Bar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CExp_Bar*		pInstance = new CExp_Bar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CExp_Bar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CExp_Bar::Clone(void * pArg)
{
	CExp_Bar*		pInstance = new CExp_Bar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CExp_Bar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CExp_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
