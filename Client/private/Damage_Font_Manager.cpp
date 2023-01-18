#include "stdafx.h"
#include "Damage_Font_Manager.h"
#include "GameInstance.h"
#include "DamageFont.h"

IMPLEMENT_SINGLETON(CDamage_Font_Manager)

CDamage_Font_Manager::CDamage_Font_Manager()
{
}

HRESULT CDamage_Font_Manager::Initialize()
{
	if (m_bInit)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CDamageFont::FONT_DESC FontDesc;
	CDamageFont* pFont = nullptr;
	for (_uint i = 0; i < 9; ++i)
	{
		ZeroMemory(&FontDesc, sizeof(FontDesc));
		FontDesc.iRepresentNum = i;
		pFont = (CDamageFont*)(pGameInstance->Clone_GameObject( TEXT("Prototype_GameObject_DamageFont"),&FontDesc));

		assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");
		
		m_DamageTenVec.push_back(pFont);
	}

	ZeroMemory(&FontDesc, sizeof(FontDesc));
	FontDesc.iRepresentNum = 9;
	 pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

	assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

	m_DamageTenVec.insert(m_DamageTenVec.begin(), pFont);


	for (_uint i = 0; i < 9; ++i)
	{
		ZeroMemory(&FontDesc, sizeof(FontDesc));
		FontDesc.iRepresentNum = i;
		pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

		assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

		m_DamageSingleVec.push_back(pFont);
	}

	ZeroMemory(&FontDesc, sizeof(FontDesc));
	FontDesc.iRepresentNum = 9;
	pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

	assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

	m_DamageSingleVec.insert(m_DamageSingleVec.begin(), pFont);

	RELEASE_INSTANCE(CGameInstance);

	m_bInit = true;

	return S_OK;
}

void CDamage_Font_Manager::Tick(_double TimeDelta)
{
	if (m_bFontUpdate)
	{
		m_fFontTimer += (_float)TimeDelta;

		for (auto &pFont : m_DamageTenVec)
			pFont->Tick(TimeDelta);

		for (auto &pFont : m_DamageSingleVec)
			pFont->Tick(TimeDelta);
	}
	

	if (m_fFontTimer >= 2.5f)
	{
		m_bFontUpdate = false;
		m_fFontTimer = 0.f;
	}


}

void CDamage_Font_Manager::Set_DamageFont(_float4 vPos, _float3 vScale, _int iDamage)
{
	for (auto& pFont : m_DamageTenVec)
	{
		pFont->Set_Shaking(true);
	}

	for (auto& pFont : m_DamageSingleVec)
	{
		pFont->Set_Shaking(true);
	}

	m_bFontUpdate = true;
	m_bShaking = true;
	m_vFontFirstPos = vPos;
	m_vScale = vScale;

	m_vFontSecondPos = vPos;
	m_vFontSecondPos.x += 0.5f;
	m_vFontSecondPos.z -= 1.0f;
	
	if (iDamage >= 10)
	{
		m_iCalDamage = iDamage / 10;
		m_iNumTens = m_iCalDamage;
		m_iNumSingle = iDamage % (10 * m_iCalDamage);
	}
	else
	{
		m_iNumTens = 0;
		m_iNumSingle = iDamage % 10;
	}

	if (0 == m_iNumTens && 0 == m_iNumSingle)
	{
		return;
	}
	else if (0 == m_iNumTens && 0 != m_iNumSingle)
	{
		m_DamageSingleVec[m_iNumSingle]->Render_Font(m_vFontFirstPos, m_vScale);
	}
	else
	{
		m_DamageTenVec[m_iNumTens]->Render_Font(m_vFontFirstPos, m_vScale);
		m_DamageSingleVec[m_iNumSingle]->Render_Font(m_vFontSecondPos, m_vScale);
	}


}

void CDamage_Font_Manager::Set_HPMPFont(_float4 vPos, _float3 vScale, _int iDamage)
{

	for (auto& pFont : m_DamageTenVec)
	{
		pFont->Set_Shaking(false);
	}

	for (auto& pFont : m_DamageSingleVec)
	{
		pFont->Set_Shaking(false);
	}


	m_bFontUpdate = true;
	m_bShaking = false;
	m_vFontFirstPos = vPos;
	m_vScale = vScale;

	m_vFontSecondPos = vPos;
	m_vFontSecondPos.x += 0.5f;
	m_vFontSecondPos.z -= 1.0f;

	if (iDamage >= 10)
	{
		m_iCalDamage = iDamage / 10;
		m_iNumTens = m_iCalDamage;
		m_iNumSingle = iDamage % (10 * m_iCalDamage);
	}
	else
	{
		m_iNumTens = 0;
		m_iNumSingle = iDamage % 10;
	}

	if (0 == m_iNumTens && 0 == m_iNumSingle)
	{
		return;
	}
	else if (0 == m_iNumTens && 0 != m_iNumSingle)
	{
		m_DamageSingleVec[m_iNumSingle]->Render_Font(m_vFontFirstPos, m_vScale);
	}
	else
	{
		m_DamageTenVec[m_iNumTens]->Render_Font(m_vFontFirstPos, m_vScale);
		m_DamageSingleVec[m_iNumSingle]->Render_Font(m_vFontSecondPos, m_vScale);
	
	}
}

void CDamage_Font_Manager::Late_Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(m_bFontUpdate && pGameInstance->GetCurLevelIdx() == LEVEL_COMBAT)
	{ 
		if (0 == m_iNumTens && 0 == m_iNumSingle)
		{
			return;
		}
		else if (0 == m_iNumTens && 0 != m_iNumSingle)
		{
			m_DamageSingleVec[m_iNumSingle]->Late_Tick(TimeDelta);
		}
		else
		{
			m_DamageTenVec[m_iNumTens]->Late_Tick(TimeDelta);
			m_DamageSingleVec[m_iNumSingle]->Late_Tick(TimeDelta);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CDamage_Font_Manager::Free()
{
	for (auto &pFont : m_DamageTenVec)
	{
		Safe_Release(pFont);
	}
	m_DamageTenVec.clear();

	for (auto &pFont2 : m_DamageSingleVec)
	{
		Safe_Release(pFont2);
	}
	m_DamageSingleVec.clear();

}

