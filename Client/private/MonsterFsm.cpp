#include "stdafx.h"
#include "..\public\MonsterFsm.h"

#include "CombatController.h"
#include "SlimeKing.h"
#include "Skeleton_Naked.h"
#include "Spider_Mana.h"

CMonsterFsm::CMonsterFsm()
	: m_pCombatController(CCombatController::GetInstance())
{
}

HRESULT CMonsterFsm::Initialize(CGameObject * pTarget, AnimType eType)
{
	HRESULT hr = 0;

	m_pTarget = pTarget;

	switch (eType)
	{
	case Client::ANIM_CHAR1:
		hr = Init_SlimeKing(pTarget);
		break;
	case Client::ANIM_CHAR2:
		hr = Init_Skelton_Naked(pTarget);
		break;
	case Client::ANIM_CHAR3:
		hr = Init_Spider_Mana(pTarget);
		break;
	default:
		break;
	}

	return hr;
}

HRESULT CMonsterFsm::Init_SlimeKing(CGameObject * pTarget)
{
	assert(pTarget != nullptr);
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Idle();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Intro"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]() {return m_pCombatController->To_Intro(); }))
		.Transition(TEXT("NormalAttack"), FSM_TRANSITION(TEXT("Idle To NormalAttack"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Normal_Attack(); }))
		.Transition(TEXT("SkillAttack1"), FSM_TRANSITION(TEXT("Idle To SkillAttack1"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Skill1_Attack(); }))
		.Transition(TEXT("Uitimate"), FSM_TRANSITION(TEXT("Idle To Uitimate"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Uitimate(); }))
		.Transition(TEXT("Buff"), FSM_TRANSITION(TEXT("Idle To Buff"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Buff(); }))
		.Transition(TEXT("UseItem"), FSM_TRANSITION(TEXT("Idle To UseItem"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Use_Item(); }))
		.Transition(TEXT("Light_Hit"), FSM_TRANSITION(TEXT("Idle To Light_Hit"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Light_Hit(); }))
		.Transition(TEXT("Heavy_Hit"), FSM_TRANSITION(TEXT("Idle To Heavy_Hit"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Heavy_Hit(); }))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Idle To Die"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Die(); }))
		.Transition(TEXT("Viroty"), FSM_TRANSITION(TEXT("Idle To Viroty"), [this]() {return static_cast<CSlimeKing*>(m_pTarget)->To_Viroty(); }))

		.AddState(L"Intro")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Intro();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"NormalAttack")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->AnimNormalAttack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
	.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})

		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"SkillAttack1")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Skill1_Attack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))


		.AddState(L"Uitimate")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Uitimate();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"Buff")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Buff();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

		.AddState(L"Light_Hit")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Light_Hit();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Light_Hit To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	},1))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Light_Hit To Die"), [this]()
	{
		return  static_cast<CCombatActors*>(m_pTarget)->Is_Dead();
	}, 0))

		.AddState(L"Heavy_Hit")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Heavy_Hit();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Heavy_Hit To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	},1))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Heavy_Hit To Die"), [this]()
	{
		return  static_cast<CCombatActors*>(m_pTarget)->Is_Dead();
	}, 0))


		.AddState(L"Die")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Die();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
	//	.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	//{
	//	return m_pCombatController->To_Idle();
	//}))

		.AddState(L"Viroty")
		.OnStart([this]()
	{
		static_cast<CSlimeKing*>(m_pTarget)->Anim_Viroty();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSlimeKing*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
	

		.Build();

	__super::Initialize(&builder);
	return S_OK;
}

HRESULT CMonsterFsm::Init_Skelton_Naked(CGameObject * pTarget)
{
	assert(pTarget != nullptr);
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

		.AddState(TEXT("Idle"))
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Idle();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Intro"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]() {return m_pCombatController->To_Intro(); }))
		.Transition(TEXT("NormalAttack"), FSM_TRANSITION(TEXT("Idle To NormalAttack"), [this]() {return static_cast<CSkeleton_Naked*>(m_pTarget)->To_Normal_Attack(); }))
		.Transition(TEXT("SkillAttack1"), FSM_TRANSITION(TEXT("Idle To SkillAttack1"), [this]() {return static_cast<CSkeleton_Naked*>(m_pTarget)->To_Skill1_Attack(); }))
		.Transition(TEXT("Defence"), FSM_TRANSITION(TEXT("Idle To Defence"), [this]() {return static_cast<CSkeleton_Naked*>(m_pTarget)->To_Defence(); }))
		.Transition(TEXT("Buff"), FSM_TRANSITION(TEXT("Idle To Buff"), [this]() {return static_cast<CSkeleton_Naked*>(m_pTarget)->To_Buff(); }))
		.Transition(TEXT("Light_Hit"), FSM_TRANSITION(TEXT("Idle To Light_Hit"), [this]() {return static_cast<CSkeleton_Naked*>(m_pTarget)->To_Light_Hit(); }))
		.Transition(TEXT("Heavy_Hit"), FSM_TRANSITION(TEXT("Idle To Heavy_Hit"), [this]() {return static_cast<CSkeleton_Naked*>(m_pTarget)->To_Heavy_Hit(); }))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Idle To Die"), [this]() {return static_cast<CSkeleton_Naked*>(m_pTarget)->To_Die(); }))
		.Transition(TEXT("Viroty"), FSM_TRANSITION(TEXT("Idle To Viroty"), [this]() {return static_cast<CSkeleton_Naked*>(m_pTarget)->To_Viroty(); }))

	.AddState(L"Intro")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Intro();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"NormalAttack")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_NormalAttack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"SkillAttack1")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Skill1_Attack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))


	.AddState(L"Defence")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Defence();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"Buff")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Buff();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"Light_Hit")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Light_Hit();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Light_Hit To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	},1))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Light_Hit To Die"), [this]()
	{
		return  static_cast<CCombatActors*>(m_pTarget)->Is_Dead();
	}, 0))


	.AddState(L"Heavy_Hit")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Heavy_Hit();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Heavy_Hit To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	},1))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Heavy_Hit To Die"), [this]()
	{
		return  static_cast<CCombatActors*>(m_pTarget)->Is_Dead();
	}, 0))

	.AddState(L"Die")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Die();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
	//	.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	//{
	//	return m_pCombatController->To_Idle();
	//}))

	.AddState(L"Viroty")
		.OnStart([this]()
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Anim_Viroty();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
	

	.Build();

	__super::Initialize(&builder);
	return S_OK;
}

HRESULT CMonsterFsm::Init_Spider_Mana(CGameObject * pTarget)
{
	assert(pTarget != nullptr);
	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))

	.AddState(TEXT("Idle"))
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Idle();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Intro"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]() {return m_pCombatController->To_Intro(); }))
		.Transition(TEXT("NormalAttack"), FSM_TRANSITION(TEXT("Idle To NormalAttack"), [this]() {return static_cast<CSpider_Mana*>(m_pTarget)->To_Normal_Attack(); }))
		.Transition(TEXT("SkillAttack1"), FSM_TRANSITION(TEXT("Idle To SkillAttack1"), [this]() {return static_cast<CSpider_Mana*>(m_pTarget)->To_Skill1_Attack(); }))
		.Transition(TEXT("SkillAttack2"), FSM_TRANSITION(TEXT("Idle To SkillAttack2"), [this]() {return static_cast<CSpider_Mana*>(m_pTarget)->To_Skill2_Attack(); }))
		.Transition(TEXT("Buff"), FSM_TRANSITION(TEXT("Idle To Buff"), [this]() {return static_cast<CSpider_Mana*>(m_pTarget)->To_Buff(); }))
		.Transition(TEXT("Light_Hit"), FSM_TRANSITION(TEXT("Idle To Light_Hit"), [this]() {return static_cast<CSpider_Mana*>(m_pTarget)->To_Light_Hit(); }))
		.Transition(TEXT("Heavy_Hit"), FSM_TRANSITION(TEXT("Idle To Heavy_Hit"), [this]() {return static_cast<CSpider_Mana*>(m_pTarget)->To_Heavy_Hit(); }))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Idle To Die"), [this]() {return static_cast<CSpider_Mana*>(m_pTarget)->To_Die(); }))
		.Transition(TEXT("Viroty"), FSM_TRANSITION(TEXT("Idle To Viroty"), [this]() {return static_cast<CSpider_Mana*>(m_pTarget)->To_Viroty(); }))

	.AddState(L"Intro")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Intro();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("Idle To Intro"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"NormalAttack")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_NormalAttack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"SkillAttack1")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Skill1_Attack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))


	.AddState(L"SkillAttack2")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Skill2_Attack();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"Buff")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Buff();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.OnExit([this]()
	{
		static_cast<CMonster*>(m_pTarget)->Fsm_Exit();
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	}))

	.AddState(L"Light_Hit")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Light_Hit();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSkeleton_Naked*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	},1))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Heavy_Hit To Die"), [this]()
	{
		return  static_cast<CCombatActors*>(m_pTarget)->Is_Dead();
	}, 0))

	.AddState(L"Heavy_Hit")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Heavy_Hit();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
		.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return  static_cast<CMonster*>(m_pTarget)->Get_IsIdle();
	},1))
		.Transition(TEXT("Die"), FSM_TRANSITION(TEXT("Heavy_Hit To Die"), [this]()
	{
		return  static_cast<CCombatActors*>(m_pTarget)->Is_Dead();
	}, 0))

	.AddState(L"Die")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Die();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})
	/*	.Transition(TEXT("Idle"), FSM_TRANSITION(TEXT("NormalAttack To Idle"), [this]()
	{
		return m_pCombatController->To_Idle();
	}))*/

	.AddState(L"Viroty")
		.OnStart([this]()
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Anim_Viroty();
	})
		.Tick([this](_double TimeDelta)
	{
		static_cast<CSpider_Mana*>(m_pTarget)->Combat_Tick(TimeDelta);
	})



		.Build();

	__super::Initialize(&builder);
	return S_OK;
}

CMonsterFsm * CMonsterFsm::Create(CGameObject * pTarget, AnimType eType)
{
	CMonsterFsm*		pInstance = new CMonsterFsm();

	if (FAILED(pInstance->Initialize(pTarget, eType)))
	{
		MSG_BOX("Failed to Created : CMonsterFsm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterFsm::Free()
{
	__super::Free();
}


