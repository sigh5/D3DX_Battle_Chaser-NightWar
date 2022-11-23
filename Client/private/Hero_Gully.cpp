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
	return S_OK;
}

HRESULT CHero_Gully::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Hero_Gully");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHero_Gully::Last_Initialize()
{
	return E_NOTIMPL;
}

void CHero_Gully::Tick(_double TimeDelta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		int testi = 0;
		int teste = 1;
		m_Hero_GullyHPDelegater.broadcast(testi, teste);
	}
}

void CHero_Gully::Late_Tick(_double TimeDelta)
{
}

HRESULT CHero_Gully::Render()
{
	return S_OK;
}

HRESULT CHero_Gully::SetUp_Components()
{
	//if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
	//	(CComponent**)&m_pRendererCom)))
	//	return E_FAIL;
	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
	//	(CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
	//	(CComponent**)&m_pVIBufferCom)))
	//	return E_FAIL;

	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"),
	//	(CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CHero_Gully::SetUp_ShaderResources()
{
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

	
	
}