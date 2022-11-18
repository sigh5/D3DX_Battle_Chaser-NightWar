#include "..\public\Level.h"
#include "Player.h"
CLevel::CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_double TimeDelta)
{
}

void CLevel::Late_Tick(_double TimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

void CLevel::ClearBroadCaster()
{
	for (auto Pair : m_mapBroadCasters)
	{
		Safe_Release(Pair.second);
	}
	m_mapBroadCasters.clear();
}

void CLevel::Add_BroadCaster(const wstring & pBroadCasterTag, CGameObject * pBroadCaster)
{
	if (nullptr == pBroadCaster)
		return;

	Safe_AddRef(pBroadCaster);
	m_mapBroadCasters.emplace(pBroadCasterTag, pBroadCaster);
}

CGameObject * CLevel::Get_BroadCaster(const wstring & pBroadCasterTag)
{
	CGameObject* pGameObject = Find_BroadCaster(pBroadCasterTag);

	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

CGameObject * CLevel::Find_BroadCaster(const wstring & pBroadCasterTag)
{
	auto iter = find_if(m_mapBroadCasters.begin(), m_mapBroadCasters.end(), [&](auto &Pair)->bool
	{
		if (Pair.first == pBroadCasterTag)
			return true;
		return false;
	});

	if (iter == m_mapBroadCasters.end())
		return nullptr;

	return iter->second;
}

void CLevel::Free()
{
	ClearBroadCaster();
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

