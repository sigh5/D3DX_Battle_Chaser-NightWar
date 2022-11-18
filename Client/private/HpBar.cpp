#include "stdafx.h"
#include "..\public\HpBar.h"

#include "GameInstance.h"
#include "Hero_Gully.h"
CHpBar::CHpBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CHpBar::CHpBar(const CHpBar & rhs)
	:CUI(rhs)
{
}

HRESULT CHpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHpBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == dynamic_cast<CHero_Gully*>((m_UIDesc.pBroadCaster)))	// 구독할 클라스를 알아야됌
		return E_FAIL;

	dynamic_cast<CHero_Gully*>(m_UIDesc.pBroadCaster)->m_Hero_GullyHPDelegater.bind(this, &CHpBar::UI_Event);

	return S_OK;
}

void CHpBar::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CHpBar::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	
}

HRESULT CHpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CHpBar::UI_Event(int a, int b)
{
	bool c = false;
	MSG_BOX("22");
}

HRESULT CHpBar::SetUp_Components()
{
	return S_OK;
}

HRESULT CHpBar::SetUp_ShaderResources()
{
	return S_OK;
}

CHpBar * CHpBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHpBar*		pInstance = new CHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHpBar::Clone(void * pArg)
{
	CHpBar*		pInstance = new CHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHpBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHpBar::Free()
{
	
	
	__super::Free();

	

}
