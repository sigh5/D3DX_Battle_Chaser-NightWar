#include "..\public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel * pNewLevel, _bool bCopy )
{
	if (nullptr == pNewLevel)
		return E_FAIL;
	
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (false == bCopy)
	{
		if (nullptr != m_pCurrentLevel)
			pGameInstance->Clear_Level(m_iLevelIndex);

		Safe_Release(m_pCurrentLevel);

		m_pCurrentLevel = pNewLevel;
		m_iLevelIndex = iLevelIndex;
	}											/* 레이어에 있는정보들을 카피해야한다.*/
	else
	{
		if (nullptr != m_pCurrentLevel)
			pGameInstance->Copy_Data(m_iLevelIndex);

		Safe_Release(m_pCurrentLevel);
		m_pCurrentLevel = pNewLevel;
		m_iLevelIndex = iLevelIndex;
	}

	Safe_Release(pGameInstance);
	return S_OK;
}



void CLevel_Manager::Tick(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(TimeDelta);
}

void CLevel_Manager::Late_Tick(_double TimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Late_Tick(TimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

void CLevel_Manager::Scene_Change()
{
	
	 m_pCurrentLevel->Scene_Change();
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
