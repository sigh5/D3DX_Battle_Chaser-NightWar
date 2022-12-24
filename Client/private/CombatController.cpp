#include "stdafx.h"
#include "..\public\CombatController.h"

#include "Client_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "Canvas.h"

#include "GameObject.h"
#include "CombatActors.h"


#include "Player.h"
#include "Monster.h"

#include "TurnStateCanvas.h"
#include "TurnUICanvas.h"
#include "HpMpBuffCanvas.h"
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
			if (nullptr != dynamic_cast<CCombatActors*>(obj))
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

	if (FAILED(Set_ActorsStatus()))
		assert(!"Set_ActorsStatus Error");

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

	// ���� Fsm �ϼ������� �ּ� /*�ʿ��� �ڵ� ����x*/
	//if (nullptr != dynamic_cast<CMonster*>(m_pCurentActor))
	//	m_pTurnStateButtonCanvas->Set_RenderActive(false);

	if (m_pGameInstace->Mouse_Up(CInput_Device::DIM_LB))
	{
		PickingTarget();
	}

	MonsterSetTarget();
	Active_Fsm();
	Collison_Event();
}

void CCombatController::Status_CanvasInit()
{
	for (auto& pCanvas : m_CanvasVec)
	{
		if (dynamic_cast<CHpMpBuffCanvas*>(pCanvas) == nullptr)
			continue;

		pCanvas->Set_RenderActive(true);
	}

}

void CCombatController::Cal_HitPlayerTarget()
{
	if (m_bMonsterSelect_Target)
		return;

	/*if (m_pGameInstace->Key_Down(DIK_V))
	{
	m_pHitActor = Find_CurActor(TEXT("Hero_Gully"));
	static_cast<CCombatActors*>(m_pCurentActor)->Set_HitTarget(m_pHitActor);
	}

	if (m_pGameInstace->Key_Down(DIK_C))
	{
	m_pHitActor = Find_CurActor(TEXT("Hero_Alumon"));
	static_cast<CCombatActors*>(m_pCurentActor)->Set_HitTarget(m_pHitActor);

	}
	if (m_pGameInstace->Key_Down(DIK_X))
	{
	m_pHitActor = Find_CurActor(TEXT("Hero_Calibretto"));
	static_cast<CCombatActors*>(m_pCurentActor)->Set_HitTarget(m_pHitActor);

	}*/

	/*
		���� �켱���� 1) �ǰ� ���� ���� �� ,2) ������ ������ 3) ���� ���� ��
					�����ʿ�
	*/

	_float MinPlayerHPRatio = 999.f;
	_float MinPlayerMPRatio = 999.f;
	CGameObject* pHitTargetMp = nullptr;
	//m_iMonster_Player_Option = 2;
	for (auto& pPlayer : m_CurActorMap)
	{
		if (dynamic_cast<CPlayer*>(pPlayer.second) !=nullptr)
		{
			_float CurPlayerHPRatio =	Find_CurStatus(pPlayer.second->Get_ObjectName())->Get_CurStatusHpRatio();
			_float CurPlayerMPRatio = Find_CurStatus(pPlayer.second->Get_ObjectName())->Get_CurStatusMpRatio();

			if (CurPlayerHPRatio <= MinPlayerHPRatio)
			{
				MinPlayerHPRatio = CurPlayerHPRatio;
				m_pHitActor = pPlayer.second;
			}
			
			if (CurPlayerMPRatio <= MinPlayerMPRatio)
			{
				MinPlayerMPRatio = CurPlayerMPRatio;
				pHitTargetMp = pPlayer.second;
			}
		}
	}



	/*
		�ൿ �켱���� 1) ���� �ǰ� ���������϶� ���, �Ǵ� ���� ,���� �̻��϶� ����
		2) ��ų ������ ������ ��� (�ǰ� �����̻�) �⺻_����, (�ǰ� ����) ���� ���
		3) �����ǰ� 20% �����̰� ������ ���� �̻��� ��� �ñر� Ȱ�� ������ ���� ���� ��ų���
		4) ���� ��ų�� �����ؼ� ���� �ʴ´�.
	*/

	_float fCurActorHpRatio = Find_CurStatus(m_pCurentActor->Get_ObjectName())->Get_CurStatusHpRatio();
	_float fCurActorMpRatio = Find_CurStatus(m_pCurentActor->Get_ObjectName())->Get_CurStatusMpRatio();

	//// 1) ���� �ǰ� �����ΰ�?
	if (fCurActorHpRatio >= 0.5f)
	{	
		// ���� ���̾��� ������ �����̾���.
		if (static_cast<CMonster*>(m_pCurentActor)->Get_HitNum() == 0
			&& false == static_cast<CMonster*>(m_pCurentActor)->Get_IsBuff())		
		{
			To_Buff();
		}
		else if(fCurActorMpRatio >= 0.8f) // ��ų // ��ų ����? 
		{

		}
		else if(fCurActorMpRatio >=0.5f)
		{

		}
	}
	else   // ���� �ǰ� ���� ���ϴ�.
	{
		
		//���� �ǰ� ���� 20%���ϰ� ������ 50�� �̻� ������ �ñر� ���
		if (fCurActorHpRatio <= 0.2f)
		{	
			if (fCurActorMpRatio >= 0.5f)
			{

			}
			else //������ 50�� ���� ��ų���
			{
					
			}
			
		}
		else
		{	
			if (fCurActorMpRatio >= 0.5f)	//���ݽ�ų���
			{

			}
			else if(fCurActorMpRatio <= 0.3f && fCurActorMpRatio >=0.1f)		// ��ų ���
			{
			}
			else						//�⺻���� ���
			{

			}
		}
	}
	//


	//

	static_cast<CCombatActors*>(m_pCurentActor)->Set_HitTarget(m_pHitActor);


	m_bMonsterSelect_Target = true;

}

void CCombatController::Refresh_CurActor()
{
	Set_CurrentActor();

	m_pHitActor = nullptr;
	m_bIsHiterhit = false;
	m_bisHitTimer_Alive = false;
	m_fHitTimer = 0.0f;
}

void CCombatController::Active_Fsm()
{
	if (m_pHitActor == nullptr)
		return;
	To_Idle();
	To_Intro();
	if (m_iMonster_Player_Option == 0)
	{
		To_Normal_Attack();
		To_Skill1_Attack();
		To_Skill2_Attack();
		To_Uitimate();
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

void CCombatController::PickingTarget()		// ��ŷ�� �÷��̾ �����ϴ�.
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
			static_cast<CMonster*>(m_pHitActor)->Set_Me_HitPlayer(m_pCurentActor);
			break;
		}
	}
}

void CCombatController::MonsterSetTarget()
{
	if (nullptr == dynamic_cast<CMonster*>(m_pCurentActor) ||nullptr == m_pCurentActor)
		return;

	Cal_HitPlayerTarget();
}

void CCombatController::Collison_Event()
{
	if (nullptr == m_pHitActor || nullptr == m_pCurentActor)
		return;


	CGameObject* pWeapon = static_cast<CCombatActors*>(m_pCurentActor)->Get_Weapon_Or_SkillBody();


	if (m_fHitTimer >= m_fHitRecoverTime)
	{
		m_bisHitTimer_Alive = false;
		m_fHitTimer = 0.0f;
	}

	if (m_bisHitTimer_Alive)
	{
		m_fHitTimer	+= (_float)CClient_Manager::TimeDelta;
		return;
	}
	if (static_cast<CCombatActors*>(m_pHitActor)->Calculator_HitColl(pWeapon))
	{
		m_bIsHiterhit = true;
		static_cast<CCombatActors*>(m_pHitActor)->HitActor_LoseHP(m_pCurentActor);
		m_bisHitTimer_Alive = true;
		
	}

	// ��� ��ü �ݶ��̴� �� �ʿ䰡����.
	// hiter �ݶ��̴��� , ������ ���� �ݶ��̴��� Ų��. 
	// �ʿ��Ѱ� ������ ����(������ == ��ų��ü, �ѱ�� �Ѿ�) ������
	// �´³� �ݶ��̴� �ٵ� ���� �����ڿ��� �������ߵȴٴ°� �´ٰ�



	//pHiterColl->Collision()
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

HRESULT CCombatController::Set_ActorsStatus()
{
	for (auto& pActors : m_CurActorMap)
	{
		CStatus* pStatus = static_cast<CStatus*>(pActors.second->Get_Component(TEXT("Com_StatusCombat")));
		if (pStatus == nullptr)
			assert(!" Error");
		m_ActorsStatusMap.emplace(pActors.second->Get_ObjectName(), pStatus);
	}
	return S_OK;
}

#ifdef _DEBUG
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

	if (m_iMonster_Player_Option == 0 && 
		m_pTurnStateButtonCanvas->Get_ButtonState()== BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Normal_Attack);
		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	else if (m_iMonster_Player_Option == 1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Normal_Attack);
	}
}

void CCombatController::To_Skill1_Attack()
{
	if (nullptr == m_pHitActor	 && m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		return;

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill1_Attack);
		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	else if (m_iMonster_Player_Option == 1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill1_Attack);
	}
}

void CCombatController::To_Skill2_Attack()
{
	if (nullptr == m_pHitActor	 && m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		return;

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill2_Attack);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	else if (m_iMonster_Player_Option == 1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill2_Attack);
	}
}

void CCombatController::To_Uitimate()
{
	if (nullptr == m_pHitActor	 && m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		return;

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Uitimate);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	else if (m_iMonster_Player_Option == 1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Uitimate);
	}
}

void CCombatController::To_Buff()
{
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Buff);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	else if (m_iMonster_Player_Option == 1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Buff);
	}
}

void CCombatController::To_WideAreaBuff()
{
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_WideAreaBuff);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	else if (m_iMonster_Player_Option == 1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_WideAreaBuff);
	}

}
	
void CCombatController::To_Use_Item()
{
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Use_Item);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	else if (m_iMonster_Player_Option == 1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Use_Item);
	}
	
}

void CCombatController::To_Defence()
{
	/*��� ���� ���Ŵϱ�*/
	if (m_pTurnStateButtonCanvas == nullptr && m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Defence);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}
	else if (m_iMonster_Player_Option == 1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Defence);
	}

}

void CCombatController::To_Light_Hit()
{
	/* �´°Ŵ� ���� �Ǵ� �ֵ��� �����ؾ��ϴµ� �̰��ҷ��� ��ŷ�� �ʿ���*/
	if (m_bIsHiterhit)
	{
		m_pHitActor->Set_FsmState(true, CGameObject::m_Light_Hit);
		m_bIsHiterhit = false;
	
	}
}

void CCombatController::To_Heavy_Hit()
{
	/* �´°Ŵ� ���� �Ǵ� �ֵ��� �����ؾ��ϴµ� �̰��ҷ��� ��ŷ�� �ʿ���*/
	if (m_pGameInstace->Key_Down(DIK_I))
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Heavy_Hit);
		m_bIsHiterhit = false;
		m_bisHitTimer_Alive = false;
		m_fHitTimer = 0.0f;
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

CStatus * CCombatController::Find_CurStatus(const wstring & pNameTag)
{
	auto Pair = find_if(m_ActorsStatusMap.begin(), m_ActorsStatusMap.end(), [&](auto MyPair)->bool
	{
		if (MyPair.first == pNameTag)
			return true;
		return	false;
	});

	if (Pair == m_ActorsStatusMap.end())
		assert(! "CCombatController_Find_CurActor_issue");
	
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