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
	matTrans = XMMatrixTranslation(-7.f, 21.7f, -11.88f);

	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	
	XMStoreFloat4x4(&m_WorldMat, m_matWorld);
	m_pTransformCom->Set_WorldMatrix(m_WorldMat);


	m_vOriginPos = _float4(-7.f, 21.7f, -11.88f, 1.f);

	return S_OK;
}

void CCamera_Combat::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	//static float fov = 60.f;
	//static int iCurActorNum = -1;
	static bool m_LerpTrue = false;;
	//ImGui::InputFloat("Fov", &fov);



	if (ImGui::Button("CurActor && FOV"))
	{
		m_fCurShakeTime = 0.f;
		m_fShakeTime = 0.5f;
		m_fMagnitude = 0.1f;	
	}

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


	//CCombatController* pCombatCotroller = GET_INSTANCE(CCombatController);
	//
	//if (pCombatCotroller->Get_CurActor() != nullptr)
	//{
	//	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//	_vector vTargetPos = pCombatCotroller->Get_CurActor()->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);

	//	_float vDir = XMVectorGetX(XMVector3Length(vTargetPos - vPos));

	//	_bool b = false;

	//	
	//}

	//RELEASE_INSTANCE(CCombatController);
		
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
		_float ShakefX = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;
		_float ShakefY = CClient_Manager::GetRandomFloat(-1.f, 1.f) * m_fMagnitude;

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