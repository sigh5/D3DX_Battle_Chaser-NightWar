#include "stdafx.h"
#include "..\public\Hero_Gully.h"

#include "GameInstance.h"

CHero_Gully::CHero_Gully(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice,pContext)
{
}

CHero_Gully::CHero_Gully(const CHero_Gully & rhs)
	: CPlayer(rhs)
{
}

HRESULT CHero_Gully::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHero_Gully::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Hero_Gully");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHero_Gully::Last_Initialize()
{
	return S_OK;
}

void CHero_Gully::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (GetKeyState('Q') & 0x8000)
	{
		int testi = 0;
		int teste = 1;
		m_Hero_GullyHPDelegater.broadcast(testi, teste);
	}


	

}

void CHero_Gully::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);




	if (pGameInstance->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		if (m_pVIBufferCom->PickingCube(g_hWnd, m_pTransformCom))
			bool b = false;
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CHero_Gully::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CHero_Gully::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Cubedds"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHero_Gully::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CHero_Gully * CHero_Gully::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHero_Gully*		pInstance = new CHero_Gully(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHero_Gully");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHero_Gully::Clone(void * pArg)
{
	CHero_Gully*		pInstance = new CHero_Gully(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHero_Gully");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHero_Gully::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}