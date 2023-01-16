#include "stdafx.h"
#include "..\public\Explain_FontMgr.h"
#include "Explain_Font.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CExplain_FontMgr)

CExplain_FontMgr::CExplain_FontMgr()
{
}

void CExplain_FontMgr::Set_Explain_Font(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bFontUpdate = true;
	
	m_vScale = vScale;

	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Explain_Font");
	}

	size_t vecSize = m_vecExplainFont.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecExplainFont[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.5f;
		vNewPos.z -= 0.4f;
		m_vecExplainFont[i]->Render_Font(vNewPos, vScale, szName[i]);
	}



}

void CExplain_FontMgr::Set_Explain_Font2(_float4 vPos, _float3 vScale, const _tchar * szName)
{
	m_bFontUpdate = true;

	m_vScale = vScale;

	_uint iSzLength = lstrlen(szName);

	if (iSzLength >= 22)
	{
		assert(!"Set_Explain_Font");
	}

	size_t vecSize = m_vecExplainFont.size();

	for (size_t i = iSzLength; i < vecSize; ++i)
	{
		m_vecExplainFont[i]->Set_RenderActive(false);
	}
	_float4 vNewPos;
	XMStoreFloat4(&vNewPos, XMLoadFloat4(&vPos));

	for (_uint i = 0; i < iSzLength; ++i)
	{
		vNewPos.x += 0.4f;
		vNewPos.z -= 0.5f;
		m_vecExplainFont[i]->Render_Font(vNewPos, vScale, szName[i]);
	}

}

void CExplain_FontMgr::Reset_Explain_Font()
{
	for (auto& pFont : m_vecExplainFont)
		pFont->Set_RenderActive(false);
}

HRESULT CExplain_FontMgr::Initialize()
{
	if (m_bInit)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CExplain_Font* pFont = nullptr;
	for (_uint i = 0; i < 22; ++i)
	{
		pFont = (CExplain_Font*)(pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Explain_Font")));
		assert(nullptr != pFont && "CExplain_FontMgr::Initialize");
		m_vecExplainFont.push_back(pFont);
	}

	RELEASE_INSTANCE(CGameInstance);
	m_bInit = true;
	return S_OK;
}

void CExplain_FontMgr::Tick(_double TimeDelta)
{
	if (m_bFontUpdate)
	{
		m_fFontTimer += (_float)TimeDelta;

		for (auto& pFont : m_vecExplainFont)
			pFont->Tick(TimeDelta);
	}
	if (m_fFontTimer >= 1.f)
	{
		Reset_Explain_Font();
		m_bFontUpdate = false;
		m_fFontTimer = 0.f;
	}

	/*static char szFont[MAX_PATH] = "";

	ImGui::InputText("FontText", szFont,MAX_PATH);


	if (ImGui::Button("Font_Render"))
	{
		_tchar Texture_NameTag[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, szFont, strlen(szFont) + 1, Texture_NameTag, MAX_PATH);

		Set_Explain_Font(_float4(1.f, 1.f, 1.f, 1.f), _float3(0.1f, 0.1f, 0.1f), Texture_NameTag);

	}*/




}

void CExplain_FontMgr::Late_Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	for (auto& pFont : m_vecExplainFont)
		pFont->Late_Tick(TimeDelta);

	RELEASE_INSTANCE(CGameInstance);
}

void CExplain_FontMgr::Free()
{
	for (auto& pFont : m_vecExplainFont)
		Safe_Release(pFont);

	m_vecExplainFont.clear();
}
