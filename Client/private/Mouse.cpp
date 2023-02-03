#include "stdafx.h"
#include "..\public\Mouse.h"
#include "GameInstance.h"


CMouse::CMouse(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMouse::CMouse(const CMouse & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMouse::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMouse::Initialize(void * pArg)
{
	m_ObjectName = TEXT("MouseCuSor");

	CGameObject::GAMEOBJECTDESC GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = 90.f;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

HRESULT CMouse::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
	
	m_pTransformCom->Set_Scaled(_float3(74.f, 49.f, 1.f));

	m_bLast_Initlize = true;
	return S_OK;
}

void CMouse::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	CGameInstance* pGameIntance = GET_INSTANCE(CGameInstance);

	if (pGameIntance->GetCurLevelIdx() == LEVEL_LOGO)
	{
		m_iTextureNum = 4;
		m_pTransformCom->Set_Scaled(_float3(40.f, 35.f, 1.f));
	}
	else if (pGameIntance->GetCurLevelIdx() == LEVEL_GAMEPLAY)
	{
		m_iTextureNum = 4;
		m_pTransformCom->Set_Scaled(_float3(40.f, 35.f, 1.f));
	}
	else if (pGameIntance->GetCurLevelIdx() == LEVEL_COMBAT)
	{
		if (m_iTextureNum == 0)
		{
			m_pTransformCom->Set_Scaled(_float3(74.f, 49.f, 1.f));
		}
		else if (m_iTextureNum == 1)
		{
			m_pTransformCom->Set_Scaled(_float3(67.f, 55.f, 1.f));
		}
		else if (m_iTextureNum == 2)
		{
			m_pTransformCom->Set_Scaled(_float3(64.f, 149.f, 1.f));
		}
		else if (m_iTextureNum == 4)
		{
			m_pTransformCom->Set_Scaled(_float3(40.f, 35.f, 1.f));
		}

		//m_iTextureNum = 1;
		//m_pTransformCom->Set_Scaled(_float3(74.f, 49.f, 1.f));
	}

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(ptMouse.x - g_iWinSizeX * 0.5f, -ptMouse.y + g_iWinSizeY * 0.5f,
		0.f, 1.f));




	RELEASE_INSTANCE(CGameInstance);

}

void CMouse::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_INVENTORY, this);
}

HRESULT CMouse::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();

	return S_OK;
}

_bool CMouse::Piciking_GameObject()
{
	return false;
}

HRESULT CMouse::SetUp_Components()
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

HRESULT CMouse::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;


	return S_OK;
}

CMouse * CMouse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMouse*		pInstance = new CMouse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMouse::Clone(void * pArg)
{
	CMouse*		pInstance = new CMouse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMouse::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}
