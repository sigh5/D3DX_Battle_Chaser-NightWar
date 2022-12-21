#include "stdafx.h"
#include "AnimFsm.h"

#include "Player.h"
#include "CombatController.h"
#include "PlayerController.h"
#include "Hero_Garrison.h"
#include "Hero_Knolan.h"
#include "Hero_Calibretto.h"



// ¾îÂ÷ÇÇ »ý¼ºÀº ³ªÁß¿¡ ÇÒ°ÅÀÓ

CAnimFsm::CAnimFsm()
	: m_pPlayerController(CPlayerController::GetInstance())
	, m_pCombatController(CCombatController::GetInstance())
{
}


HRESULT CAnimFsm::Initialize(CGameObject * pTarget, AnimType eType)
{
	HRESULT hr = 0;

	m_pTarget = pTarget;

	switch (eType)
	{
	case Client::ANIM_CHAR1:
		hr = Init_Knolan(pTarget);
		break;
	case Client::ANIM_CHAR2:
		hr = Init_Garrison(pTarget);
		break;
	case Client::ANIM_CHAR3:
		hr = Init_Calibretto(pTarget);
		break;
	default:
		break;
	}

	return hr;
}

HRESULT CAnimFsm::Init_Knolan(CGameObject * pTarget)
{
	assert(pTarget != nullptr);
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
		.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Idle();
			})
			.Tick([this](_double TimeDelta) {
			static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta); })
			.Transition(TEXT("Intro"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]() {return m_pCombatController->To_Intro(); }))
			.Transition(TEXT("NormalAttack"), FSM_TRANSITION(TEXT("Idle To NormalAttack"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Normal_Attack(); }))
			.Transition(TEXT("SkillAttack1"), FSM_TRANSITION(TEXT("Idle To SkillAttack1"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Skill1_Attack(); }))
			.Transition(TEXT("SkillAttack2"), FSM_TRANSITION(TEXT("Idle To SkillAttack2"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Skill2_Attack(); }))
			.Transition(TEXT("Uitimate"), FSM_TRANSITION(TEXT("Idle To Uitimate"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Uitimate(); }))
			.Transition(TEXT("Buff"), FSM_TRANSITION(TEXT("Idle To Buff"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Buff(); }))
			.Transition(TEXT("WideBuff"), FSM_TRANSITION(TEXT("Idle To WideBuff"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_WideAreaBuff(); }))
			.Transition(TEXT("Defence"), FSM_TRANSITION(TEXT("Idle To Defence"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Defence(); }))
			.Transition(TEXT("UseItem"), FSM_TRANSITION(TEXT("Idle To UseItem"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Use_Item(); }))
			.Transition(TEXT("Light_Hit"), FSM_TRANSITION(TEXT("Idle To Light_Hit"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Light_Hit(); }))
			.Transition(TEXT("Heavy_Hit"), FSM_TRANSITION(TEXT("Idle To Heavy_Hit"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Heavy_Hit(); }))
			.Transition(TEXT("Flee"), FSM_TRANSITION(TEXT("Idle To Flee"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Flee(); }))
			.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Idle To Die"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Die(); }))
			.Transition(TEXT("Viroty"), FSM_TRANSITION(TEXT("Idle To Viroty"), [this]() {return static_cast<CHero_Knolan*>(m_pTarget)->To_Viroty(); }))

		.AddState(L"Intro")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Intro();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
				.OnExit([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Intro_Exit();
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"NormalAttack")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->AnimNormalAttack();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.OnExit([this]()
			{
				static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"SkillAttack1")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Skill1_Attack();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.OnExit([this]()
			{
				static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"SkillAttack2")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Skill2_Attack();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.OnExit([this]()
			{
				static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"Uitimate")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Uitimate();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.OnExit([this]()
			{
				static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"Buff")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Buff();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.OnExit([this]()
			{
				static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"WideBuff")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_WideAreaBuff();
			})
				.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
				.OnExit([this]()
			{
				static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
			})
				.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"UseItem")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Use_Item();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
				.OnExit([this]()
			{
				static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))


			.AddState(L"Defence")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Defence();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
				.OnExit([this]()
			{
				static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))


			.AddState(L"Light_Hit")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Light_Hit();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"Heavy_Hit")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Heavy_Hit();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"Flee")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Flee();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return m_pCombatController->To_Idle();
			}))

		.AddState(L"Die")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Die();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return m_pCombatController->To_Idle();
			}))

		.AddState(L"Viroty")
			.OnStart([this]()
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Anim_Viroty();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Knolan*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return m_pCombatController->To_Idle();
			}))

		.Build();
		__super::Initialize(&builder);

		return S_OK;
}

HRESULT CAnimFsm::Init_Garrison(CGameObject * pTarget)
{
	assert(pTarget != nullptr);
	CFSMComponentBuilder builder = CFSMComponentBuilder()
	.InitState(TEXT("Idle"))

	.AddState(TEXT("Idle"))
		.OnStart([this]()
	{
		static_cast<CHero_Garrison*>(m_pTarget)->Anim_Idle();
	})
			.Tick([this](_double TimeDelta) {
					static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta); })
			.Transition(TEXT("Intro"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]() {return m_pCombatController->To_Intro(); }))
			.Transition(TEXT("NormalAttack"), FSM_TRANSITION(TEXT("Idle To NormalAttack"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Normal_Attack(); }))
			.Transition(TEXT("SkillAttack1"), FSM_TRANSITION(TEXT("Idle To SkillAttack1"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Skill1_Attack(); }))
			.Transition(TEXT("SkillAttack2"), FSM_TRANSITION(TEXT("Idle To SkillAttack2"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Skill2_Attack(); }))
			.Transition(TEXT("Uitimate"), FSM_TRANSITION(TEXT("Idle To Uitimate"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Uitimate(); }))
			.Transition(TEXT("Buff"), FSM_TRANSITION(TEXT("Idle To Buff"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Buff(); }))
			.Transition(TEXT("Defence"), FSM_TRANSITION(TEXT("Idle To Defence"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Defence(); }))
			.Transition(TEXT("UseItem"), FSM_TRANSITION(TEXT("Idle To UseItem"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Use_Item(); }))
			.Transition(TEXT("Light_Hit"), FSM_TRANSITION(TEXT("Idle To Light_Hit"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Light_Hit(); }))
			.Transition(TEXT("Heavy_Hit"), FSM_TRANSITION(TEXT("Idle To Heavy_Hit"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Heavy_Hit(); }))
			.Transition(TEXT("Flee"), FSM_TRANSITION(TEXT("Idle To Flee"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Flee(); }))
			.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Idle To Die"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Die(); }))
			.Transition(TEXT("Viroty"), FSM_TRANSITION(TEXT("Idle To Viroty"), [this]() {return static_cast<CHero_Garrison*>(m_pTarget)->To_Viroty(); }))

		.AddState(L"Intro")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Anim_Intro();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))


			.AddState(L"NormalAttack")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->AnimNormalAttack();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))


			.AddState(L"SkillAttack1")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Anim_Skill1_Attack();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_BlendAnimTick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

			.AddState(L"SkillAttack2")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Anim_Skill2_Attack();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

			.AddState(L"Uitimate")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Anim_Uitimate();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_Ultimate(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

			.AddState(L"Buff")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Anim_Uitimate();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

			.AddState(L"UseItem")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Anim_Use_Item();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))


			.AddState(L"Defence")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Anim_Defence();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))


			.AddState(L"Light_Hit")
			.OnStart([this]()
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Anim_Light_Hit();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"Heavy_Hit")
			.OnStart([this]()
			{
				static_cast<CHero_Garrison*>(m_pTarget)->Anim_Heavy_Hit();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
			}))

		.AddState(L"Flee")
			.OnStart([this]()
			{
				static_cast<CHero_Garrison*>(m_pTarget)->Anim_Flee();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return m_pCombatController->To_Idle();		//¾ê´Â µµ¸ÁÄ¡´Â°Å
			}))

		.AddState(L"Die")
			.OnStart([this]()
			{
				static_cast<CHero_Garrison*>(m_pTarget)->Anim_Die();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return m_pCombatController->To_Idle();	// ¾ê´Â Á×´Â°Å
			}))

		.AddState(L"Viroty")
			.OnStart([this]()
			{
				static_cast<CHero_Garrison*>(m_pTarget)->Anim_Viroty();
			})
			.Tick([this](_double TimeDelta)
			{
				static_cast<CHero_Garrison*>(m_pTarget)->Combat_Tick(TimeDelta);
			})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
			{
				return m_pCombatController->To_Idle();		//¾ê´Â ½Â¸®
			}))

			.Build();
		__super::Initialize(&builder);

		return S_OK;
}

HRESULT CAnimFsm::Init_Calibretto(CGameObject * pTarget)
{
	assert(pTarget != nullptr);
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
			.OnStart([this]()
			{
				static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Idle();})
			.Tick([this](_double TimeDelta) {
				static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta); 
				})
			.Transition(TEXT("Intro"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]() {return m_pCombatController->To_Intro(); }))
			.Transition(TEXT("NormalAttack"), FSM_TRANSITION(TEXT("Idle To NormalAttack"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Normal_Attack(); }))
			.Transition(TEXT("SkillAttack1"), FSM_TRANSITION(TEXT("Idle To SkillAttack1"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Skill1_Attack(); }))
			.Transition(TEXT("SkillAttack2"), FSM_TRANSITION(TEXT("Idle To SkillAttack2"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Skill2_Attack(); }))
			.Transition(TEXT("Uitimate"), FSM_TRANSITION(TEXT("Idle To Uitimate"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Uitimate(); }))
			.Transition(TEXT("Buff"), FSM_TRANSITION(TEXT("Idle To Buff"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Buff(); }))
			.Transition(TEXT("WideBuff"), FSM_TRANSITION(TEXT("Idle To WideBuff"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_WideAreaBuff(); }))
			.Transition(TEXT("UseItem"), FSM_TRANSITION(TEXT("Idle To UseItem"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Use_Item(); }))
			.Transition(TEXT("Light_Hit"), FSM_TRANSITION(TEXT("Idle To Light_Hit"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Light_Hit(); }))
			.Transition(TEXT("Heavy_Hit"), FSM_TRANSITION(TEXT("Idle To Heavy_Hit"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Heavy_Hit(); }))
			.Transition(TEXT("Flee"), FSM_TRANSITION(TEXT("Idle To Flee"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Flee(); }))
			.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Idle To Die"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Die(); }))
			.Transition(TEXT("Viroty"), FSM_TRANSITION(TEXT("Idle To Viroty"), [this]() {return static_cast<CHero_Calibretto*>(m_pTarget)->To_Viroty(); }))

		.AddState(L"Intro")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Intro();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Intro_Exit();
		
		})
			
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"NormalAttack")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->AnimNormalAttack();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"SkillAttack1")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Skill1_Attack();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"SkillAttack2")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Skill2_Attack();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"Uitimate")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Uitimate();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"Buff")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Uitimate();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"WideBuff")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_WideAreaBuff();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"UseItem")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Use_Item();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.OnExit([this]()
		{
			static_cast<CPlayer*>(m_pTarget)->Fsm_Exit();
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"Light_Hit")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Light_Hit();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"Heavy_Hit")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Heavy_Hit();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return  static_cast<CPlayer*>(m_pTarget)->Get_IsIdle();
		}))

		.AddState(L"Flee")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Flee();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return m_pCombatController->To_Idle();
		}))

		.AddState(L"Die")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Die();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return m_pCombatController->To_Idle();
		}))

		.AddState(L"Viroty")
			.OnStart([this]()
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Anim_Viroty();
		})
			.Tick([this](_double TimeDelta)
		{
			static_cast<CHero_Calibretto*>(m_pTarget)->Combat_Tick(TimeDelta);
		})
			.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
		{
			return m_pCombatController->To_Idle();
		}))

		.Build();
	
	__super::Initialize(&builder);

		return S_OK;
}




CAnimFsm * CAnimFsm::Create(CGameObject *pTarget, AnimType eType)
{
	CAnimFsm*		pInstance = new CAnimFsm();

	if (FAILED(pInstance->Initialize(pTarget, eType)))
	{
		MSG_BOX("Failed to Created : CAnimFsm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimFsm::Free()
{
	__super::Free();

	
}



