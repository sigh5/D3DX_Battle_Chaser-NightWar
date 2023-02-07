#include "stdafx.h"
#include "..\public\CombatController.h"

#include "Client_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "Canvas.h"

#include "GameObject.h"
#include "CombatActors.h"
#include "Weapon.h"

#include "Player.h"
#include "Monster.h"

#include "TurnStateCanvas.h"
#include "TurnUICanvas.h"
#include "HpMpBuffCanvas.h"
#include "TrunWinCanvas.h"
#include "Camera_Combat.h"
#include "Damage_Font_Manager.h"
#include "Explain_FontMgr.h"
#include "CombatMap.h"
#include "Mouse.h"


#include "MyImage.h"
#include "MeshGround.h"



IMPLEMENT_SINGLETON(CCombatController);

CCombatController::CCombatController()
	:m_pGameInstace{ CGameInstance::GetInstance() }
	, m_pFontManager(CDamage_Font_Manager::GetInstance())
	, m_pExplainFontManager(CExplain_FontMgr::GetInstance())
{
	
}

CStatus * CCombatController::Get_CurActorStatus() 
{
	if (m_pCurentActor == nullptr)
		return nullptr;

	return Find_CurStatus(m_pCurentActor->Get_ObjectName());
}

void CCombatController::Setting_Win_Canvas(CCanvas * pCanvas)
{
	assert(nullptr != dynamic_cast<CTrunWinCanvas*>(pCanvas)
		&& "CCombatController::Setting_Win_Canvas");


	for (auto& pCurActor : m_CurActorMap)
	{
		static_cast<CTrunWinCanvas*>(pCanvas)->Exp_Set_Status(m_ActorsStatusMap);
	}


}

void CCombatController::Set_MouseTextureOption(_int iMouseTextureNum)
{
	if (m_pMouse != nullptr)
	{
		m_pMouse->Set_MouseTextureNUm(iMouseTextureNum);
	}
}

CGameObject* CCombatController::Get_Player(wstring pName)
{
	return	Find_CurActor(pName);
}

void CCombatController::Scene_Chane_Safe_Release()
{
	for (auto& pStatus : m_ActorsStatusMap)
		Safe_Release(pStatus.second);
	m_ActorsStatusMap.clear();

	for (auto& pActor : m_CurActorMap)
		Safe_Release(pActor.second);
	m_CurActorMap.clear();

	for (auto& pCanvas : m_CanvasVec)
	{
		//pCanvas->Delete_Delegate();
		Safe_Release(pCanvas);
	}
	m_CanvasVec.clear();

	
	m_bVirtory = false;
	m_pCombatCamera = nullptr;
	m_pTurnCanvas = nullptr;
	m_pCurentActor = nullptr;
	m_pHitActor = nullptr;
	m_pTurnCanvas = nullptr;
	m_pTurnStateButtonCanvas = nullptr;
	
	m_bCombatIntro = false;
	m_bLateInit = false;
	m_dIntroTimer = 0.0;

	m_bIsHiterhit = false;
	m_bisHitTimer_Alive = false;

	m_fHitTimer = 0.0f;
	m_fHitRecoverTime = 0.2f;

	m_bMonsterSelect_Target = false;
	m_iMonster_Player_Option = 0;

	m_bIsPlayer = false;
	m_bMonsterTurnEnd = false;

	m_iHitNum = 0;
}

void CCombatController::Reset_Timer()
{
	m_dIntroTimer = 0.0;
	m_bVirtory = false;
}

HRESULT CCombatController::Initialize(_uint iLevel)
{
#ifdef LEVEL_BOSSONLY

#else
	wstring ObjTag = TEXT("");
	for (auto& Pair : m_pGameInstace->Get_Layer()[iLevel])
	{
		for (auto& obj : Pair.second->GetGameObjects())
		{
			if ( dynamic_cast<CCombatActors*>(obj))
			{	
				ObjTag = obj->Get_ObjectName();
				m_CurActorMap.emplace(ObjTag, obj);
				Safe_AddRef(obj);

				static_cast<CCombatActors*>(obj)->Initialize_CombatSound();
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

	m_pCombatCamera = static_cast<CCamera_Combat*>(
		m_pGameInstace->Get_GameObject(LEVEL_COMBAT,
			TEXT("Layer_Camera"), TEXT("CombatLevel_Camera")));

	m_pFontManager->Initialize();
	m_pExplainFontManager->Initialize();

	m_pMouse = static_cast<CMouse*>(CGameInstance::GetInstance()->Get_GameObject(LEVEL_COMBAT,LAYER_PLAYER, TEXT("MouseCuSor")));
#endif // LEVEL_BOSSONLY

	return S_OK;
}

HRESULT CCombatController::Late_Init()
{
	if (m_bLateInit)
		return S_OK;
#ifdef LEVEL_BOSSONLY

#else
	Set_CurrentActor();

	if (FAILED(Set_ActorsStatus()))
		assert(!"Set_ActorsStatus Error");

	for (auto& pCanvas : m_CanvasVec)
	{
		if (dynamic_cast<CHpMpBuffCanvas*>(pCanvas) != nullptr)
		{
			static_cast<CHpMpBuffCanvas*>(pCanvas)->Set_StatusHpMpBar(m_ActorsStatusMap);
		}
	}

	m_bLateInit = true;
#endif // LEVEL_BOSSONLY
	return S_OK;
}

void CCombatController::CurrentTurn_ActorControl(_double TimeDelta)
{

#ifdef LEVEL_BOSSONLY

#else
	m_pFontManager->Tick(TimeDelta);
	m_pExplainFontManager->Tick(TimeDelta);

	if (m_bVirtory)
	{
		Render_StopCanvas();
		return;
	}
	m_pFontManager->Tick(TimeDelta);
	m_pExplainFontManager->Tick(TimeDelta);

	m_dIntroTimer += TimeDelta *1.f;

	if (m_dIntroTimer >= 3.f)
	{
		Late_Init();
		m_bCombatIntro = false;
		PlayerWin();
	}
	else
	{
		To_Intro();
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
	}

	// 몬스터 Fsm 완성전까지 주석 /*필요한 코드 삭제x*/
	if (nullptr != dynamic_cast<CMonster*>(m_pCurentActor))
		m_pTurnStateButtonCanvas->Set_RenderActive(false);


	MouseChangeOn_Target();

	if (m_pGameInstace->Mouse_Up(CInput_Device::DIM_LB))
	{
		PickingTarget();
	}

	MonsterSetTarget(TimeDelta);
	Active_Fsm();
	Collison_Event();
#endif

}

void CCombatController::Status_CanvasInit()
{
	for (auto& pCanvas : m_CanvasVec)
	{
		if (dynamic_cast<CHpMpBuffCanvas*>(pCanvas) == nullptr)
			pCanvas->Set_RenderActive(true);
		else if(dynamic_cast<CTurnUICanvas*>(pCanvas) == nullptr)
			pCanvas->Set_RenderActive(true);
		else
			continue;

	}
}

void CCombatController::PlayerWin()
{
	_float fHPRatio = 0.f;
	for (auto& pMonster : m_CurActorMap)
	{
		if (nullptr == dynamic_cast<CMonster*>(pMonster.second))
			continue;

		_float fMonsterHpRatio = Find_CurStatus(pMonster.first)->Get_CurStatusHpRatio();

		fHPRatio += fMonsterHpRatio;
	}
	
	if (fHPRatio <= 0.f && !m_bVirtory)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		CClient_Manager::m_bCombatWin = true;
		m_bVirtory = true;

		for (auto& pPlayer : m_CurActorMap)
		{
			pPlayer.second->Set_FsmState(true, CGameObject::m_Viroty);
		}
		RELEASE_INSTANCE(CGameInstance);
	}
}

void CCombatController::Late_Tick(_double TimeDelta)
{
#ifdef LEVEL_BOSSONLY

#else
	m_pFontManager->Late_Tick(TimeDelta);
	m_pExplainFontManager->Late_Tick(TimeDelta);
	Ultimate_LateTick(TimeDelta);
#endif
}

void CCombatController::Cal_HitPlayerTarget()
{
	if (m_bMonsterSelect_Target)
		return;

	m_iMonster_Player_Option = 1; 
	m_bMonsterTurnEnd = false;
	/*
		공격 우선순위 1) 나를 때린 놈 2) 피가 제일 작은 놈 ,3) 마나가 적은놈 
					수정필요
	*/

	_float MinPlayerHPRatio = 999.f;
	_float MinPlayerMPRatio = 999.f;
	//CGameObject* pHitTargetMp = nullptr;
	//m_iMonster_Player_Option = 2;

	CGameObject* pMeHit_Player = static_cast<CMonster*>(m_pCurentActor)->Get_Me_hitPlayer();
	iMonsterActiveNum = static_cast<CMonster*>(m_pCurentActor)->Get_MonsterActiveNum();
	if (pMeHit_Player != nullptr)
	{
		m_pHitActor = pMeHit_Player;
		static_cast<CCombatActors*>(m_pCurentActor)->Set_HitTarget(pMeHit_Player);
	
		CStatus* pStatus = Find_CurStatus(m_pHitActor->Get_ObjectName());
		_float CurPlayer_HPRatio = pStatus->Get_CurStatusHpRatio();
		if (CurPlayer_HPRatio <= 0.f)
		{
			pMeHit_Player = nullptr;
		}
	}
	
	if(pMeHit_Player == nullptr)
	{
		for (auto& pPlayer : m_CurActorMap)
		{
			if (dynamic_cast<CPlayer*>(pPlayer.second) != nullptr)
			{
				_float CurPlayerHPRatio = Find_CurStatus(pPlayer.second->Get_ObjectName())->Get_CurStatusHpRatio();
				_float CurPlayerMPRatio = Find_CurStatus(pPlayer.second->Get_ObjectName())->Get_CurStatusMpRatio();

				if (CurPlayerHPRatio <= MinPlayerHPRatio && CurPlayerHPRatio > 0.f)
				{
					MinPlayerHPRatio = CurPlayerHPRatio;
					m_pHitActor = pPlayer.second;
				}

				if (CurPlayerMPRatio <= MinPlayerMPRatio  && CurPlayerMPRatio > 0.f)
				{
					MinPlayerMPRatio = CurPlayerMPRatio;
					//pHitTargetMp = pPlayer.second;
				}
			}
		}
		static_cast<CCombatActors*>(m_pCurentActor)->Set_HitTarget(m_pHitActor);
	}

	/*
		행동 우선순위 1) 본인 피가 절반이하일때 방어, 또는 버프 ,절반 이상일때 공격
		2) 스킬 마나가 부족한 경우 (피가 절반이상) 기본_공격, (피가 절반) 이하 방어
		3) 본인피가 20% 이하이고 마나가 절반 이상일 경우 궁극기 활용 마나가 절반 이하 스킬사용
		4) 같은 스킬은 연속해서 쓰지 않는다.
	*/

	fCurActorHpRatio = Find_CurStatus(m_pCurentActor->Get_ObjectName())->Get_CurStatusHpRatio();
	fCurActorMpRatio = Find_CurStatus(m_pCurentActor->Get_ObjectName())->Get_CurStatusMpRatio();

	int iMonsterSettingNum = m_pGameInstace->Get_Setting_MonsterScene();

	if (iMonsterSettingNum != 4)
		Cal_MapOneMonsterFsm();

	else
		Cal_MapTwoMonsterFsm();


	Mana_Refresh();
	m_bMonsterSelect_Target = true;

}

void CCombatController::Set_Player_StateCanvas()
{
	if (m_bIsPlayer == true)
	{
		m_pTurnStateButtonCanvas->Set_RenderActive(true);
	}
}

void CCombatController::Ultimate_Camera_On()
{
	m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_END);

	for (auto& pCanvas : m_CanvasVec)
	{
		if (dynamic_cast<CCombatMap*>(pCanvas) != nullptr)
		{
			continue;
		}
		else
			pCanvas->Set_RenderActive(false);
	}

	m_bCurActorUltimateUse = true;

	if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Hero_Gully")))
	{
		m_pCurBannerImage = static_cast<CMyImage*>(m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_Knolan"), LEVEL_COMBAT));
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Hero_Alumon")))
	{
		m_pCurBannerImage = static_cast<CMyImage*>(m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_Garrison"), LEVEL_COMBAT));
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Hero_Calibretto")))
	{
		m_pCurBannerImage = static_cast<CMyImage*>(m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_Caibretto"), LEVEL_COMBAT));
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Monster_SlimeKing")))
	{
		m_pCurBannerImage = static_cast<CMyImage*>(m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_SlimeKing"), LEVEL_COMBAT));
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Boss_Alumon")))
	{
		m_pCurBannerImage = static_cast<CMyImage*>(m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_SlimeKing"), LEVEL_COMBAT));
	}
	else
		return;

}

void CCombatController::Ultimate_Camera_Off()
{
	for (auto &pActor : m_CurActorMap)
		static_cast<CCombatActors*>(pActor.second)->Set_ModelRender(true);

	for (auto& pCanvas : m_CanvasVec)
	{
		if(dynamic_cast<CTurnStateCanvas*>(pCanvas) == nullptr)
			pCanvas->Set_RenderActive(true);
	}
	m_pCombatCamera->Ultimate_EndCameraWork();
}

void CCombatController::Ultimate_LateTick(_double TimeDelta)
{
	if (m_bVirtory)
		return;
	
	Ultimate_Start_LateTick(TimeDelta);
	Ultimate_Timedelta_Tick(TimeDelta);
	Ultimate_End_LateTick(TimeDelta);
	
}
	

void CCombatController::Ready_Ultimate()
{
	Get_CurActor()->Set_FsmState(true, CGameObject::m_Uitimate);

	CGameObject* pMeshGround = m_pGameInstace->Get_GameObject(LEVEL_COMBAT, LAYER_UI, TEXT("Commbat_MeshGround"));
	assert(nullptr != pMeshGround && "CCombatController::Ready_Ultimate()");
	static_cast<CMeshGround*>(pMeshGround)->Set_MeshGroundRender_Active(false);



}

void CCombatController::Ultimate_Start_LateTick(_double TimeDelta)
{
	if (false == m_bCurActorUltimateUse)
		return;

	if (true == m_pCurBannerImage->Get_BannerTimerFinsish())
	{
		CombatMap_RenderStop();
		m_pCombatCamera->UltimateStart_CameraWork(m_pCurentActor);
		m_pGameInstace->DeleteGameObject(LEVEL_COMBAT, m_pCurBannerImage->Get_ObjectName());
		m_bCurActorUltimateUse = false;
		m_pCurBannerImage = nullptr;
		m_bBannerClose = true;
		m_pCombatBG = m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_BG"), LEVEL_COMBAT);
		
		for (auto &pActor : m_CurActorMap)
		{
			static_cast<CCombatActors*>(pActor.second)->Set_ModelRender(false);
		}
		static_cast<CCombatActors*>(m_pCurentActor)->Set_ModelRender(true);
	}

}

void CCombatController::Ultimate_Timedelta_Tick(_double TimeDelta)
{
	if (false == m_bBannerClose)
		return;
	
	m_fTimeSlowRatio -= _float(TimeDelta);
	m_fTimeSlowRatio = fmax(m_fTimeSlowRatio, 0.3f);

	if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Hero_Calibretto")))
	{
		m_fTimeSlowRatio = fmax(m_fTimeSlowRatio, 1.0f);
		return;
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Hero_Alumon")))
	{
		m_fTimeSlowRatio = fmax(m_fTimeSlowRatio, 1.f);
		return;
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Monster_SlimeKing")))
	{
		m_fTimeSlowRatio = fmax(m_fTimeSlowRatio, 0.4f);
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Boss_Alumon")))
	{
		m_fTimeSlowRatio = fmax(m_fTimeSlowRatio, 0.4f);
	}
	
	m_pGameInstace->Set_Timedelta(TEXT("Timer_60"), m_fTimeSlowRatio);	
	
}

void CCombatController::Ultimate_End_LateTick(_double TimeDelta)
{
	if (false == m_bCurActorUltimateEnd)
		return;
	
	m_bBannerClose = false;
	m_bCurActorUltimateEnd = false;
	m_pGameInstace->DeleteGameObject(LEVEL_COMBAT, m_pCombatBG->Get_ObjectName());
	m_pGameInstace->DeleteGameObject(LEVEL_COMBAT, m_pCombatBG2->Get_ObjectName());

	CGameObject* pMeshGround = m_pGameInstace->Get_GameObject(LEVEL_COMBAT, LAYER_UI, TEXT("Commbat_MeshGround"));
	assert(nullptr != pMeshGround && "CCombatController::Ready_Ultimate()");
	static_cast<CMeshGround*>(pMeshGround)->Set_MeshGroundRender_Active(true);
	CombatMap_RenderStart();
	Ultimate_Camera_Off();
	m_pCombatBG = nullptr;
	m_pCombatBG2 = nullptr;
}

void CCombatController::CombatMap_RenderStop()
{
	for (auto& pCanvas : m_CanvasVec)
	{
		if (dynamic_cast<CCombatMap*>(pCanvas) != nullptr)
			pCanvas->Set_RenderActive(false);
	}
}

void CCombatController::CombatMap_RenderStart()
{
	for (auto& pCanvas : m_CanvasVec)
	{
		if (dynamic_cast<CCombatMap*>(pCanvas) != nullptr)
			pCanvas->Set_RenderActive(true);
	}
}

void CCombatController::Refresh_CurActor()
{
	Set_CurrentActor();
	m_pHitActor = nullptr;
	m_bIsHiterhit = false;
	m_bisHitTimer_Alive = false;
	m_fHitTimer = 0.0f;
	m_iHitNum = 0;
	
	
	Set_Player_StateCanvas();
}

void CCombatController::Mana_Refresh()
{
	for (auto &Canvas : m_CanvasVec)
	{
		if (dynamic_cast<CHpMpBuffCanvas*>(Canvas) != nullptr)
		{
			static_cast<CHpMpBuffCanvas*>(Canvas)->Set_MpEvent(true);
		}
	}
}

void CCombatController::Active_Fsm()
{
	if (ImGui::Button("Level 2"))
	{
		CStatus* pStatus = Find_CurStatus(m_pCurentActor->Get_ObjectName());

		pStatus->Set_Exp(120);
	}

	To_Idle();
	To_Intro();
	To_Normal_Attack();
	To_Skill1_Attack();
	To_Skill2_Attack();
	To_Uitimate();
	
	To_Buff();
	To_WideAreaBuff();
	To_Use_Item();
	To_Defence();
	To_Flee();
	//To_Viroty();

	To_Light_Hit();
	To_Heavy_Hit();

	//To_Die();
	
}

void CCombatController::Load_CamBG2()
{
	if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Hero_Calibretto")))
	{
		m_pCombatBG2 = m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_BG_3"), LEVEL_COMBAT);
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Hero_Alumon")))
	{
		m_pCombatBG2 = m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_BG_1"), LEVEL_COMBAT);
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Hero_Gully")))
	{
		m_pCombatBG2 = m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_BG_2"), LEVEL_COMBAT);
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Monster_SlimeKing")))
	{
		m_pCombatBG2 = m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_BG_2"), LEVEL_COMBAT);
	}
	else if (!lstrcmp(m_pCurentActor->Get_ObjectName(), TEXT("Boss_Alumon")))
	{
		m_pCombatBG2 = m_pGameInstace->Load_Object(TEXT("Texture_Ultimate_Banner_BG_2"), LEVEL_COMBAT);
	}
	else
		return;
}

void CCombatController::Wide_Debuff(_bool bIsPlayer, CStatus::DEBUFFTYPE eDebuffOption)
{
	if (bIsPlayer == true)
	{
		
		for (auto& pPlayer : m_CurActorMap)
		{
			if (dynamic_cast<CPlayer*>(pPlayer.second) == nullptr)
				continue;

			static_cast<CCombatActors*>(pPlayer.second)->Create_Wide_Debuff(eDebuffOption);
		}

	}
	else
	{
		/*	for (auto& pMonster : m_CurActorMap)
			{
				if (dynamic_cast<CMonster*>(pMonster.second) == nullptr)
					continue;

				pMonster.second->Set_FsmState(true, CGameObject::m_Heavy_Hit);
				static_cast<CCombatActors*>(pMonster.second)->Set_WideAttackDamage(iDamage);
			}*/
	}


}

void CCombatController::HPMp_Update(CGameObject* pHiter)
{
	for (auto &Canvas : m_CanvasVec)
	{
		if (dynamic_cast<CHpMpBuffCanvas*>(Canvas) != nullptr)
		{
			static_cast<CHpMpBuffCanvas*>(Canvas)->Set_HitEvent(pHiter, true);
			static_cast<CHpMpBuffCanvas*>(Canvas)->Set_MpEvent(true);
			
		}
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
	
	m_bVirtory = false;
}

void CCombatController::Render_StopCanvas()
{
	for (auto& pCanvas : m_CanvasVec)
	{
		if(dynamic_cast<CCombatMap*>(pCanvas) == nullptr)
			pCanvas->Set_RenderActive(false);
	}
}

void CCombatController::Camera_Shaking()
{
	m_pCombatCamera->Ready_CameraShaking(0.5f, 0.1f);
	m_pCombatCamera->Set_CameraShaking_Active(true);
}

void CCombatController::Camera_ShortShaking()
{
	m_pCombatCamera->Ready_CameraShaking(0.25f, 0.1f,true);
	m_pCombatCamera->Set_CameraShaking_Active(true);
}

void CCombatController::Camera_Zoom_Out()
{
	m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_recover);
}

void CCombatController::Camera_Zoom_In()
{
	m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
}

void CCombatController::UI_Shaking(_bool bShaking)
{
	for (auto& pCurCanvas : m_CanvasVec)
		pCurCanvas->Set_Child_UI_Shaking(bShaking);

}

void CCombatController::Wide_Attack(_bool IsPlayer, _int iDamage)
{
	if (true == IsPlayer)
	{
		for (auto& pPlayer : m_CurActorMap)
		{
			if (dynamic_cast<CPlayer*>(pPlayer.second) == nullptr)
				continue;

			pPlayer.second->Set_FsmState(true, CGameObject::m_Heavy_Hit);
			static_cast<CCombatActors*>(pPlayer.second)->Set_WideAttackDamage(iDamage);
		}
		Camera_Shaking();
	}
	else
	{
		for (auto& pMonster : m_CurActorMap)
		{
			if(dynamic_cast<CMonster*>(pMonster.second) == nullptr)
				continue;
			pMonster.second->Set_FsmState(true, CGameObject::m_Heavy_Hit);
			static_cast<CCombatActors*>(pMonster.second)->Set_WideAttackDamage(iDamage);
		}
		Camera_Shaking();
	}
}

void CCombatController::PickingTarget()		// 피킹은 플레이어만 가능하다.
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
			m_iMonster_Player_Option = 0;
			static_cast<CPlayer*>(m_pCurentActor)->Set_HitTarget(m_pHitActor);
			static_cast<CMonster*>(m_pHitActor)->Set_Me_HitPlayer(m_pCurentActor);
			break;
		}
	}
}

void CCombatController::MouseChangeOn_Target()
{
	_bool bIsMouse_OnTarget = false;
	for (auto& pMonster : m_CurActorMap)
	{
		if (nullptr == dynamic_cast<CMonster*>(pMonster.second))
			continue;

		if (static_cast<CMonster*>(pMonster.second)->IsCollMouse())
		{
			m_pMouse->Set_MouseTextureNUm(1);
			bIsMouse_OnTarget = true;
		}
	}

	if (bIsMouse_OnTarget == false)
		m_pMouse->Set_MouseTextureNUm(4);
}

void CCombatController::MonsterSetTarget(_double TimeDelta)
{
	if (nullptr == dynamic_cast<CMonster*>(m_pCurentActor) )
		return;


	CStatus* pStatus = Find_CurStatus(m_pCurentActor->Get_ObjectName());
	if (pStatus->Get_CurStatusHpRatio() <= 0.f)
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

	if (pWeapon == nullptr)
		return;

	if (static_cast<CHitBoxObject*>(pWeapon)->Get_HitNum() > m_iHitNum && static_cast<CCombatActors*>(m_pHitActor)->Calculator_HitColl(pWeapon))
	{
		m_bIsHiterhit = true;
		m_bisHitTimer_Alive = true;
		++m_iHitNum;
	}
}

HRESULT CCombatController::Set_CurrentActor()
{
	if (m_bVirtory)
		return S_OK;

	assert(m_pTurnCanvas != nullptr && "CCombatController::Set_CurrentActor issue");

	UI_REPRESENT eCurActor = m_pTurnCanvas->Get_CurrentActor();
	
	switch (eCurActor)
	{
	case Client::REPRESENT_KNOLAN:
		m_pCurentActor = Find_CurActor(TEXT("Hero_Gully"));
		m_bIsPlayer = true;
		m_pTurnStateButtonCanvas->CurState_Fsm_ButtonICon(TEXT("Hero_Gully"));
		m_iMonster_Player_Option = 0;
		static_cast<CCombatActors*>(m_pCurentActor)->Set_UseDefence(false);
		break;
	case Client::REPRESENT_GARRISON:
		m_pCurentActor = Find_CurActor(TEXT("Hero_Alumon"));
		m_bIsPlayer = true;
		m_pTurnStateButtonCanvas->CurState_Fsm_ButtonICon(TEXT("Hero_Alumon"));
		m_iMonster_Player_Option = 0;
		static_cast<CCombatActors*>(m_pCurentActor)->Set_UseDefence(false);
		break;
	case Client::REPRESENT_CALIBRETTO:
		m_pCurentActor = Find_CurActor(TEXT("Hero_Calibretto"));
		m_pTurnStateButtonCanvas->CurState_Fsm_ButtonICon(TEXT("Hero_Calibretto"));
		m_iMonster_Player_Option = 0;
		m_bIsPlayer = true;
		break;
	case Client::REPRESENT_SKELTON_NAKED:
		m_pCurentActor = Find_CurActor(TEXT("Skeleton_Naked"));
		m_iMonster_Player_Option = 1;
		m_bIsPlayer = false;
		break;
	case Client::REPRESENT_SLIMEKING:
		m_pCurentActor = Find_CurActor(TEXT("Monster_SlimeKing"));
		m_iMonster_Player_Option = 1;
		m_bIsPlayer = false;
		break;
	case Client::REPRESENT_SPIDER_MANA:
		m_pCurentActor = Find_CurActor(TEXT("Spider_Mana"));
		m_iMonster_Player_Option = 1;
		m_bIsPlayer = false;
		break;
	case Client::REPRESENT_REAL_BOSS:
		m_pCurentActor = Find_CurActor(TEXT("Boss_Alumon"));
		m_iMonster_Player_Option = 1;
		m_bIsPlayer = false;
		break;
		

	case Client::REPRESENT_END:
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
		Safe_AddRef(pStatus);
		m_ActorsStatusMap.emplace(pActors.second->Get_ObjectName(), pStatus);

	
	}

	return S_OK;
}

void CCombatController::Cal_MapOneMonsterFsm()
{
	if (fCurActorMpRatio <= 0.f)
	{
		To_Normal_Attack();
	}
	else if (fCurActorHpRatio >= 0.5f) // 1) 본인 피가 절반인가? 
	{
		// 맞은 적이없고 버프를 쓴적이없다.
		if (static_cast<CMonster*>(m_pCurentActor)->Get_HitNum() == 0
			&& true == static_cast<CMonster*>(m_pCurentActor)->IsHaveBuff()
			&& false == static_cast<CMonster*>(m_pCurentActor)->Get_UseBuff())
		{

			To_Buff();
		}
		else
		{
			if (0 == iMonsterActiveNum)
			{
				To_Normal_Attack();
				++iMonsterActiveNum;
			}
			else if (1 == iMonsterActiveNum)
			{
				To_Skill1_Attack();
				++iMonsterActiveNum;
			}
			else if (2 == iMonsterActiveNum)
			{
				if (static_cast<CMonster*>(m_pCurentActor)->IsHaveSkill2())
				{
					To_Skill2_Attack();
				}
				else
					To_Skill1_Attack();

				iMonsterActiveNum = 0;
			}

		}

	}
	else   // 본인 피가 절반 이하다.
	{
		//본인 피가 절반 20%이하고 마나가 50퍼 이상 높을때 궁극기 사용
		if (fCurActorHpRatio <= 0.45f)		// 나중에 0.3f 로 바꾸기
		{
			if (fCurActorMpRatio >= 0.5f)
			{
				if (static_cast<CMonster*>(m_pCurentActor)->IsHaveUlitmate() && !m_bMonsterUseUltimate)
				{
					m_bMonsterUseUltimate = true;
					iMonsterActiveNum = 0;
					To_Uitimate();
				}
				else if (static_cast<CMonster*>(m_pCurentActor)->IsHaveSkill2() && iMonsterActiveNum >= 1)
				{
					To_Skill2_Attack();
					iMonsterActiveNum = 0;
				}
				else
				{
					To_Skill1_Attack();
					++iMonsterActiveNum;
					if (static_cast<CMonster*>(m_pCurentActor)->IsHaveUlitmate())
					{
						m_bMonsterUseUltimate = false;
					}
				}
			}
			else //마나가 50퍼 공격 스킬사용
			{
				if (static_cast<CMonster*>(m_pCurentActor)->IsHaveSkill2() && iMonsterActiveNum >= 1)
				{
					To_Skill2_Attack();
					iMonsterActiveNum = 0;
				}
				else
				{
					To_Skill1_Attack();
					++iMonsterActiveNum;
				}
			}
		}
		else
		{
			if (static_cast<CMonster*>(m_pCurentActor)->IsHaveSkill2() && fCurActorMpRatio >= 0.25f && iMonsterActiveNum == 2)
			{
				if (static_cast<CMonster*>(m_pCurentActor)->IsHaveSkill2())
					To_Skill2_Attack();
				else if (false == static_cast<CMonster*>(m_pCurentActor)->IsHaveSkill2()
					&& true == static_cast<CMonster*>(m_pCurentActor)->IsHaveUlitmate())
					To_Uitimate();
				else
					To_Skill1_Attack();

				++iMonsterActiveNum;
			}
			else if (fCurActorMpRatio >= 0.1f &&  iMonsterActiveNum == 1)	//공격스킬사용
			{
				To_Skill1_Attack();
				++iMonsterActiveNum;
			}
			else						//기본공격 사용
			{
				To_Normal_Attack();
				iMonsterActiveNum = 0;
			}
		}
	}

	static_cast<CMonster*>(m_pCurentActor)->Set_MonsterActiveNum(iMonsterActiveNum);
}

void CCombatController::Cal_MapTwoMonsterFsm()
{
	To_Use_NormalAttack2();		// 보스는 노멀어택 2임
	++m_iBossActiveOption;
	return;

	if (fCurActorMpRatio <= 0.f)
	{
		if (m_iBossActiveOption == 0)
		{
			To_Normal_Attack();
			++m_iBossActiveOption;
		}
		if (m_iBossActiveOption == 1)
		{
			To_Use_Item();
			m_iBossActiveOption = 0;
		}
	}
	else if (fCurActorHpRatio >= 0.5f) // 1) 본인 피가 절반인가? 
	{
		// 맞은 적이없고 버프를 쓴적이없다.
		if (static_cast<CMonster*>(m_pCurentActor)->Get_HitNum() == 0)
		{
			To_Buff();
			m_iBossActiveOption = 0;
		}
		else
		{
			if (0 == m_iBossActiveOption)
			{
				To_Normal_Attack();
				++m_iBossActiveOption;
			}
			else if (1 == m_iBossActiveOption)
			{
				To_Skill1_Attack();
				++m_iBossActiveOption;
			}
			else if (2 == m_iBossActiveOption)
			{
				To_Use_Item();		// 보스는 노멀어택 2임
				++m_iBossActiveOption;
			}
			else if (3 == m_iBossActiveOption)
			{
				To_Skill2_Attack();
				m_iBossActiveOption = 0;
			}
		}
	}
	else   // 본인 피가 절반 이하다.
	{
		//본인 피가 절반 20%이하고 마나가 50퍼 이상 높을때 궁극기 사용
		if (fCurActorHpRatio <= 0.45f)		// 나중에 0.3f 로 바꾸기
		{
			if (fCurActorMpRatio >= 0.5f)
			{
				if (!m_bMonsterUseUltimate )
				{
					m_bMonsterUseUltimate = true;
					m_iBossActiveOption = 1;	
					To_Uitimate();
				}
				else if (m_iBossActiveOption==1)
				{
					To_Skill2_Attack();
					++m_iBossActiveOption;
				}
				else if (m_iBossActiveOption == 2)
				{
					To_Skill1_Attack();
					m_bMonsterUseUltimate = false;
					m_iBossActiveOption = 0;
				}
			}
			else //마나가 50퍼이하 공격 스킬사용
			{
				if (false == m_bManaHalfAndHpHalf)
				{
					To_Skill2_Attack();
					m_iBossActiveOption = 0;
					m_bManaHalfAndHpHalf = true;
				}
				else if (m_iBossActiveOption ==0)
				{
					To_Use_Item();
					iMonsterActiveNum = 0;
				
				}
				else if (m_iBossActiveOption == 1)
				{
					To_Buff();
					++iMonsterActiveNum;
				}
				else if (m_iBossActiveOption == 2)
				{
					To_Normal_Attack();
					++iMonsterActiveNum;
				}
				else if (m_iBossActiveOption == 3)
				{
					To_Skill1_Attack();
					m_bManaHalfAndHpHalf = false;
				}
			}
		}
		else
		{
			if (false==m_bBossUltimateTwo)
			{
				To_WideAreaBuff();		// 보스는 이게 궁극기 2임
				
				if (m_iBossActiveOption >= 1)
					m_iBossActiveOption = 0;
				else
					++m_iBossActiveOption;

				m_bBossUltimateTwo = true;
			}
			else if (m_iBossActiveOption == 1)
			{
				To_Skill1_Attack();
				++m_iBossActiveOption;
			}
			else if (m_iBossActiveOption == 2)
			{
				To_Buff();
				++iMonsterActiveNum;
			}
			else if (m_iBossActiveOption == 3)
			{
				To_Uitimate();
				++iMonsterActiveNum;
			}
			else if (m_iBossActiveOption == 4)
			{
				To_Skill2_Attack();
				++iMonsterActiveNum;
			}
			else if (m_iBossActiveOption == 5)
			{
				To_Normal_Attack();
				m_bBossUltimateTwo = false;
			}
		}
	}
}



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
	if (nullptr == m_pHitActor	|| m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		return;

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_NORMALATTACK)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Normal_Attack);
		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		//m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
	}
	else if (m_iMonster_Player_Option == 1 && !m_bMonsterTurnEnd)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Normal_Attack);
		m_bMonsterTurnEnd = true;
		//m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
	}
	else
		return;
}

void CCombatController::To_Skill1_Attack()
{
	if (nullptr == m_pHitActor || m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		return;

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_SKILL1)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill1_Attack);
		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		//m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
		Mana_Refresh();
	}
	else if (m_iMonster_Player_Option == 1 && !m_bMonsterTurnEnd)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill1_Attack);
	//	m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
		m_bMonsterTurnEnd = true;
	}
	else
		return;
}

void CCombatController::To_Skill2_Attack()
{
	if (nullptr == m_pHitActor || m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		return;

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_SKILL2)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill2_Attack);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		//m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
		Mana_Refresh();
	}
	else if (m_iMonster_Player_Option == 1 && !m_bMonsterTurnEnd)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Skill2_Attack);
		m_bMonsterTurnEnd = true;
		//m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
	}
	else
		return;
}

void CCombatController::To_Uitimate()
{
	if (nullptr == m_pHitActor || m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		return;


	CStatus* pStatus = Find_CurStatus(m_pCurentActor->Get_ObjectName());
	assert(nullptr != pStatus && "CCombatController::To_Uitimate");

	if (m_iMonster_Player_Option == 0 
		&&  pStatus->Get_Level() >=2
		&&  m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&  m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_ULTIMATE)
	{
		Ultimate_Camera_On();
		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		Mana_Refresh();
	}
	else if (m_iMonster_Player_Option == 1 && !m_bMonsterTurnEnd)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Uitimate);
		Mana_Refresh();
		m_bMonsterTurnEnd = true;
	}
	else
		return;
}

void CCombatController::To_Buff()
{
	if (m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_BUFF)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Buff);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
		Mana_Refresh();
	}
	else if (m_iMonster_Player_Option == 1 && !m_bMonsterTurnEnd)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Buff);
		m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
		m_bMonsterTurnEnd = true;
	}
	else
		return;
}

void CCombatController::To_WideAreaBuff()
{
	if (m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ABLILTY//수정필요
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_WIDEBUFF)//수정필요
	{
		//if(static_cast<CCombatActors*>(m_pCurentActor)->)

		m_pCurentActor->Set_FsmState(true, CGameObject::m_WideAreaBuff);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
		Mana_Refresh();
	}
	else
		return;

}

void CCombatController::To_Use_NormalAttack2()
{
	if (dynamic_cast<CMonster*>(m_pCurentActor) == nullptr)
		return;
	if (m_iMonster_Player_Option == 1 && !m_bMonsterTurnEnd)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Use_Item);
		m_bMonsterTurnEnd = true;
	}
	else
		return;
}
	
void CCombatController::To_Use_Item()
{
	if (dynamic_cast<CPlayer*>(m_pCurentActor) == nullptr || m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		return;


	if (static_cast<CPlayer*>(m_pCurentActor)->Get_RestHpPotion() >=0
		&& m_iMonster_Player_Option == 0 
		&& m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ITEM
		&& m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_USE_HP_ITEM)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Use_Item);
		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		static_cast<CPlayer*>(m_pCurentActor)->Use_HpPotion();
		HPMp_Update(m_pCurentActor);
		//m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
	}
	else if (static_cast<CPlayer*>(m_pCurentActor)->Get_RestMpPotion() >= 0
		&& m_iMonster_Player_Option == 0 
		&& m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ITEM
		&& m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_USE_MP_ITEM)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Use_Item);

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		static_cast<CPlayer*>(m_pCurentActor)->Use_MpPotion();
		HPMp_Update(m_pCurentActor);
	}
	else if (m_iMonster_Player_Option == 1 && !m_bMonsterTurnEnd)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Use_Item);
		m_bMonsterTurnEnd = true;
	}
	else
		return;
	
}

void CCombatController::To_Defence()		
{
	/*어쩃든 턴을 쓴거니까*/
	if (m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_iMonster_Player_Option == 0 &&
		m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ACTION//수정필요
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_DEFENCE)//수정필요
	{
		//m_pCurentActor->Set_FsmState(true, CGameObject::m_Use_Item);

		static_cast<CCombatActors*>(m_pCurentActor)->Set_UseDefence(true);
		static_cast<CCombatActors*>(m_pCurentActor)->Create_Defence_Effect_And_Action();

		m_pTurnStateButtonCanvas->Set_ButtonState(BUTTON_STATE_END);
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
		m_bCanvasRenderCheck = true;
		m_pTurnStateButtonCanvas->Set_RenderActive(false);
		Mana_Refresh();
		m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
	}
	else if (m_iMonster_Player_Option == 1 && !m_bMonsterTurnEnd)
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::CHAR_STATE_END);
		m_bMonsterTurnEnd = true;
		m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_CurActor);
	}
	else
		return;

}

void CCombatController::To_Light_Hit()
{
	if (m_pHitActor == nullptr)
		return;
	/* 맞는거는 히팅 되는 애들을 추적해야하는데 이걸할려면 피킹이 필요함*/
	if (m_bIsHiterhit)
	{
		if (static_cast<CCombatActors*>(m_pHitActor)->Get_UseDeffece())
		{
			m_pHitActor->Set_FsmState(true, CGameObject::m_Defence);
		}
		else
		{
			m_pHitActor->Set_FsmState(true, CGameObject::m_Light_Hit);
		}
	
		for (auto &Canvas : m_CanvasVec)
		{
			if (dynamic_cast<CHpMpBuffCanvas*>(Canvas) != nullptr)
			{
				static_cast<CHpMpBuffCanvas*>(Canvas)->Set_HitEvent(m_pHitActor,true);
				static_cast<CHpMpBuffCanvas*>(Canvas)->Set_MpEvent(true);
			}
		}
		m_pCombatCamera->Set_ZoomMoveOption(CCamera_Combat::CameraTarget_Hiter);
		m_bIsHiterhit = false;
		m_bisHitTimer_Alive = false;
		m_fHitTimer = 0.0f;
	}
	else
		return;
}

void CCombatController::To_Heavy_Hit()
{
	if (m_pHitActor == nullptr)
		return;
	/* 맞는거는 히팅 되는 애들을 추적해야하는데 이걸할려면 피킹이 필요함*/
	if (m_pGameInstace->Key_Down(DIK_I))
	{
		m_pCurentActor->Set_FsmState(true, CGameObject::m_Heavy_Hit);
		m_bIsHiterhit = false;
		m_bisHitTimer_Alive = false;
		m_fHitTimer = 0.0f;
	}
	else
		return;
}

void CCombatController::To_Flee()
{
	if (m_pTurnStateButtonCanvas == nullptr || m_pTurnCanvas == nullptr)
		assert(!"CCombatController::To_Normal_Attack_Issue");

	if (m_pTurnStateButtonCanvas->Get_ButtonState() == BUTTON_STATE_ITEM
		&&m_pTurnStateButtonCanvas->Get_ButtonFsmState() == BUTTON_FSM_FLEE)
	{
		
		for (auto& pCurActor : m_CurActorMap)
		{
			if (nullptr != dynamic_cast<CPlayer*>(pCurActor.second))
			{
				pCurActor.second->Set_FsmState(true, CGameObject::m_Flee);
				Render_StopCanvas();
			}
			else
				pCurActor.second->Set_FsmState(true, CGameObject::m_Viroty);
		}
		//CGameInstance::GetInstance()->Play_Sound(TEXT(""), 1.f, false, SOUND_VOCIE);
		//m_pTurnCanvas->Move_Children();

		CClient_Manager::m_bCombatlose = true;
		m_pTurnStateButtonCanvas->Set_ButtonFsmState(BUTTON_FSM_STATE_END);
	}
	else
		return;
}

void CCombatController::To_Die()
{
	/*if (static_cast<CCombatActors*>(m_pHitActor)->Is_Dead())
		m_pHitActor->Set_FsmState(true, CGameObject::m_Die);*/
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
	{
		return nullptr;
		assert("CCombatController_Find_CurActor_issue");
	}
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
	{
		assert(!"CCombatController_Find_CurActor_issue");
		return nullptr;
	}
	return Pair->second;
}

void CCombatController::Free()
{
	for (auto& pStatus : m_ActorsStatusMap)
		Safe_Release(pStatus.second);
	m_ActorsStatusMap.clear();

	for (auto& pActor : m_CurActorMap)
		Safe_Release(pActor.second);
	m_CurActorMap.clear();

	for (auto& pCanvas : m_CanvasVec)
		Safe_Release(pCanvas);
	m_CanvasVec.clear();

	Safe_Release(m_pCurBannerImage);

	//Safe_Release(m_pGameInstace);

}