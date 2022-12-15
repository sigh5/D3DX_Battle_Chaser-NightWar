#include "..\public\Monster.h"

#include "Model.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	:CGameObject(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMonster::Last_Initialize()
{
	return S_OK;
}

void CMonster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMonster::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::CurAnimQueue_Play_Tick(_double Time, CModel * pModel)
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
}

void CMonster::CurAnimQueue_Play_LateTick(CModel * pModel)
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

void CMonster::Set_CombatAnim_Index(CModel * pModel)
{
	for (_uint i = 0; i < _uint(CHAR_STATE_END); ++i)
		Set_FsmState(false, CHAR_STATE(i));

	pModel->Set_PlayTime(pModel->Get_AnimIndex());
	pModel->InitChannel();
//	pModel->Set_Lerp(pModel->Get_AnimIndex(), m_CurAnimqeue.front().first);
	pModel->Set_AnimIndex(m_CurAnimqeue.front().first);
	pModel->Set_AnimTickTime(m_CurAnimqeue.front().second);
	m_bIsIdle = false;
	m_CurAnimqeue.pop();
}

void CMonster::Free()
{
	__super::Free();
}
