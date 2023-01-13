#include "..\public\CombatActors.h"
#include "Model.h"
#include "Collider.h"

CCombatActors::CCombatActors(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
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

void CCombatActors::Create_Heacy_Hit_Effect()
{
}

void CCombatActors::Create_Defence_Effect_And_Action()
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
}
