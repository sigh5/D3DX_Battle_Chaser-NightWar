#include "stdafx.h"
#include "PlayerController.h"

#include "Player.h"
#include "GameInstance.h"
#include "Layer.h"

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
		m_pCaptinPlayer = nullptr;
		for (auto& pPlayer : m_pPlayerVec)
		{
			pPlayer->Set_FollowTarget(nullptr);
		}
	}
	else if(iLevelIndex == LEVEL_GAMEPLAY)
	{
		Initialize(LEVEL_GAMEPLAY);
	}
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




