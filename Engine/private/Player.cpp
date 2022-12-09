#include "..\public\Player.h"

#include "Input_Device.h"
#include "Model.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
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
	return E_NOTIMPL;
}

void CPlayer::Tick(_double TimeDelta)
{
	ChaseTarget(TimeDelta);
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
	if (pPlayer == nullptr)
		return;

	Safe_Release(m_pTargetPlayer);
	m_pTargetPlayer = pPlayer;
	Safe_AddRef(m_pTargetPlayer);
}

void CPlayer::ChaseTarget(_double TimeDelta)
{
	if (nullptr == m_pTargetPlayer)
		return;

	_vector vTargetPos = m_pTargetPlayer->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	m_pTransformCom->Chase(vTargetPos, TimeDelta, 3.f);

}

void CPlayer::KeyInput(_double TimeDelta, CModel* pModel)
{

	if (!m_bControlKeyInput)
		return;

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

	if (m_bKeyInput)
	{
		m_fWalkTime += TimeDelta* 1.0;
		m_pTransformCom->LookAt(XMLoadFloat4(&vTargetPos));

		if (m_fWalkTime >= 1.f)
		{
			pModel->Set_AnimIndex(2);
			m_fMoveSpeedRatio = 1.f;
		}
		else
		{
			pModel->Set_AnimIndex(1);
			m_fMoveSpeedRatio = 0.5f;
		}
		m_pTransformCom->Go_Straight(TimeDelta, m_fMoveSpeedRatio);
	}
	else
	{
		pModel->Set_AnimIndex(0);
		m_fWalkTime = 0.0;
	}

	RELEASE_INSTANCE(CInput_Device);



}


void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pTargetPlayer);
}
