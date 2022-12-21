#include "stdafx.h"
#include "..\public\CombatController.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Canvas.h"

#include "Player.h"
#include "Monster.h"

#include "TurnStateCanvas.h"
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

	for (auto& pCanvas : m_CanvasVec)
	{
		if (nullptr != dynamic_cast<CTurnUICanvas*>(pCanvas))
			m_pTurnCanvas = static_cast<CTurnUICanvas*>(pCanvas);
		
		if (nullptr != dynamic_cast<CTurnStateCanvas*>(pCanvas))
			m_pTurnStateButtonCanvas = static_cast<CTurnStateCanvas*>(pCanvas);

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
	{
		To_Intro();
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}

	// 몬스터 Fsm 완성전까지 주석 /*필요한 코드 삭제x*/
	//if (nullptr != dynamic_cast<CMonster*>(m_pCurentActor))
	//	m_pTurnStateButtonCanvas->Set_RenderActive(false);

	if (m_pGameInstace->Mouse_Up(CInput_Device::DIM_LB))
	{
		PickingTarget();
	}



	 Active_Fsm();
}

void CCombatController::Refresh_CurActor()
{
	Set_CurrentActor();
}

void CCombatController::Active_Fsm()
{
	To_Idle();
	To_Intro();
	
	if (m_pHitActor != nullptr)
	{
		To_Normal_Attack();
		To_Skill1_Attack();
		To_Skill2_Attack();
		To_Uitimate();
	}
	
	To_Buff();
	To_WideAreaBuff();
	To_Use_Item();
	To_Defence();
	To_Light_Hit();
	To_Heavy_Hit();
	To_Flee();
	To_Die();
	To_Viroty();
}

void CCombatController::ResetState()
{
	if (m_pCurentActor == nullptr)
		return;
	for (_uint i = 0; i < (_uint)(CGameObject::CHAR_STATE_END); ++i)
	{
		m_pCurentActor->Set_FsmState(false, (CGameObject::CHAR_STATE)(i));
	}
}

void CCombatController::PickingTarget()
{
	
	if (nullptr == dynamic_cast<CPlayer*>(m_pCurentActor))
		return;

	for (auto& pMonster : m_CurActorMap)
	{
		if (nullptr == dynamic_cast<CMonster*>(pMonster.second))
			continue;

		if (static_cast<CMonster*>(pMonster.second)->IsCollMouse())
		{
			m_pHitActor = pMonster.second;
			static_cast<CPlayer*>(m_pCurentActor)->Set_HitTarget(m_pHitActor);
			break;
		}
	}
	


}

HRESULT CCombatController::Set_CurrentActor()
{
	assert(m_pTurnCanvas != nullptr && "CCombatController::Set_CurrentActor issue");

	UI_REPRESENT eCurActor = m_pTurnCanvas->Get_CurrentActor();
	
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

	ImGui::InputInt("Anim Type MAX=10", &Type_Num);

	if (Type_Num >= 14)
		Type_Num = 0;
	ImGui::Text(str[Type_Num].c_str());
}

#endif // !DEBUG

_bool CCombatController::To_Idle()
{
	_bool bResult = false;

	if (m_pGameInstace->Key_Down(DIK_1))
	{
		bResult = true;
	}
	return bResult;
}

_bool CCombatController::To_Intro()
{
	_bool bResult = false;

	if (m_bCombatIntro)
		bResult = true;
	
	return bResult;
}

void CCombatController::To_Normal_Attack()
{
	if (nullptr == m_pHitActor	 && m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		return;

	
	if (m_pTurnStateButtonCanvas->Get_ButtonState()== BUTTON_STATE_ACTION 
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Normal_Attack);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		m_pHitActor = nullptr;
	}
}

void CCombatController::To_Skill1_Attack()
{
	if (nullptr == m_pHitActor	 && m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		return;

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_SKILL1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill1_Attack);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		m_pHitActor = nullptr;
	}
}

void CCombatController::To_Skill2_Attack()
{
	if (nullptr == m_pHitActor	 && m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		return;

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_SKILL2)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill2_Attack);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		m_pHitActor = nullptr;
	}
}

void CCombatController::To_Uitimate()
{
	if (nullptr == m_pHitActor	 && m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		return;

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_ULTIMATE)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Uitimate);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		m_pHitActor = nullptr;
	}
}

void CCombatController::To_Buff()
{
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_BUFF)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Buff);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
}

void CCombatController::To_WideAreaBuff()
{
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_WIDEBUFF)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_WideAreaBuff);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}

}
	
void CCombatController::To_Use_Item()
{
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ITEM
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_USEITEM)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Use_Item);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	
}

void CCombatController::To_Defence()
{
	/*어쩃든 턴을 쓴거니까*/
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_FSM_NORMALATTACK
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_DEFENCE)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Defence);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}

}

void CCombatController::To_Light_Hit()
{
	/* 맞는거는 히팅 되는 애들을 추적해야하는데 이걸할려면 피킹이 필요함*/
	if (m_pGameInstace->Key_Down(DIK_O))
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Light_Hit);
	}
}

void CCombatController::To_Heavy_Hit()
{
	/* 맞는거는 히팅 되는 애들을 추적해야하는데 이걸할려면 피킹이 필요함*/
	if (m_pGameInstace->Key_Down(DIK_I))
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Heavy_Hit);
		
	}
}

void CCombatController::To_Flee()
{
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_FSM_USEITEM
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_FLEE)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Flee);
		m_pTurnCanvas->Move_Children();

		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
	}
}

void CCombatController::To_Die()
{
	if (m_pGameInstace->Key_Down(DIK_U))
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Die);
}

void CCombatController::To_Viroty()
{
	if (m_pGameInstace->Key_Down(DIK_L))
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Viroty);
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