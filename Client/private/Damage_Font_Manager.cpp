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

	/*Target_0*/
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
	/*~Target_0*/

	/*Target_1*/
	pFont = nullptr;
	for (_uint i = 0; i < 9; ++i)
	{
		ZeroMemory(&FontDesc, sizeof(FontDesc));
		FontDesc.iRepresentNum = i;
		pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

		assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

		m_Target2_DamageTenVec.push_back(pFont);
	}

	ZeroMemory(&FontDesc, sizeof(FontDesc));
	FontDesc.iRepresentNum = 9;
	pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

	assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

	m_Target2_DamageTenVec.insert(m_Target2_DamageTenVec.begin(), pFont);


	for (_uint i = 0; i < 9; ++i)
	{
		ZeroMemory(&FontDesc, sizeof(FontDesc));
		FontDesc.iRepresentNum = i;
		pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

		assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

		m_Target2_DamageSingleVec.push_back(pFont);
	}

	ZeroMemory(&FontDesc, sizeof(FontDesc));
	FontDesc.iRepresentNum = 9;
	pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

	assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

	m_Target2_DamageSingleVec.insert(m_Target2_DamageSingleVec.begin(), pFont);
	/*~Target_1*/


	/*Target_2*/
	pFont = nullptr;
	for (_uint i = 0; i < 9; ++i)
	{
		ZeroMemory(&FontDesc, sizeof(FontDesc));
		FontDesc.iRepresentNum = i;
		pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

		assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

		m_Target3_DamageTenVec.push_back(pFont);
	}

	ZeroMemory(&FontDesc, sizeof(FontDesc));
	FontDesc.iRepresentNum = 9;
	pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

	assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

	m_Target3_DamageTenVec.insert(m_Target3_DamageTenVec.begin(), pFont);


	for (_uint i = 0; i < 9; ++i)
	{
		ZeroMemory(&FontDesc, sizeof(FontDesc));
		FontDesc.iRepresentNum = i;
		pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

		assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

		m_Target3_DamageSingleVec.push_back(pFont);
	}

	ZeroMemory(&FontDesc, sizeof(FontDesc));
	FontDesc.iRepresentNum = 9;
	pFont = (CDamageFont*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DamageFont"), &FontDesc));

	assert(nullptr != pFont && "CDamage_Font_Manager::Initialize");

	m_Target3_DamageSingleVec.insert(m_Target3_DamageSingleVec.begin(), pFont);
	/*~Target_2*/


	RELEASE_INSTANCE(CGameInstance);

	m_bInit = true;

	return S_OK;
}

void CDamage_Font_Manager::Tick(_double TimeDelta)
{
	if (m_bTarget0_FontUpdate)
	{
		m_fFont_Target0_Timer += (_float)TimeDelta;

		for (auto &pFont : m_DamageTenVec)
			pFont->Tick(TimeDelta);

		for (auto &pFont : m_DamageSingleVec)
			pFont->Tick(TimeDelta);
	
		if (m_fFont_Target0_Timer >= 2.0f)
		{
			m_bTarget0_FontUpdate = false;
			m_fFont_Target0_Timer = 0.f;
		}
	}


	if (m_bTarget1_FontUpdate)
	{
		m_fFont_Target1_Timer += (_float)TimeDelta;

		for (auto &pFont : m_Target2_DamageTenVec)
			pFont->Tick(TimeDelta);

		for (auto &pFont : m_Target2_DamageSingleVec)
			pFont->Tick(TimeDelta);

		if (m_fFont_Target1_Timer >= 2.0f)
		{
			m_bTarget1_FontUpdate = false;
			m_fFont_Target1_Timer = 0.f;
		}
	}



	if (m_bTarget2_FontUpdate)
	{
		m_fFont_Target2_Timer += (_float)TimeDelta;

		for (auto &pFont : m_Target3_DamageTenVec)
			pFont->Tick(TimeDelta);

		for (auto &pFont : m_Target3_DamageSingleVec)
			pFont->Tick(TimeDelta);
	
		if (m_fFont_Target2_Timer >= 2.0f)
		{
			m_bTarget2_FontUpdate = false;
			m_fFont_Target2_Timer = 0.f;
		}
	}





}

void CDamage_Font_Manager::Set_Damage_Target0_Font(_float4 vPos, _float3 vScale, _int iDamage,_float vSecontPosX, _float vSecontPosY)
{
	for (auto& pFont : m_DamageTenVec)
	{
		pFont->Set_Shaking(true);
	}

	for (auto& pFont : m_DamageSingleVec)
	{
		pFont->Set_Shaking(true);
	}

	m_bTarget0_FontUpdate = true;
	m_bShaking = true;
	m_vFontFirstPos = vPos;
	m_vScale = vScale;

	m_vFontSecondPos = vPos;
	m_vFontSecondPos.x += vSecontPosX;
	m_vFontSecondPos.z -= vSecontPosY;
	
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

void CDamage_Font_Manager::Set_Damage_Target1_Font(_float4 vPos, _float3 vScale, _int iDamage, _float vSecontPosX, _float vSecontPosY)
{
	for (auto& pFont : m_Target2_DamageTenVec)
	{
		pFont->Set_Shaking(true);
	}

	for (auto& pFont : m_Target2_DamageSingleVec)
	{
		pFont->Set_Shaking(true);
	}

	m_bTarget1_FontUpdate = true;
	m_bShaking = true;
	m_vFontFirstPos1 = vPos;
	m_vScale1 = vScale;

	m_vFontSecondPos1 = vPos;
	m_vFontSecondPos1.x += vSecontPosX;
	m_vFontSecondPos1.z -= vSecontPosY;

	if (iDamage >= 10)
	{
		m_iCalDamage1 = iDamage / 10;
		m_iNumTens1 = m_iCalDamage1;
		m_iNumSingle1 = iDamage % (10 * m_iCalDamage1);
	}
	else
	{
		m_iNumTens1 = 0;
		m_iNumSingle1 = iDamage % 10;
	}

	if (0 == m_iNumTens1 && 0 == m_iNumSingle1)
	{
		return;
	}
	else if (0 == m_iNumTens1 && 0 != m_iNumSingle1)
	{
		m_Target2_DamageSingleVec[m_iNumSingle1]->Render_Font(m_vFontFirstPos1, m_vScale1);
	}
	else
	{
		m_Target2_DamageTenVec[m_iNumTens1]->Render_Font(m_vFontFirstPos1, m_vScale1);
		m_Target2_DamageSingleVec[m_iNumSingle1]->Render_Font(m_vFontSecondPos1, m_vScale1);
	}

}

void CDamage_Font_Manager::Set_Damage_Target2_Font(_float4 vPos, _float3 vScale, _int iDamage, _float vSecontPosX, _float vSecontPosY)
{
	for (auto& pFont : m_Target3_DamageTenVec)
	{
		pFont->Set_Shaking(true);
	}

	for (auto& pFont : m_Target3_DamageSingleVec)
	{
		pFont->Set_Shaking(true);
	}

	m_bTarget2_FontUpdate = true;
	m_bShaking = true;
	m_vFontFirstPos2 = vPos;
	m_vScale2 = vScale;

	m_vFontSecondPos2 = vPos;
	m_vFontSecondPos2.x += vSecontPosX;
	m_vFontSecondPos2.z -= vSecontPosY;

	if (iDamage >= 10)
	{
		m_iCalDamage2 = iDamage / 10;
		m_iNumTens2 = m_iCalDamage2;
		m_iNumSingle2 = iDamage % (10 * m_iCalDamage2);
	}
	else
	{
		m_iNumTens2 = 0;
		m_iNumSingle2 = iDamage % 10;
	}

	if (0 == m_iNumTens2 && 0 == m_iNumSingle2)
	{
		return;
	}
	else if (0 == m_iNumTens2 && 0 != m_iNumSingle2)
	{
		m_Target3_DamageSingleVec[m_iNumSingle2]->Render_Font(m_vFontFirstPos2, m_vScale2);
	}
	else
	{
		m_Target3_DamageTenVec[m_iNumTens2]->Render_Font(m_vFontFirstPos2, m_vScale2);
		m_Target3_DamageSingleVec[m_iNumSingle2]->Render_Font(m_vFontSecondPos2, m_vScale2);
	}
}

void CDamage_Font_Manager::Set_HPMPFont_0(_float4 vPos, _float3 vScale, _int iDamage)
{

	for (auto& pFont : m_DamageTenVec)
	{
		pFont->Set_Shaking(false);
	}

	for (auto& pFont : m_DamageSingleVec)
	{
		pFont->Set_Shaking(false);
	}

	m_bTarget0_FontUpdate = true;
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

	if(m_bTarget0_FontUpdate && pGameInstance->GetCurLevelIdx() == LEVEL_COMBAT)
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

	if(m_bTarget1_FontUpdate && pGameInstance->GetCurLevelIdx() == LEVEL_COMBAT)
	{
		if (0 == m_iNumTens1 && 0 == m_iNumSingle1)
		{
			return;
		}
		else if (0 == m_iNumTens1 && 0 != m_iNumSingle1)
		{
			m_Target2_DamageSingleVec[m_iNumSingle1]->Late_Tick(TimeDelta);
		}
		else
		{
			m_Target2_DamageTenVec[m_iNumTens1]->Late_Tick(TimeDelta);
			m_Target2_DamageSingleVec[m_iNumSingle1]->Late_Tick(TimeDelta);
		}
	}

	if (m_bTarget2_FontUpdate && pGameInstance->GetCurLevelIdx() == LEVEL_COMBAT)
	{
		if (0 == m_iNumTens2 && 0 == m_iNumSingle2)
		{
			return;
		}
		else if (0 == m_iNumTens2 && 0 != m_iNumSingle2)
		{
			m_Target3_DamageSingleVec[m_iNumSingle2]->Late_Tick(TimeDelta);
		}
		else
		{
			m_Target3_DamageTenVec[m_iNumTens2]->Late_Tick(TimeDelta);
			m_Target3_DamageSingleVec[m_iNumSingle2]->Late_Tick(TimeDelta);
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


	for (auto &pFont2 : m_Target2_DamageTenVec)
	{
		Safe_Release(pFont2);
	}
	m_Target2_DamageTenVec.clear();

	for (auto &pFont : m_Target2_DamageSingleVec)
	{
		Safe_Release(pFont);
	}
	m_Target2_DamageSingleVec.clear();


	for (auto &pFont2 : m_Target3_DamageTenVec)
	{
		Safe_Release(pFont2);
	}
	m_Target3_DamageTenVec.clear();

	for (auto &pFont : m_Target3_DamageSingleVec)
	{
		Safe_Release(pFont);
	}
	m_Target3_DamageSingleVec.clear();

}

