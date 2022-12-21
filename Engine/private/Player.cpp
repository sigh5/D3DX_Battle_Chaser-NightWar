#include "..\public\Player.h"

#include "Input_Device.h"
#include "Model.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
	, m_bControlKeyInput(rhs.m_bControlKeyInput)
	, m_iAnimIndex(rhs.m_iAnimIndex)
	, m_fWalkTime(rhs.m_fWalkTime)
	, m_fMoveSpeedRatio(m_fMoveSpeedRatio)
	, m_bKeyInput(rhs.m_bKeyInput)
	, m_bIsSwap(rhs.m_bIsSwap)
{
}

void CPlayer::Fsm_Exit()
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	ZeroMemory(&m_PlayerDesc, sizeof(m_PlayerDesc));

	if(nullptr != pArg)
		memcpy(&m_PlayerDesc, pArg, sizeof(m_PlayerDesc));

	if (FAILED(__super::Initialize(&m_PlayerDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CPlayer::Last_Initialize()
{
	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{

	/* 수정 필요*/
	LookAtTarget(TimeDelta);
	//ChaseTarget(TimeDelta);

}

void CPlayer::Late_Tick(_double TimeDelta)
{
}

HRESULT CPlayer::Render()
{
	return S_OK;
}

void CPlayer::Set_FollowTarget(CGameObject * pPlayer)
{
	m_bIsSwap = true;
	Safe_Release(m_pTargetPlayer);

	if (pPlayer != nullptr)
	{
		m_pTargetPlayer = pPlayer;
		Safe_AddRef(m_pTargetPlayer);
		NormalLightCharUI();
	}
	else
	{
		m_pTargetPlayer = nullptr;
		HighLightChar();
	}
	
}

void CPlayer::ChaseTarget(_double TimeDelta)
{	
	/*if (nullptr == m_pTargetPlayer)
		return;

	_vector vTargetPos = m_pTargetPlayer->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);

	if (m_bIsSwap == true)
		return;
	else if (!m_bIsSwap && !m_isStop)
		m_isStop = m_pTransformCom->JudgeChaseState(vTargetPos, 2.5f);

	if (m_isStop)
	{
		if(m_pTargetPlayer->m_bIsWalk)

		m_pTransformCom->Chase(vTargetPos, TimeDelta, 2.5f);
	}
	else
		m_iAnimIndex = 0;
	*/
}



void CPlayer::LookAtTarget(_double TimeDelta)
{
	if (m_pTargetPlayer == nullptr )
		return;

	_float4x4	TargetMat;
	XMStoreFloat4x4(&TargetMat,m_pTargetPlayer->Get_Transform()->Get_WorldMatrix());

	_vector vRight, vUp, vLook;

	memcpy(&vRight, &TargetMat._11, sizeof(_vector));
	memcpy(&vUp, &TargetMat._21, sizeof(_vector));
	memcpy(&vLook, &TargetMat._31, sizeof(_vector));

	_float3 vScale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight)*vScale.x );
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp)*vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook)*vScale.z);
	_vector vTargetPos = m_pTargetPlayer->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);

	if (m_bIsSwap == true)
		return;
	else if (!m_bIsSwap && !m_isStop)
		m_isStop = m_pTransformCom->JudgeChaseState(vTargetPos, 3.f);

	if (m_isStop)
	{
		if(static_cast<CPlayer*>(m_pTargetPlayer)->m_bIsWalk)
			m_pTransformCom->Chase_Speed(vTargetPos, TimeDelta, 0.5f, 2.5f);
		else
			m_pTransformCom->Chase_Speed(vTargetPos, TimeDelta, 1.f, 2.5f);
	}	
	else
		m_iAnimIndex = 0;
}

_bool CPlayer::IsCaptin()
{
	if (m_pTargetPlayer == nullptr)
		return true;

	return false;
}

void CPlayer::CurAnimQueue_Play_Tick(_double Time,CModel* pModel)
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

void CPlayer::CurAnimQueue_Play_LateTick(CModel* pModel)
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

void CPlayer::Set_CombatAnim_Index(CModel * pModel)
{
	for (_uint i = 0; i < _uint(CHAR_STATE_END); ++i)
		Set_FsmState(false, CHAR_STATE(i));

	pModel->Set_PlayTime(pModel->Get_AnimIndex());
	pModel->InitChannel();
	//pModel->Set_Lerp(pModel->Get_AnimIndex(), m_CurAnimqeue.front().first);
	pModel->Set_AnimIndex(m_CurAnimqeue.front().first);
	pModel->Set_AnimTickTime(m_CurAnimqeue.front().second);
	m_bIsIdle = false;
	m_CurAnimqeue.pop();
}


_bool CPlayer::KeyInput(_double TimeDelta, CNavigation* pNavigation)
{

	if (!m_bControlKeyInput)
		return false;

	_float4 vTargetPos;
	XMStoreFloat4(&vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	CInput_Device* pInputDev = GET_INSTANCE(CInput_Device);

	if (pInputDev->Key_Pressing(DIK_DOWN))
	{
		vTargetPos.z -= 1.f;
		if (pInputDev->Key_Pressing(DIK_RIGHT))
		{
			vTargetPos.z += 0.5f;
			vTargetPos.x += 0.5f;
		}
		else if (pInputDev->Key_Pressing(DIK_LEFT))
		{
			vTargetPos.z += 0.5f;
			vTargetPos.x -= 0.5f;
		}
		m_bKeyInput = true;

	}
	else if (pInputDev->Key_Pressing(DIK_UP))
	{
		vTargetPos.z += 1.f;
		if (pInputDev->Key_Pressing(DIK_RIGHT))
		{
			vTargetPos.z -= 0.5f;
			vTargetPos.x += 0.5f;
		}
		else if (pInputDev->Key_Pressing(DIK_LEFT))
		{
			vTargetPos.z -= 0.5f;
			vTargetPos.x -= 1.f;
		}
		m_bKeyInput = true;

	}
	else if (pInputDev->Key_Pressing(DIK_RIGHT))
	{
		m_bKeyInput = true;
		vTargetPos.x += 1.f;
	}
	else if (pInputDev->Key_Pressing(DIK_LEFT))
	{
		m_bKeyInput = true;
		vTargetPos.x -= 1.f;
	}
	else
		m_bKeyInput = false;

	RELEASE_INSTANCE(CInput_Device);

	if (m_bKeyInput)
	{
		m_bIsSwap = false;
		m_fWalkTime += TimeDelta* 1.0;
		m_pTransformCom->LookAt(XMLoadFloat4(&vTargetPos));

		if (m_fWalkTime >= 1.f)
		{
			m_iAnimIndex = 2;
			m_fMoveSpeedRatio = 1.f;
			m_bIsWalk = false;
		}
		else
		{
			m_iAnimIndex = 1;
			m_fMoveSpeedRatio = 0.5f;	
			m_bIsWalk = true;
		}
		m_pTransformCom->Go_Straight(TimeDelta, m_fMoveSpeedRatio, pNavigation);

		return true;
	}
	else
	{
		m_fWalkTime = 0.0;
		return false;
	}
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTargetPlayer);
}

