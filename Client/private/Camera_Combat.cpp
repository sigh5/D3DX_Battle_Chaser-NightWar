#include "stdafx.h"
#include "..\public\Camera_Combat.h"
#include "GameInstance.h"

#include "CombatController.h"
#include "Client_Manager.h"

CCamera_Combat::CCamera_Combat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Combat::CCamera_Combat(const CCamera_Combat & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Combat::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Combat::Initialize(void * pArg)
{
	m_ObjectName = TEXT("CombatLevel_Camera");

	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(-7.f, 21.7f, -11.88f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fMouse_sensitivity = 0.1f;
		CameraDesc.TransformDesc.fSpeedPerSec = 5.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Initialize(&CameraDesc)))
		return E_FAIL;


	if (FAILED(SetUp_Components()))
		return E_FAIL;



	_matrix		m_matWorld,matScale, matRotX, matRotY, matRotZ, matTrans;

	matScale	=	XMMatrixScaling(1.f, 1.f, 1.f);
	
	matRotX = XMMatrixRotationX(XMConvertToRadians(17.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(47.f));
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(-6.5f, 21.7f, -13.5f);

	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	
	XMStoreFloat4x4(&m_WorldMat, m_matWorld);
	m_pTransformCom->Set_WorldMatrix(m_WorldMat);


	m_vOriginPos = _float4(-6.5f, 21.7f, -13.5f, 1.f);

	return S_OK;
}

void CCamera_Combat::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	Camera_UltiMate_ZoomTick(TimeDelta);

	Camera_Shaking(TimeDelta);

	switch (m_bZoomMoveOption)
	{
	case Client::CCamera_Combat::CameraTarget_CurActor:
		Camera_ZoomIn_CurActor(TimeDelta);
		break;
	case Client::CCamera_Combat::CameraTarget_Hiter:
		Camera_ZoomIn_HitActor(TimeDelta);
		break;
	case Client::CCamera_Combat::CameraTarget_recover:
		Camera_ZoomOut(TimeDelta);
		break;
	case Client::CCamera_Combat::CameraTarget_END:
		break;
	default:
		break;
	}



	/*if(m_bUltimateCamWorkTick)
	{

		CGameObject* pPlayer = CCombatController::GetInstance()->Get_Player(TEXT("Hero_Gully"));

		_float4 vPos;
		XMStoreFloat4(&vPos, pPlayer->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));

		m_pTransformCom->Chase(XMLoadFloat4(&vPos), TimeDelta* 10.f, 10.f);
	}
	else
	{
		m_LeprFov += _float(TimeDelta * 10.f);
		if (m_fFov <= m_fFixFov)
		{
			m_fFov += m_LeprFov;
		}

		m_pTransformCom->Chase(XMLoadFloat4(&m_vOriginPos), TimeDelta, 3.f);
	}*/


	


#ifdef _DEBUG
	
	if (GetKeyState('W') & 0x8000)
	{
		m_pTransformCom->Go_Straight(TimeDelta);
	}

	if (GetKeyState('S') & 0x8000)
	{
		m_pTransformCom->Go_Backward(TimeDelta);
	}

	if (GetKeyState('A') & 0x8000)
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}

	if (GetKeyState('D') & 0x8000)
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}



	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Key_Down(DIK_T))
	{
		m_bCusorFix = !m_bCusorFix;
	}

	if (m_bCusorFix)
	{


		if (pGameInstance->Get_DIMouseState(CInput_Device::DIM_RB) & 0x80)
		{
			_long			MouseMove = 0;

			if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 20.f);
			}

			if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * 20.f);
			}
		}
	}
	else
	{
		_long			MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * 0.1f);
		}
	}

	RELEASE_INSTANCE(CGameInstance);

#endif
	



}

void CCamera_Combat::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CCamera_Combat::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bLast_Initlize = true;
	return S_OK;
}

HRESULT CCamera_Combat::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Combat::Camera_Shaking(_double TimeDelta)
{
	if (!m_bCameraShakingActive)
		return;

	_float4		vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	

	if (m_fShakeTime > m_fCurShakeTime)
	{
		m_fCurShakeTime += _float(TimeDelta);
		_float ShakefX = 0.f;
		_float ShakefY = 0.f;
		if (false==m_bXShaking)
		{ 
			ShakefX = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;
			ShakefY = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;
		}
		else
		{
			ShakefX = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;
		}
		_matrix matLocal = XMMatrixTranslation(ShakefX, ShakefY, 0.f);
		
		_matrix matWorld = m_pTransformCom->Get_WorldMatrix();

		matWorld = matLocal*matWorld;

		_float4x4 vShakingMat;
		XMStoreFloat4x4(&vShakingMat, matWorld);
		
		m_pTransformCom->Set_WorldMatrix(vShakingMat);

	}
	else
	{
		m_pTransformCom->Set_WorldMatrix(m_WorldMat);
		m_bCameraShakingActive = false;
		m_bXShaking = false;
	}


}

void CCamera_Combat::Camera_ZoomIn_CurActor(_double TimeDelta)
{
	CCombatController* pCombatCotroller = GET_INSTANCE(CCombatController);
	
	CGameObject* pCurActor = pCombatCotroller->Get_CurActor();

	if (nullptr == pCurActor && m_bZoomMoveOption == CameraTarget_CurActor)
	{
		RELEASE_INSTANCE(CCombatController);
		return;
	}

	m_fZoomActor_to_Hiter_Timer  += _float(TimeDelta);
	m_LeprFov += _float(TimeDelta * 5.f);

	if (m_fFov >= m_fZoomInFov)
	{
		m_fFov -= m_LeprFov;
	}
	_float vDir = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vOriginPos) - 
		m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)));

	if (vDir >= 4.f)
	{
		RELEASE_INSTANCE(CCombatController);
		return;
	}
	_vector vTargetPos = pCurActor->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	m_pTransformCom->Chase(vTargetPos, TimeDelta, 36.f);

	RELEASE_INSTANCE(CCombatController);
}

void CCamera_Combat::Camera_ZoomIn_HitActor(_double TimeDelta)
{

	m_fZoomHiter_to_Actor_Timer += _float(TimeDelta);

	CCombatController* pCombatCotroller = GET_INSTANCE(CCombatController);

	CGameObject* pCurActor = pCombatCotroller->Get_HitActor();

	if (nullptr == pCurActor && m_bZoomMoveOption == CameraTarget_Hiter)
	{
		RELEASE_INSTANCE(CCombatController);
		return;
	}

	_float vDir = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vOriginPos) -
		m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION)));

	if (vDir >= 6.f)
	{
		RELEASE_INSTANCE(CCombatController);
		return;
	}

	_vector vTargetPos = pCurActor->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	m_pTransformCom->Chase(vTargetPos, TimeDelta, 36.f);

	RELEASE_INSTANCE(CCombatController);

}

void CCamera_Combat::Camera_ZoomOut(_double TimeDelta)
{
	m_LeprFov += _float(TimeDelta * 10.f);
	if (m_fFov <= m_fFixFov)
	{
		m_fFov += m_LeprFov;
	}

	m_pTransformCom->Chase(XMLoadFloat4(&m_vOriginPos), TimeDelta, 3.f);


}

void CCamera_Combat::UltimateStart_CameraWork(CGameObject * pCurActor)
{
	_matrix		matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	_float4x4 UntiMat;
	if (!lstrcmp(pCurActor->Get_ObjectName(), TEXT("Hero_Gully")))
	{
		/*matScale = XMMatrixScaling(1.f, 1.f, 1.f);

		matRotX = XMMatrixRotationX(XMConvertToRadians(18.f));
		matRotY = XMMatrixRotationY(XMConvertToRadians(21.7f));
		matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
		matTrans = XMMatrixTranslation(-4.05f, 5.13f, 16.127f);

		matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

		XMStoreFloat4x4(&UntiMat, matWorld);
		m_pTransformCom->Set_WorldMatrix(UntiMat);*/

		m_bUltimateCamWorkTick[UlTIMATE_TARGET_KNOLAN] = true;

	}
	else if (!lstrcmp(pCurActor->Get_ObjectName(), TEXT("Hero_Alumon")))
	{
		/*matScale = XMMatrixScaling(1.f, 1.f, 1.f);

		matRotX = XMMatrixRotationX(XMConvertToRadians(20.115f));
		matRotY = XMMatrixRotationY(XMConvertToRadians(18.5f));
		matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
		matTrans = XMMatrixTranslation(7.6f, 9.5f, 1.95f);

		matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

		XMStoreFloat4x4(&UntiMat, matWorld);
		m_pTransformCom->Set_WorldMatrix(UntiMat);*/
		m_bUltimateCamWorkTick[UlTIMATE_TARGET_ALUMON] = true;

	}
	else if (!lstrcmp(pCurActor->Get_ObjectName(), TEXT("Hero_Calibretto")))
	{
	/*	matScale = XMMatrixScaling(1.f, 1.f, 1.f);

		matRotX = XMMatrixRotationX(XMConvertToRadians(34.f));
		matRotY = XMMatrixRotationY(XMConvertToRadians(48.2f));
		matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
		matTrans = XMMatrixTranslation(-3.15f, 16.f, 19.8f);

		matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

		XMStoreFloat4x4(&UntiMat, matWorld);
		m_pTransformCom->Set_WorldMatrix(UntiMat);*/
	
		m_bUltimateCamWorkTick[UlTIMATE_TARGET_CALLIBRETTO] = true;
	}
	else
		return;


}

void CCamera_Combat::Ultimate_EndCameraWork()
{
	
	m_pTransformCom->Set_WorldMatrix(m_WorldMat);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->Set_Timedelta(TEXT("Timer_60"), 1.0);
	RELEASE_INSTANCE(CGameInstance);
	Reset_Ultimate_Cam();
	
	
}

void CCamera_Combat::Camera_UltiMate_ZoomTick(_double TimeDelta)
{
	if (   !m_bUltimateCamWorkTick[UlTIMATE_TARGET_KNOLAN] 
		&& !m_bUltimateCamWorkTick[UlTIMATE_TARGET_ALUMON]
		&& !m_bUltimateCamWorkTick[UlTIMATE_TARGET_CALLIBRETTO]
		&& !m_bUltimateCamWorkTick[UlTIMATE_TARGET_BOSS])
		return;
	
	CGameObject* pPlayer = CCombatController::GetInstance()->Get_CurActor();


	_float4 vPos;
	_float	vRange = 0.f;
	XMStoreFloat4(&vPos, pPlayer->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));

	if (m_bUltimateCamWorkTick[UlTIMATE_TARGET_KNOLAN])
	{
		vPos.x -= 5.f;
		vRange = 10.f;
		
	}
	else if (m_bUltimateCamWorkTick[UlTIMATE_TARGET_ALUMON])
	{
		vPos.x -= 2.f;		// Alumon ÀºÇÕ°Ý
		vRange = 10.f;
		
	}
	else if (m_bUltimateCamWorkTick[UlTIMATE_TARGET_CALLIBRETTO])
	{
		vPos.x -= 5.f;
		vRange = 20.f;

	}
	else if (m_bUltimateCamWorkTick[UlTIMATE_TARGET_BOSS])
	{

	}
	
	m_pTransformCom->Chase(XMLoadFloat4(&vPos), TimeDelta* 20.f, vRange);

	//m_pTransformCom->Chase(XMLoadFloat4(&vPos), TimeDelta* 30.f, 10.f);
}

HRESULT CCamera_Combat::SetUp_Components()
{
	return S_OK;
}



CCamera_Combat * CCamera_Combat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Combat*		pInstance = new CCamera_Combat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Combat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Combat::Clone(void * pArg)
{
	CCamera_Combat*		pInstance = new CCamera_Combat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Combat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Combat::Free()
{
	__super::Free();

}