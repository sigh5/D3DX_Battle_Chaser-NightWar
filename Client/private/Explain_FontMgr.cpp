#include "stdafx.h"
#include "..\public\Explain_FontMgr.h"
#include "Explain_Font.h"
#include "GameInstance.h"
#include "Debuff_Font.h"

IMPLEMENT_SINGLETON(CExplain_FontMgr)

CExplain_FontMgr::CExplain_FontMgr()
{
}
/* First*/
void CExplain_FontMgr::Set_Explain_Target0_Font0(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget0_FontExplain_Update = true;

	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Explain_Target0_Font0");
	}

	size_t vecSize = m_vecExplainTarget0_Font0.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecExplainTarget0_Font0[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.5f;
		vNewPos.z -= 0.4f;
		m_vecExplainTarget0_Font0[i]->Render_Font(vNewPos, vScale, szName[i]);
	}

}

void CExplain_FontMgr::Set_Explain_Target0_Font1(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget0_Font1Explain_Update = true;

	//m_vScale = vScale;

	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Explain_Target0_Font0");
	}

	size_t vecSize = m_vecExplainTarget0_Font1.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecExplainTarget0_Font1[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.4f;
		vNewPos.z -= 0.5f;
		m_vecExplainTarget0_Font1[i]->Render_Font(vNewPos, vScale, szName[i]);
	}

}

void CExplain_FontMgr::Set_Debuff_Target0_Font(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget0_FontDebuff_Update = true;
	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Explain_Target0_Font0");
	}

	size_t vecSize = m_vecTarget0_DeBuffFont.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecTarget0_DeBuffFont[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.3f;
		vNewPos.z -= 0.6f;
		m_vecTarget0_DeBuffFont[i]->Render_Font(vNewPos, vScale, szName[i]);
	}

}

void CExplain_FontMgr::Reset_Explain_Target0_Font0()
{
	for (auto& pFont : m_vecExplainTarget0_Font0)
		pFont->Set_RenderActive(false);
}

void CExplain_FontMgr::Reset_Explain_Target0_Font1()
{
	for (auto& pFont : m_vecExplainTarget0_Font1)
		pFont->Set_RenderActive(false);
}

void CExplain_FontMgr::Reset_Debuff_Target0_Font()
{
	for (auto& pFont : m_vecTarget0_DeBuffFont)
		pFont->Set_RenderActive(false);
}
/*~First*/

/*Second*/
void CExplain_FontMgr::Set_Explain_Target1_Font0(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget1_FontExplain_Update = true;
	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Explain_Target1_Font0");
	}

	size_t vecSize = m_vecExplainTarget1_Font0.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecExplainTarget1_Font0[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.5f;
		vNewPos.z -= 0.4f;
		m_vecExplainTarget1_Font0[i]->Render_Font(vNewPos, vScale, szName[i]);
	}

}

void CExplain_FontMgr::Set_Explain_Target1_Font1(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget1_Font1Explain_Update = true;
	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Explain_Target1_Font1");
	}

	size_t vecSize = m_vecExplainTarget1_Font1.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecExplainTarget1_Font1[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.4f;
		vNewPos.z -= 0.5f;
		m_vecExplainTarget1_Font1[i]->Render_Font(vNewPos, vScale, szName[i]);
	}
}

void CExplain_FontMgr::Set_Debuff_Target1_Font(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget1_FontDebuff_Update = true;

	//m_vScale = vScale;

	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"m_vecTarget1_DeBuffFont");
	}

	size_t vecSize = m_vecTarget1_DeBuffFont.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecTarget1_DeBuffFont[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.3f;
		vNewPos.z -= 0.6f;
		m_vecTarget1_DeBuffFont[i]->Render_Font(vNewPos, vScale, szName[i]);
	}
}

void CExplain_FontMgr::Reset_Explain_Target1_Font0()
{
	for (auto& pFont : m_vecExplainTarget1_Font0)
		pFont->Set_RenderActive(false);
}

void CExplain_FontMgr::Reset_Explain_Target1_Font1()
{
	for (auto& pFont : m_vecExplainTarget1_Font1)
		pFont->Set_RenderActive(false);
}

void CExplain_FontMgr::Reset_Debuff_Target1_Font()
{
	for (auto& pFont : m_vecTarget1_DeBuffFont)
		pFont->Set_RenderActive(false);
}
/*~Second*/

/*Third*/
void CExplain_FontMgr::Set_Explain_Target2_Font0(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget2_FontExplain_Update = true;
	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"m_vecExplainTarget2_Font0");
	}

	size_t vecSize = m_vecExplainTarget2_Font0.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecExplainTarget2_Font0[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.5f;
		vNewPos.z -= 0.4f;
		m_vecExplainTarget2_Font0[i]->Render_Font(vNewPos, vScale, szName[i]);
	}
}

void CExplain_FontMgr::Set_Explain_Target2_Font1(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget2_Font1Explain_Update = true;
	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Explain_Target1_Font1");
	}

	size_t vecSize = m_vecExplainTarget2_Font1.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecExplainTarget2_Font1[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.4f;
		vNewPos.z -= 0.5f;
		m_vecExplainTarget2_Font1[i]->Render_Font(vNewPos, vScale, szName[i]);
	}
}

void CExplain_FontMgr::Set_Debuff_Target2_Font(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bTarget2_FontDebuff_Update = true;

	//m_vScale = vScale;

	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Debuff_Target2_Font");
	}

	size_t vecSize = m_vecTarget2_DeBuffFont.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecTarget2_DeBuffFont[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.3f;
		vNewPos.z -= 0.6f;
		m_vecTarget2_DeBuffFont[i]->Render_Font(vNewPos, vScale, szName[i]);
	}
}
void CExplain_FontMgr::Reset_Explain_Target2_Font0()
{
	for (auto& pFont : m_vecExplainTarget2_Font0)
		pFont->Set_RenderActive(false);
}
void CExplain_FontMgr::Reset_Explain_Target2_Font1()
{
	for (auto& pFont : m_vecExplainTarget2_Font1)
		pFont->Set_RenderActive(false);
}
void CExplain_FontMgr::Reset_Debuff_Target2_Font()
{
	for (auto& pFont : m_vecTarget2_DeBuffFont)
		pFont->Set_RenderActive(false);
}
/*~Third*/

HRESULT CExplain_FontMgr::Initialize()
{
	if (m_bInit)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CExplain_Font* pFont = nullptr;
	CDebuff_Font* pDebuffFont = nullptr;
	/*First_Target*/
	for (_uint i = 0; i < 22; ++i)
	{
		pFont = (CExplain_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Explain_Font")));
		assert(nullptr != pFont && "CExplain_FontMgr::Initialize");
		m_vecExplainTarget0_Font0.push_back(pFont);
	}

	pFont = nullptr;
	for (_uint i = 0; i < 22; ++i)
	{
		pFont = (CExplain_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Explain_Font")));
		assert(nullptr != pFont && "CExplain_FontMgr::Initialize");
		m_vecExplainTarget0_Font1.push_back(pFont);
	}

	for (_uint i = 0; i < 22; ++i)
	{
		pDebuffFont = (CDebuff_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Debuff_Font")));
		assert(nullptr != pFont && "pDebuffFont::Initialize");
		m_vecTarget0_DeBuffFont.push_back(pDebuffFont);
	}
	/*~First_Target*/

	/*Second_Target*/
	pFont = nullptr;
	for (_uint i = 0; i < 22; ++i)
	{
		pFont = (CExplain_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Explain_Font")));
		assert(nullptr != pFont && "CExplain_FontMgr::Initialize");
		m_vecExplainTarget1_Font0.push_back(pFont);
	}

	pFont = nullptr;
	for (_uint i = 0; i < 22; ++i)
	{
		pFont = (CExplain_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Explain_Font")));
		assert(nullptr != pFont && "CExplain_FontMgr::Initialize");
		m_vecExplainTarget1_Font1.push_back(pFont);
	}


	pDebuffFont = nullptr;
	for (_uint i = 0; i < 22; ++i)
	{
		pDebuffFont = (CDebuff_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Debuff_Font")));
		assert(nullptr != pFont && "pDebuffFont::Initialize");
		m_vecTarget1_DeBuffFont.push_back(pDebuffFont);
	}
	/*~Second_Target*/


	/*Third_Target*/
	pFont = nullptr;
	for (_uint i = 0; i < 22; ++i)
	{
		pFont = (CExplain_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Explain_Font")));
		assert(nullptr != pFont && "CExplain_FontMgr::Initialize");
		m_vecExplainTarget2_Font0.push_back(pFont);
	}

	pFont = nullptr;
	for (_uint i = 0; i < 22; ++i)
	{
		pFont = (CExplain_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Explain_Font")));
		assert(nullptr != pFont && "CExplain_FontMgr::Initialize");
		m_vecExplainTarget2_Font1.push_back(pFont);
	}


	pDebuffFont = nullptr;
	for (_uint i = 0; i < 22; ++i)
	{
		pDebuffFont = (CDebuff_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Debuff_Font")));
		assert(nullptr != pFont && "pDebuffFont::Initialize");
		m_vecTarget2_DeBuffFont.push_back(pDebuffFont);
	}
	/*~Third_Target*/


	RELEASE_INSTANCE(CGameInstance);
	m_bInit = true;
	return S_OK;
}

void CExplain_FontMgr::Tick(_double TimeDelta)
{
	///*First*/
	if (m_bTarget0_FontExplain_Update)
	{
		m_fTarget0_FontTimer += (_float)TimeDelta;
		for (auto& pFont : m_vecExplainTarget0_Font0)
			pFont->Tick(TimeDelta);

		if (m_fTarget0_FontTimer >= 2.0f)
		{
			Reset_Explain_Target0_Font0();
			m_bTarget0_FontExplain_Update = false;
			m_fTarget0_FontTimer = 0.f;
		}
	}

	if (m_bTarget0_Font1Explain_Update)
	{
		m_fTarget0_Font1_Timer += (_float)TimeDelta;
		for (auto& pFont : m_vecExplainTarget0_Font1)
			pFont->Tick(TimeDelta);

		if (m_fTarget0_Font1_Timer >= 2.0f)
		{
			Reset_Explain_Target0_Font1();
			m_bTarget0_Font1Explain_Update = false;
			m_fTarget0_Font1_Timer = 0.f;
		}
	}

	
	if (m_bTarget0_FontDebuff_Update)
	{
		m_fTarget0_DebuffFontTimer += (_float)TimeDelta;
		for (auto& pFont : m_vecTarget0_DeBuffFont)
			pFont->Tick(TimeDelta);

		if (m_fTarget0_DebuffFontTimer >= 2.0f)
		{
			Reset_Debuff_Target0_Font();
			m_bTarget0_FontDebuff_Update = false;
			m_fTarget0_DebuffFontTimer = 0.f;
		}
	}
	
	/*~First*/

	/*Second*/
	if (m_bTarget1_FontExplain_Update)
	{
		m_fTarget1_FontTimer += (_float)TimeDelta;
		for (auto& pFont : m_vecExplainTarget1_Font0)
			pFont->Tick(TimeDelta);

		if (m_fTarget1_FontTimer >= 2.0f)
		{
			Reset_Explain_Target1_Font0();
			m_bTarget1_FontExplain_Update = false;
			m_fTarget1_FontTimer = 0.f;
		}
	}

	if (m_bTarget1_Font1Explain_Update)
	{
		m_fTarget1_Font1_Timer += (_float)TimeDelta;
		for (auto& pFont : m_vecExplainTarget1_Font1)
			pFont->Tick(TimeDelta);

		if (m_fTarget1_Font1_Timer >= 2.0f)
		{
			Reset_Explain_Target1_Font1();
			m_bTarget1_Font1Explain_Update = false;
			m_fTarget1_Font1_Timer = 0.f;
		}
	}


	if (m_bTarget1_FontDebuff_Update)
	{
		m_fTarget1_DebuffFontTimer += (_float)TimeDelta;
		for (auto& pFont : m_vecTarget1_DeBuffFont)
			pFont->Tick(TimeDelta);

		if (m_fTarget1_DebuffFontTimer >= 2.0f)
		{
			Reset_Debuff_Target1_Font();
			m_bTarget1_FontDebuff_Update = false;
			m_fTarget1_DebuffFontTimer = 0.f;
		}
	}
	/*~Second*/

	/*Third*/
	if (m_bTarget2_FontExplain_Update)
	{
		m_fTarget2_FontTimer += (_float)TimeDelta;
		for (auto& pFont : m_vecExplainTarget2_Font0)
			pFont->Tick(TimeDelta);

		if (m_fTarget2_FontTimer >= 2.0f)
		{
			Reset_Explain_Target2_Font0();
			m_bTarget2_FontExplain_Update = false;
			m_fTarget2_FontTimer = 0.f;
		}
	}

	if (m_bTarget2_Font1Explain_Update)
	{
		m_fTarget2_Font1_Timer += (_float)TimeDelta;
		for (auto& pFont : m_vecExplainTarget2_Font1)
			pFont->Tick(TimeDelta);

		if (m_fTarget2_Font1_Timer >= 2.0f)
		{
			Reset_Explain_Target2_Font1();
			m_bTarget2_Font1Explain_Update = false;
			m_fTarget2_Font1_Timer = 0.f;
		}
	}


	if (m_bTarget2_FontDebuff_Update)
	{
		m_fTarget2_DebuffFontTimer += (_float)TimeDelta;
		for (auto& pFont : m_vecTarget2_DeBuffFont)
			pFont->Tick(TimeDelta);

		if (m_fTarget2_DebuffFontTimer >= 2.0f)
		{
			Reset_Debuff_Target2_Font();
			m_bTarget2_FontDebuff_Update = false;
			m_fTarget2_DebuffFontTimer = 0.f;
		}
	}
	/*~Third*/


}

void CExplain_FontMgr::Late_Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	/*First*/
	for (auto& pFont : m_vecExplainTarget0_Font0)
		pFont->Late_Tick(TimeDelta);

	for (auto& pFont : m_vecExplainTarget0_Font1)
		pFont->Late_Tick(TimeDelta);

	for (auto& pFont : m_vecTarget0_DeBuffFont)
		pFont->Late_Tick(TimeDelta);
	/*~First*/

	/*Second*/
	for (auto& pFont : m_vecExplainTarget1_Font0)
		pFont->Late_Tick(TimeDelta);

	for (auto& pFont : m_vecExplainTarget1_Font1)
		pFont->Late_Tick(TimeDelta);

	for (auto& pFont : m_vecTarget1_DeBuffFont)
		pFont->Late_Tick(TimeDelta);
	/*~Second*/

	/*Third*/
	for (auto& pFont : m_vecExplainTarget2_Font0)
		pFont->Late_Tick(TimeDelta);

	for (auto& pFont : m_vecExplainTarget2_Font1)
		pFont->Late_Tick(TimeDelta);

	for (auto& pFont : m_vecTarget2_DeBuffFont)
		pFont->Late_Tick(TimeDelta);
	/*~Third*/

	RELEASE_INSTANCE(CGameInstance);
}

void CExplain_FontMgr::Free()
{
	for (auto& pFont : m_vecExplainTarget0_Font0)
		Safe_Release(pFont);
	m_vecExplainTarget0_Font0.clear();

	for (auto& pFont : m_vecExplainTarget0_Font1)
		Safe_Release(pFont);
	m_vecExplainTarget0_Font1.clear();

	for (auto& pFont : m_vecTarget0_DeBuffFont)
		Safe_Release(pFont);
	m_vecTarget0_DeBuffFont.clear();


	for (auto& pFont : m_vecExplainTarget1_Font0)
		Safe_Release(pFont);
	m_vecExplainTarget1_Font0.clear();

	for (auto& pFont : m_vecExplainTarget1_Font1)
		Safe_Release(pFont);
	m_vecExplainTarget1_Font1.clear();

	for (auto& pFont : m_vecTarget1_DeBuffFont)
		Safe_Release(pFont);
	m_vecTarget1_DeBuffFont.clear();


	for (auto& pFont : m_vecExplainTarget2_Font0)
		Safe_Release(pFont);
	m_vecExplainTarget2_Font0.clear();

	for (auto& pFont : m_vecExplainTarget2_Font1)
		Safe_Release(pFont);
	m_vecExplainTarget2_Font1.clear();

	for (auto& pFont : m_vecTarget2_DeBuffFont)
		Safe_Release(pFont);
	m_vecTarget2_DeBuffFont.clear();



}
