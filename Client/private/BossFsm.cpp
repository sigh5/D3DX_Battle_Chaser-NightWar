#include "stdafx.h"
#include "..\public\BossFsm.h"
#include "GameInstance.h"

#include "CombatController.h"
#include "Boss_Alumon.h"

CBossFsm::CBossFsm()
	: m_pCombatController(CCombatController::GetInstance())
{
}

HRESULT CBossFsm::Initialize(CGameObject * pTarget)
{
	m_pTarget = pTarget;

	if (FAILED(Init_Boss(pTarget)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossFsm::Init_Boss(CGameObject * pTarget)
{
	assert(pTarget != nullptr);
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))
		.AddState(TEXT("Idle"))
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Idle();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Intro"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]() {return m_pCombatController->To_Intro(); }))
		.Transition(TEXT("NormalAttack"), FSM_TRANSITION(TEXT("Idle To NormalAttack"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Normal_Attack(); }))
		.Transition(TEXT("SkillAttack1"), FSM_TRANSITION(TEXT("Idle To SkillAttack1"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Skill1_Attack(); }))
		.Transition(TEXT("SkillAttack2"), FSM_TRANSITION(TEXT("Idle To SkillAttack2"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Skill2_Attack(); }))
		.Transition(TEXT("Uitimate"), FSM_TRANSITION(TEXT("Idle To Uitimate"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Uitimate(); }))
		.Transition(TEXT("Buff"), FSM_TRANSITION(TEXT("Idle To Buff"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Buff(); }))
		.Transition(TEXT("WideBuff"), FSM_TRANSITION(TEXT("Idle To WideBuff"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_WideAreaBuff(); }))
		.Transition(TEXT("Defence"), FSM_TRANSITION(TEXT("Idle To Defence"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Defence(); }))
		.Transition(TEXT("UseItem"), FSM_TRANSITION(TEXT("Idle To UseItem"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Use_Item(); }))
		.Transition(TEXT("Light_Hit"), FSM_TRANSITION(TEXT("Idle To Light_Hit"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Light_Hit(); }))
		.Transition(TEXT("Heavy_Hit"), FSM_TRANSITION(TEXT("Idle To Heavy_Hit"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Heavy_Hit(); }))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Idle To Die"), [this]() {return static_cast<CBoss_Alumon*>(m_pTarget)->To_Die(); }))
		
		.AddState(L"Intro")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Intro();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"NormalAttack")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_NormalAttack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Fsm_Exit();
	})

		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"SkillAttack1")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Skill1_Attack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"SkillAttack2")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Skill2_Attack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"Uitimate")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Uitimate();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_UltimateTick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"Buff")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Buff();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}))
		.AddState(L"WideBuff")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Uitimate2();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Fsm_UltimateOne_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("WideBuff To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}))
		.AddState(L"Defence")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Defence();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Defence To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"UseItem")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_NormalAttack2();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("UseItem To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"Light_Hit")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Light_Hit();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Light_Hit To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}, 1))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Light_Hit To Die"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Is_Dead();
	}, 0))

		.AddState(L"Heavy_Hit")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Heavy_Hit();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Heavy_Hit To Idle"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Get_IsIdle();
	}, 1))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Heavy_Hit To Die"), [this]()
	{
		return  static_cast<CBoss_Alumon*>(m_pTarget)->Is_Dead();
	}, 0))
		.OnExit([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->UltiHeavyHitExit();
	})


	.AddState(L"Die")
		.OnStart([this]()
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Anim_Die();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CBoss_Alumon*>(m_pTarget)->Combat_DeadTick(TimeDelta);
	})
	


		.Build();

	__super::Initialize(&builder);
	return S_OK;
}

CBossFsm * CBossFsm::Create(CGameObject * pTarget)
{
	CBossFsm*		pInstance = new CBossFsm();

	if (FAILED(pInstance->Initialize(pTarget)))
	{
		MSG_BOX("Failed to Created : CBossFsm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossFsm::Free()
{
	__super::Free();
}



