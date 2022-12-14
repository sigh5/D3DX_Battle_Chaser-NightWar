#include "stdafx.h"
#include "PlayerController.h"

#include "Player.h"
#include "GameInstance.h"
#include "Layer.h"

// 디버그용
static string str[14] = { {"STATE_INTRO"},{ "STATE_NORMAL_ATTACK" },{ "STATE_SKILL_ATTACK1" },
						{ "STATE_SKILL_ATTACK2" },{ "STATE_UlTIMATE" },{ "STATE_BUFF" },
						  {"STATE_WIDEAREA_BUFF"},{ "STATE_USE_ITEM" },{ "STATE_DEFENCE" },
							{"STATE_LIGHT_HIT"},{ "STATE_HEAVY_HIT" },{ "STATE_FLEE" },
							{"STATE_DIE"},{ "STATE_VITORY" } };
			


IMPLEMENT_SINGLETON(CPlayerController);

HRESULT CPlayerController::Initialize()
{
	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

	CGameObject *pTarget = nullptr;
	_int iIndex = 0;
	for (auto& Pair : pGameInstace->Get_Layer()[pGameInstace->GetCurLevelIdx()])
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
			
			/*Test*/
			if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Calibretto")))
			{
				pCurentActor = pPlayer;
			}
			/*Test*/
		}
	}
	else if(iLevelIndex == LEVEL_GAMEPLAY)
	{
		Initialize();
	}
}

_bool CPlayerController::CombatScene()
{
	if (m_iCurrentScene == LEVEL_COMBAT)
		return true;

	return false;
}

void CPlayerController::CurrentTurn_AnimControl()
{
	static int CharIndex = 0;
	static int Type_Num = 0;

	ImGui::RadioButton("Garrison", &CharIndex, 0);
	ImGui::RadioButton("Knolan", &CharIndex, 1);
	ImGui::RadioButton("Calibreotto", &CharIndex, 2);
	if (CharIndex == 0)
	{
		for (auto& pPlayer : m_pPlayerVec)
		{
			if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Alumon")))
			{
				pCurentActor = pPlayer;
			}
		}
	}
	else if (CharIndex == 1)
	{
		for (auto& pPlayer : m_pPlayerVec)
		{
			if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Gully")))
			{
				pCurentActor = pPlayer;
			}
		}
	}
	else if (CharIndex == 2)
	{
		for (auto& pPlayer : m_pPlayerVec)
		{
			if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Calibretto")))
			{
				pCurentActor = pPlayer;
			}
		}
	}

	ImGui::InputInt("Anim Type MAX=10", &Type_Num);

	if (Type_Num >= 14)
		Type_Num = 0;
	ImGui::Text(str[Type_Num].c_str());


	/*CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_P))
	{
		static_cast<CPlayer*>(pCurentActor)->Set_Current_AnimQueue(CPlayer::CurrentState(Type_Num), true);
	}

	RELEASE_INSTANCE(CGameInstance);*/
	
}

_bool CPlayerController::To_Idle()
{
	_bool bResult = false;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_1))
	{
		bResult=  true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return bResult;
}

_bool CPlayerController::To_Intro()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_2))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}
_bool CPlayerController::To_Normal_Attack()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_3))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Skill1_Attack()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_4))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Skill2_Attack()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_5))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Uitimate()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_6))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Buff()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_7))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_WideAreaBuff()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_8))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Use_Item()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_9))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Defence()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_P))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Light_Hit()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_O))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Heavy_Hit()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_I))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Flee()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_U))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

_bool CPlayerController::To_Die()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_Y))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
	
}

_bool CPlayerController::To_Viroty()
{
	_bool bResult = false;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_L))
		bResult = true;

	RELEASE_INSTANCE(CGameInstance);

	return bResult;
}

void CPlayerController::Free()
{
	for (auto& pPlayer : m_pPlayerVec)
	{
		Safe_Release(pPlayer);
	}
}




