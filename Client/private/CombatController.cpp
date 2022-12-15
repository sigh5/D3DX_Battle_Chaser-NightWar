#include "stdafx.h"
#include "..\public\CombatController.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Canvas.h"

#include "Player.h"
#include "Monster.h"

#include "TurnUICanvas.h"
#include <string>

#ifndef DEBUG

static string str[14] = { { "STATE_INTRO" },{ "STATE_NORMAL_ATTACK" },{ "STATE_SKILL_ATTACK1" },
{ "STATE_SKILL_ATTACK2" },{ "STATE_UlTIMATE" },{ "STATE_BUFF" },
{ "STATE_WIDEAREA_BUFF" },{ "STATE_USE_ITEM" },{ "STATE_DEFENCE" },
{ "STATE_LIGHT_HIT" },{ "STATE_HEAVY_HIT" },{ "STATE_FLEE" },
{ "STATE_DIE" },{ "STATE_VITORY" } };

#endif // !_DEBUG

IMPLEMENT_SINGLETON(CCombatController);


CCombatController::CCombatController()
	:m_pGameInstace{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstace);
}

HRESULT CCombatController::Initialize(_uint iLevel)
{
	wstring ObjTag = TEXT("");

	for (auto& Pair : m_pGameInstace->Get_Layer()[iLevel])
	{
		for (auto& obj : Pair.second->GetGameObjects())
		{
			if (nullptr != dynamic_cast<CPlayer*>(obj) ||
				nullptr != dynamic_cast<CMonster*>(obj) )
			{

				ObjTag = obj->Get_ObjectName();
				m_CurActorMap.emplace(ObjTag, obj);
				Safe_AddRef(obj);

			}
		}
	}

	for (auto& Pair : m_pGameInstace->Get_Layer()[iLevel])
	{
		for (auto& obj : Pair.second->GetGameObjects())
		{
			if (nullptr != dynamic_cast<CCanvas*>(obj))
			{
				m_CanvasVec.push_back(static_cast<CCanvas*>(obj));
				Safe_AddRef(obj);
			}
		}
	}


	return S_OK;
}

HRESULT CCombatController::Late_Init()
{
	if (m_bLateInit)
		return S_OK;

	Set_CurrentActor();

	m_bLateInit = true;
	return S_OK;
}

void CCombatController::CurrentTurn_ActorControl(_double TimeDelta)
{
	m_dIntroTimer += TimeDelta *1.f;

	if (m_dIntroTimer >= 3.f)
	{
		Late_Init();
		m_bCombatIntro = false;
	}
	else
		To_Intro();

	 CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	 if (pInstance->Key_Down(DIK_SPACE))
	 {
		 /*_uint iMoveSpeed = 100;
		 m_CurActorDelegator.broadcast(iMoveSpeed);
	 }*/
	 }
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCombatController::Set_CurrentActor()
{
	CTurnUICanvas* pTrunCanvas = nullptr;

	for (auto& pCanvas : m_CanvasVec)
	{
		if (dynamic_cast<CTurnUICanvas*>(pCanvas) != nullptr)
		{
			pTrunCanvas = static_cast<CTurnUICanvas*>(pCanvas);
			break;
		}
	}
	assert(pTrunCanvas != nullptr && "CCombatController::Set_CurrentActor issue");

	UI_REPRESENT eCurActor = pTrunCanvas->Get_CurrentActor();
	
	switch (eCurActor)
	{
	case Client::REPRESENT_KNOLAN:
		m_pCurentActor = Find_CurActor(TEXT("Hero_Gully"));
		break;
	case Client::REPRESENT_GARRISON:
		m_pCurentActor = Find_CurActor(TEXT("Hero_Alumon"));
		break;
	case Client::REPRESENT_CALIBRETTO:
		m_pCurentActor = Find_CurActor(TEXT("Hero_Calibretto"));
		break;
	case Client::REPRESENT_SKELTON_NAKED:
		m_pCurentActor = Find_CurActor(TEXT("Skeleton_Naked"));
		break;
	case Client::REPRESENT_SLIMEKING:
		m_pCurentActor = Find_CurActor(TEXT("Monster_SlimeKing"));
		break;
	case Client::REPRESENT_SPIDER_MANA:
		m_pCurentActor = Find_CurActor(TEXT("Spider_Mana"));
		break;
	case Client::REPRESENT_END:
		break;
	default:
		break;
	}


	return S_OK;
}

#ifndef DEBUG
void CCombatController::Imgui_CharAnim()
{
	static int CharIndex = 0;
	static int Type_Num = 0;

	ImGui::RadioButton("Garrison", &CharIndex, 0);
	ImGui::RadioButton("Knolan", &CharIndex, 1);
	ImGui::RadioButton("Calibreotto", &CharIndex, 2);
	ImGui::RadioButton("Slime", &CharIndex, 3);
	ImGui::RadioButton("Skeleton_Naked", &CharIndex, 4);
	ImGui::RadioButton("Spider_Mana", &CharIndex, 5);
	//if (CharIndex == 0)
	//{
	//	for (auto& pPlayer : m_CombatActors)
	//	{
	//		if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Alumon")))
	//		{
	//			m_pCurentActor = pPlayer;
	//		}
	//	}
	//}
	//else if (CharIndex == 1)
	//{
	//	for (auto& pPlayer : m_CombatActors)
	//	{
	//		if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Gully")))
	//		{
	//			m_pCurentActor = pPlayer;
	//		}
	//	}
	//}
	//else if (CharIndex == 2)
	//{
	//	for (auto& pPlayer : m_CombatActors)
	//	{
	//		if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Hero_Calibretto")))
	//		{
	//			m_pCurentActor = pPlayer;
	//		}
	//	}
	//}

	//else if (CharIndex == 3)
	//{
	//	for (auto& pPlayer : m_CombatActors)
	//	{
	//		if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Monster_SlimeKing")))
	//		{
	//			m_pCurentActor = pPlayer;
	//		}
	//	}
	//}
	//else if (CharIndex == 4)
	//{
	//	for (auto& pPlayer : m_CombatActors)
	//	{
	//		if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Skeleton_Naked")))
	//		{
	//			m_pCurentActor = pPlayer;
	//		}
	//	}
	//}

	//else if (CharIndex == 5)
	//{
	//	for (auto& pPlayer : m_CombatActors)
	//	{
	//		if (!lstrcmp(pPlayer->Get_ObjectName(), TEXT("Spider_Mana")))
	//		{
	//			m_pCurentActor = pPlayer;
	//		}
	//	}
	//}


	ImGui::InputInt("Anim Type MAX=10", &Type_Num);

	if (Type_Num >= 14)
		Type_Num = 0;
	ImGui::Text(str[Type_Num].c_str());
}

#endif // !DEBUG



_bool CCombatController::Test()
{
	for (auto& Pair : m_CurActorMap)
	{
		if (!lstrcmp(m_pCurentActor->Get_ObjectName(), Pair.first.c_str()))
			return true;
	}
	
	return false;
}

_bool CCombatController::To_Idle()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_1))
		bResult = true;
	
	return bResult;
}

_bool CCombatController::To_Intro()
{
	_bool bResult = false;

	if (m_bCombatIntro)
		bResult = true;
	
	return bResult;
}

_bool CCombatController::To_Normal_Attack()
{
	_bool bResult = false;
	if (m_pGameInstace->Key_Down(DIK_3))
		bResult = Test();

	return bResult;
}

_bool CCombatController::To_Skill1_Attack()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_4))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_Skill2_Attack()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_5))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_Uitimate()
{
	_bool bResult = false;
	if (m_pGameInstace->Key_Down(DIK_6))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_Buff()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_7))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_WideAreaBuff()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_8))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_Use_Item()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_9))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_Defence()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_P))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_Light_Hit()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_O))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_Heavy_Hit()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_I))
		bResult = true;


	return bResult;
}

_bool CCombatController::To_Flee()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_U))
		bResult = true;

	return bResult;
}

_bool CCombatController::To_Die()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_Y))
		bResult = true;

	return bResult;

}

_bool CCombatController::To_Viroty()
{
	_bool bResult = false;

	
	if (m_pGameInstace->Key_Down(DIK_L))
		bResult = true;


	return bResult;
}


CGameObject * CCombatController::Find_CurActor(const wstring& pNameTag)
{
	auto Pair = find_if(m_CurActorMap.begin(), m_CurActorMap.end(), [&](auto MyPair)->bool
	{
		if (MyPair.first == pNameTag)
			return true;
		return	false;
	});

	if (Pair == m_CurActorMap.end())
		assert("CCombatController_Find_CurActor_issue");
	return Pair->second;
}

void CCombatController::Free()
{
	for (auto& pActor : m_CurActorMap)
		Safe_Release(pActor.second);
	m_CurActorMap.clear();


	for (auto& pCanvas : m_CanvasVec)
		Safe_Release(pCanvas);
	m_CanvasVec.clear();

	Safe_Release(m_pGameInstace);

}