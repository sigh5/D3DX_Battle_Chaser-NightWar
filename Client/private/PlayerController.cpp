#include "stdafx.h"
#include "PlayerController.h"

#include "Player.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Inventory.h"
#include "Dungeon_Canvas.h"

IMPLEMENT_SINGLETON(CPlayerController);

HRESULT CPlayerController::Initialize(_uint iLevel)
{
	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

	CGameObject *pTarget = nullptr;
	_int iIndex = 0;
	for (auto& Pair : pGameInstace->Get_Layer()[iLevel])
	{
		for (auto& obj : Pair.second->GetGameObjects())
		{
			if (nullptr != dynamic_cast<CPlayer*>(obj))
			{
				m_pPlayerVec.push_back((CPlayer*)obj);
				Safe_AddRef(obj);

				if (iIndex == 0)
					m_pCaptinPlayer = static_cast<CPlayer*>(obj);
				
				static_cast<CPlayer*>(obj)->Set_FollowTarget(pTarget);
				pTarget = obj;
				++iIndex;
			}
		}
	}	

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CPlayerController::Late_Initialize()
{
	if (m_bLateInit)
		return S_OK;
	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);	
#ifdef NOMODLES

#else
	m_pInventory = static_cast<CInventory*>(pGameInstace->Get_GameObject(pGameInstace->GetCurLevelIdx(),LAYER_UI,TEXT("Inventory_Canvas")));
	m_pDungeonCanvas = static_cast<CDungeon_Canvas*>(pGameInstace->Get_GameObject(pGameInstace->GetCurLevelIdx(), LAYER_UI, TEXT("DungeonCanvas")));
	

	for (auto& pPlayer : m_pPlayerVec)
	{
		if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Gully")))
		{
			m_pInventory->Set_Knolan(pPlayer);
		}
		else if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Alumon")))
		{
			m_pInventory->Set_Garrison(pPlayer);
		}
		else if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Calibretto")))
		{
			m_pInventory->Set_Calibretto(pPlayer);
		}
	}

#endif
	
	RELEASE_INSTANCE(CGameInstance);
	
	
	m_bLateInit = true;
	
	return S_OK;
}

void CPlayerController::Player_Controll_Tick(_double TimeDelta)
{
	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);
#ifdef NOMODLES

#else
	if (pGameInstace->Key_Down(DIK_I))
	{
		m_bInventoryRender = !m_bInventoryRender;

		m_pInventory->Set_RenderActive(m_bInventoryRender);
		m_pDungeonCanvas->Set_RenderActive(!m_bInventoryRender);

		if (false == m_bInventoryRender)
		{
			m_pInventory->Clear_InventoryImage();
		}


	}
#endif

	RELEASE_INSTANCE(CGameInstance);

}

void CPlayerController::Set_CaptinPlayer()
{
	/* 나중에는 클릭으로 바꾸는 기능도 추가해야함*/
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_TAB))
	{
		CGameObject* pPlayer = m_pCaptinPlayer;

		for (auto iter = m_pPlayerVec.begin(); iter != m_pPlayerVec.end();)
		{
			if (!lstrcmp((*iter)->Get_ObjectName(), m_pCaptinPlayer->Get_ObjectName()))
			{
				static_cast<CPlayer*>(*iter)->Set_ControlInput(false);
				iter = m_pPlayerVec.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		m_pCaptinPlayer = m_pPlayerVec[0];
		m_pCaptinPlayer->Set_ControlInput(true);
		m_pCaptinPlayer->Set_FollowTarget(nullptr);
		m_pPlayerVec.push_back(static_cast<CPlayer*>(pPlayer));

		pPlayer = m_pCaptinPlayer;
		
		for (auto &iter : m_pPlayerVec)
		{
			if (!lstrcmp(iter->Get_ObjectName(), m_pCaptinPlayer->Get_ObjectName()))
			{
				continue;
			}
			iter->Set_FollowTarget(pPlayer);
			pPlayer = iter;
		}
	}
	RELEASE_INSTANCE(CGameInstance);

}

void CPlayerController::SyncAninmation()
{
	_uint iCaptinAnimIndex = m_pCaptinPlayer->Get_AnimationIndex();
	_bool IsFollow = m_pCaptinPlayer->Get_IsSwap();

	for (auto &pPlayer : m_pPlayerVec)
	{
		if (IsFollow == false)
		{
			pPlayer->Set_IsSwap(false);
		}
		pPlayer->SyncAnimation(iCaptinAnimIndex);
	}
}

void CPlayerController::Change_Scene(_uint	iLevelIndex)
{
	m_iCurrentScene = iLevelIndex;

	if (iLevelIndex == LEVEL_COMBAT)
	{
		//m_pCaptinPlayer = nullptr;
		for (auto& pPlayer : m_pPlayerVec)
		{
			pPlayer->Set_FollowTarget(nullptr);
		}

		for (auto& pPlayer : m_pPlayerVec)
		{
			Safe_Release(pPlayer);
		}
		m_pPlayerVec.clear();

	}
	else if (iLevelIndex == LEVEL_GAMEPLAY)
	{
		Initialize(LEVEL_GAMEPLAY);
	}
	else
		return;
}

_bool CPlayerController::CombatScene()
{
	if (m_iCurrentScene == LEVEL_COMBAT)
		return true;

	return false;
}


void CPlayerController::Free()
{
	for (auto& pPlayer : m_pPlayerVec)
	{
		Safe_Release(pPlayer);
	}
	m_pPlayerVec.clear();
}




