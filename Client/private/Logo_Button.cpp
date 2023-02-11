#include "stdafx.h"
#include "..\public\Logo_Button.h"
#include "GameInstance.h"
#include "CombatController.h"
#include "CombatActors.h"
#include "MyImage.h"
#include "Player.h"

CLogo_Button::CLogo_Button(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CLogo_Button::CLogo_Button(const CLogo_Button & rhs)
	: CUI(rhs)
{
}

HRESULT CLogo_Button::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogo_Button::Initialize(void * pArg)
{
	m_ObjectName = TEXT("GameStartButton");

	if (nullptr != pArg)
		memcpy(&m_UIDesc, pArg, sizeof(UIDESC));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	
	m_pTransformCom->Set_Scaled(_float3(300.f, 200.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f,-250.f,0.1f,1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	m_bRenderActive = false;
	return S_OK;
}

HRESULT CLogo_Button::Last_Initialize()
{
	return S_OK;
}

void CLogo_Button::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
	{
		Click_This_Button();
	}

	In_This_Button();

	RELEASE_INSTANCE(CGameInstance);
}

void CLogo_Button::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLogo_Button::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();

	return S_OK;
}

_bool CLogo_Button::Click_This_Button()
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_long lLeft, lTop, lRight, lBottom;
	_float4	vPos;

	memcpy(&vPos, &m_pTransformCom->Get_WorldMatrix().r[3], sizeof(_float4));

	_float3 vScale;
	XMStoreFloat3(&vScale, XMLoadFloat3(&m_pTransformCom->Get_Scaled()));

	lLeft = (_long)(vPos.x + g_iWinSizeX*0.5f - vScale.x*0.5f);
	lRight = (_long)(vPos.x + g_iWinSizeX*0.5f + vScale.x*0.5f);
	lTop = (_long)(g_iWinSizeY * 0.5f - vPos.y - vScale.y*0.5f);
	lBottom = (_long)(g_iWinSizeY * 0.5f - vPos.y + vScale.y*0.5f);

	RECT rcButton = { lLeft, lTop, lRight, lBottom };

	if (PtInRect(&rcButton, ptMouse))
	{
		m_bIsClicked = true;
	}

	return m_bIsClicked;
}

void CLogo_Button::In_This_Button()
{
	POINT ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_long lLeft, lTop, lRight, lBottom;
	_float4	vPos;

	memcpy(&vPos, &m_pTransformCom->Get_WorldMatrix().r[3], sizeof(_float4));

	_float3 vScale;
	XMStoreFloat3(&vScale, XMLoadFloat3(&m_pTransformCom->Get_Scaled()));

	lLeft = (_long)(vPos.x + g_iWinSizeX*0.5f - vScale.x*0.5f);
	lRight = (_long)(vPos.x + g_iWinSizeX*0.5f + vScale.x*0.5f);
	lTop = (_long)(g_iWinSizeY * 0.5f - vPos.y - vScale.y*0.5f);
	lBottom = (_long)(g_iWinSizeY * 0.5f - vPos.y + vScale.y*0.5f);

	RECT rcButton = { lLeft, lTop, lRight, lBottom };

	if (PtInRect(&rcButton, ptMouse))
	{
		m_pTransformCom->Set_Scaled(_float3(350.f, 250.f, 1.f));
	}
	else
		m_pTransformCom->Set_Scaled(_float3(300.f, 200.f, 1.f));

}


HRESULT CLogo_Button::SetUp_Components()
{
	/* For.Com_Renderer */
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
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_MouseCusor"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogo_Button::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTH))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",5)))
		return E_FAIL;


	return S_OK;
}

CLogo_Button * CLogo_Button::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLogo_Button*		pInstance = new CLogo_Button(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLogo_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLogo_Button::Clone(void * pArg)
{
	CLogo_Button*		pInstance = new CLogo_Button(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLogo_Button");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLogo_Button::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
