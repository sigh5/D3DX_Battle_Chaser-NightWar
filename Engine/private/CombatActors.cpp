#include "..\public\CombatActors.h"
#include "Model.h"
#include "Collider.h"
#include "Status.h"

CCombatActors::CCombatActors(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CCombatActors::CCombatActors(const CCombatActors & rhs)
	: CGameObject(rhs)
	, m_iAnimIndex(rhs.m_iAnimIndex)
{

}


void CCombatActors::Create_Hit_Effect()
{
}

void CCombatActors::Create_Heavy_Hit_Effect()
{
}

void CCombatActors::Create_Defence_Effect_And_Action()
{
}

void CCombatActors::Calculator_HitDamage()
{
}

void CCombatActors::Is_Hit_DebuffSkill()
{
}

void CCombatActors::Set_StunState()
{
}

void CCombatActors::Is_Stuning()
{
}

void CCombatActors::Set_Misstate()
{
}

void CCombatActors::WideBuff_Status(CStatus * pStatus, _int iOption, _int iAmount)
{
	if (iOption == 0)
		pStatus->Incrase_Hp(iAmount);
	else if (iOption == 1)
		pStatus->Incrase_Mp(iAmount);
}

_bool CCombatActors::Is_DebuffBlend(CStatus* pStatus, CHitBoxObject::WEAPON_OPTIONAL eWeaponOption, OUT _int* iCurDamage, OUT wstring& pDebuffTag)
{
	_int iChangeValue = *iCurDamage;

	CStatus::DEBUFFTYPE		eType = CStatus::DEBUFFTYPE::DEBUFF_NONE;

	if (pStatus->Get_DebuffType().isDebuff_FIRE)
	{
		if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_RED_KNOLAN_SKILL2)
		{
			pDebuffTag = TEXT("fire critical");
			(*iCurDamage) += 10;
			eType = CStatus::DEBUFFTYPE::DEBUFF_FIRE;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_RED_KNOLAN_NORMAL)
		{
			(*iCurDamage) += 5;
			pDebuffTag = TEXT("fire critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_FIRE;
			pStatus->Set_DebuffOption(eType, false);
		}

	}
	if (pStatus->Get_DebuffType().isDebuff_BLEED)
	{

		if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_GARRISON_NORMAL)
		{
			(*iCurDamage) += 5;
			pDebuffTag = TEXT("bleeding");
			eType = CStatus::DEBUFFTYPE::DEBUFF_BLEED;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_GARRISON_SKILL2)
		{
			(*iCurDamage) += 10;
			pDebuffTag = TEXT("bleeding");
			eType = CStatus::DEBUFFTYPE::DEBUFF_BLEED;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_SPIDER_ATTACK)
		{
			(*iCurDamage) += 5;
			pDebuffTag = TEXT("bleeding");
			eType = CStatus::DEBUFFTYPE::DEBUFF_BLEED;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_SPIDER_HEAD)
		{
			(*iCurDamage) += 10;
			pDebuffTag = TEXT("bleeding");
			eType = CStatus::DEBUFFTYPE::DEBUFF_BLEED;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_BOSS_WHIP)
		{
			(*iCurDamage) += 10;
			pDebuffTag = TEXT("bleeding");
			eType = CStatus::DEBUFFTYPE::DEBUFF_BLEED;
			pStatus->Set_DebuffOption(eType, false);
		}

	}
	if (pStatus->Get_DebuffType().isDebuff_MAGIC)
	{

		if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_RED_KNOLAN_SKILL2)
		{
			(*iCurDamage) += 10;
			pDebuffTag = TEXT("magic critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_MAGIC;
			pStatus->Set_DebuffOption(eType, false);
		}
		else  if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_RED_KNOLAN_SKILL1)
		{
			(*iCurDamage) += 8;
			pDebuffTag = TEXT("magic critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_MAGIC;
			pStatus->Set_DebuffOption(eType, false);
		}
		else  if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_RED_KNOLAN_NORMAL)
		{
			(*iCurDamage) += 5;
			pDebuffTag = TEXT("magic critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_MAGIC;
			pStatus->Set_DebuffOption(eType, false);
		}
		else  if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_SLIME_KING_HEAD)
		{
			(*iCurDamage) += 10;
			pDebuffTag = TEXT("magic critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_MAGIC;
			pStatus->Set_DebuffOption(eType, false);
		}
		else  if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_SLIME_KING_BREATH)
		{
			(*iCurDamage) += 15;
			pDebuffTag = TEXT("magic critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_MAGIC;
			pStatus->Set_DebuffOption(eType, false);
		}
	}

	if (pStatus->Get_DebuffType().isDebuff_ARMOR)
	{
		if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_PULPLE)
		{
			(*iCurDamage) += 8;
			pDebuffTag = TEXT("armor critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_ARMOR;
			pStatus->Set_DebuffOption(eType, false);

		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_GARRISON_NORMAL)
		{
			(*iCurDamage) += 8;
			pDebuffTag = TEXT("armor critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_ARMOR;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_GARRISON_SKILL1)
		{
			(*iCurDamage) += 15;
			pDebuffTag = TEXT("armor critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_ARMOR;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_PUNCH_HIT)
		{
			(*iCurDamage) += 8;
			pDebuffTag = TEXT("armor critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_ARMOR;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_PUNCH_GUN)
		{
			(*iCurDamage) += 15;
			pDebuffTag = TEXT("armor critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_ARMOR;
			pStatus->Set_DebuffOption(eType, false);
		}
		else if (eWeaponOption == Engine::CHitBoxObject::WEAPON_OPTIONAL_BOSS_SHILED)
		{
			(*iCurDamage) += 15;
			pDebuffTag = TEXT("armor critical");
			eType = CStatus::DEBUFFTYPE::DEBUFF_ARMOR;
			pStatus->Set_DebuffOption(eType, false);
		}

	}

	if (iChangeValue < (*iCurDamage))
	{
		return true;
	}
	return false;
}

void CCombatActors::Create_Wide_Debuff(CStatus::DEBUFFTYPE eDebuffOption)
{
}

void CCombatActors::Initialize_CombatSound()
{
}




HRESULT CCombatActors::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCombatActors::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCombatActors::Last_Initialize()
{
	return S_OK;
}

void CCombatActors::Tick(_double TimeDelta)
{
}

void CCombatActors::Late_Tick(_double TimeDelta)
{
}

HRESULT CCombatActors::Render()
{
	return S_OK;
}

void CCombatActors::Set_Debuff(CStatus* pStatus, CStatus::DEBUFFTYPE eDebuff)
{
	pStatus->Set_DebuffOption(eDebuff);

}

void CCombatActors::CurAnimQueue_Play_Tick(_double Time, CModel * pModel)
{
	if (!m_CurAnimqeue.empty() && pModel->Get_Finished(pModel->Get_AnimIndex()))
	{
		m_bIsCombatAndAnimSequnce = false;
		m_iOldAnim = pModel->Get_AnimIndex();
		_uint i = m_CurAnimqeue.front().first;
		pModel->Set_AnimIndex(i);
		pModel->Set_AnimTickTime(m_CurAnimqeue.front().second);
		m_CurAnimqeue.pop();
		m_bFinishOption = ANIM_CONTROL_NEXT;

		if (m_CurAnimqeue.empty())
		{
			m_bIsCombatAndAnimSequnce = true;
		}
	}
	Defence_Exit();
}

void CCombatActors::CurAnimQueue_Play_LateTick(CModel * pModel)
{
	if (m_bFinishOption == ANIM_CONTROL_NEXT)
	{
		pModel->Set_Finished(m_iOldAnim, false);
		m_bFinishOption = ANIM_CONTROL_SEQUNCE;
		m_bIsIdle = false;
	}

	if (m_bIsCombatAndAnimSequnce && m_CurAnimqeue.empty() && pModel->Get_Finished(pModel->Get_AnimIndex()))
	{
		pModel->Set_PlayTime(pModel->Get_AnimIndex());
		m_bIsIdle = true;
	}

}

void CCombatActors::Set_CombatAnim_Index(CModel * pModel)
{
	for (_uint i = 0; i < _uint(CHAR_STATE_END); ++i)
		Set_FsmState(false, CHAR_STATE(i));

	pModel->Set_PlayTime(pModel->Get_AnimIndex());
	pModel->InitChannel();
	pModel->Set_AnimIndex(m_CurAnimqeue.front().first);
	pModel->Set_AnimTickTime(m_CurAnimqeue.front().second);
	m_bIsIdle = false;
	m_CurAnimqeue.pop();
}

void CCombatActors::Combat_DeadTick(_double TimeDelta)
{
}

CGameObject * CCombatActors::Get_Weapon_Or_SkillBody()
{
	return nullptr;
}


void CCombatActors::Fsm_Exit()
{
}

_bool CCombatActors::Calculator_HitColl(CGameObject * pWeapon)
{
	return false;
}

void CCombatActors::HitActor_LoseHP(CGameObject * pCurActor)
{
	// 데미지로직짜기
	_bool b = false;
}

void CCombatActors::Defence_Exit()
{
}

void CCombatActors::Free()
{
	__super::Free();

	for (auto& pBuff : m_vecBuffImage)
	{
		Safe_Release(pBuff);
	}
	m_vecBuffImage.clear();

	while (!m_CurAnimqeue.empty())
		m_CurAnimqeue.pop();

	m_pHitTarget = nullptr;
}
