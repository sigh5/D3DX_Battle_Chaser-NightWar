#include "stdafx.h"
#include "..\public\Camera_Static.h"

#include "GameInstance.h"
#include "PlayerController.h"
#include "Client_Manager.h"

CCamera_Static::CCamera_Static(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Static::CCamera_Static(const CCamera_Static & rhs)
	:CCamera(rhs)
{
}

HRESULT CCamera_Static::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Static::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Static_Camera");

	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof CameraDesc);

	if (nullptr != pArg)
		memcpy(&CameraDesc, pArg, sizeof(CAMERADESC));
	else
	{
		CameraDesc.vEye = _float4(0.f, 1.5f, -1.f, 1.f);
		CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		CameraDesc.vUp = _float4(0.f, 1.f, 0.f, 0.f);
		CameraDesc.fMouse_sensitivity = 0.1f;
		CameraDesc.TransformDesc.fSpeedPerSec = 20.f;
		CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	}

	if (FAILED(__super::Initialize(&CameraDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

#ifdef _DEBUG
	m_bCameraActive = false;
#else
	m_bCameraActive = true;
#endif

	return S_OK;
}

void CCamera_Static::Tick(_double TimeDelta)
{
	if (!m_bCameraActive)
		return;
	
	Last_Initialize();
	__super::Tick(TimeDelta);
	
	Update_CameraLookPos(TimeDelta);

	
}

void CCamera_Static::Late_Tick(_double TimeDelta)	
{
	if (!m_bCameraActive)
		return;

	__super::Late_Tick(TimeDelta);
}

HRESULT CCamera_Static::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	_bool m_bHavePosition = false;
	for (_uint i = 0; i < 20; ++i)
	{
		if (CClient_Manager::bIsCollPlayerTo3DUI[i] == true)
		{
			m_bHavePosition = true;
			break;
		}
	}

	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);
	m_pCurTaget = pPlayerController->Get_Captin();

	assert(m_pCurTaget != nullptr && " CCamera_Static::Last_Initialize");


	if( false==m_bHavePosition )
	{ 
		_vector vTargetPos = m_pCurTaget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		m_CameraEyeZ = fabs(XMVectorGetZ(vTargetPos) - XMVectorGetZ(vCamPos));
	}
	else
	{
		_float4x4 CamMatrix;
		XMStoreFloat4x4(&CamMatrix, CClient_Manager::m_StaticCameraMatrix);
		m_pTransformCom->Set_WorldMatrix(CamMatrix);
		m_CameraEyeZ = CClient_Manager::m_CameraEye_Z;
	}
	
	RELEASE_INSTANCE(CPlayerController);
	m_bLast_Initlize = true;
	
	return S_OK;
}

HRESULT CCamera_Static::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Static::Set_CameraActive(_bool bCameraActive)
{
	__super::Set_CameraActive(bCameraActive);
	
}

HRESULT CCamera_Static::SetUp_Components()
{
	return S_OK;
}

HRESULT CCamera_Static::Update_CameraLookPos(_double TimeDelta)
{
	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);
	
	CPlayer* pPlayer = pPlayerController->Get_Captin();

	RELEASE_INSTANCE(CPlayerController);
	
	assert(m_pCurTaget != nullptr && "CCamera_Static::Update_CameraLookPos");

	if (lstrcmp(pPlayer->Get_ObjectName(), m_pCurTaget->Get_ObjectName()))
	{
		m_bLerp = true;
	}

	if (!m_bLerp)
	{
		m_pCurTaget = pPlayer;
		NormalCameraActive();
	}
	else			// 0.2 초가 적당하다.
	{
		_vector vNewTargetPos = pPlayer->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);


		m_fLerpTimer += (_float)TimeDelta;
		if (m_fLerpTimer >= m_fLerpTime)
		{
			m_bLerp = false;
			m_fLerpTimer = 0.f;
			m_pCurTaget = pPlayer;
		}
		else
			Lerp_CameraActive(m_fLerpTimer, pPlayer);

	}
	

	return S_OK;
}

void CCamera_Static::NormalCameraActive()
{
	_vector vPlayerPos = m_pCurTaget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = vPlayerPos - CameraPos;

	_float4 PlayerVecotr;
	XMStoreFloat4(&PlayerVecotr, vPlayerPos);

	_float4 vNewCam;
	XMStoreFloat4(&vNewCam, CameraPos);

	vNewCam.x = XMVectorGetX(vPlayerPos);
	vNewCam.z = XMVectorGetZ(vPlayerPos) - m_CameraEyeZ;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vNewCam));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
}

void CCamera_Static::Lerp_CameraActive(_float LerpTime, CPlayer* pNewCaptin)
{
	_vector vPrevCapPos = m_pCurTaget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vNewCapPos = pNewCaptin->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_vector vPrevLook = vPrevCapPos - CameraPos;
	_vector vNewLook  = vNewCapPos  - CameraPos;

	_float Ratio = LerpTime / m_fLerpTime;
	_vector vCameraPos = XMVectorLerp(vPrevCapPos, vNewCapPos, Ratio);

	_float4 vNewCam;
	XMStoreFloat4(&vNewCam, CameraPos);
	vNewCam.x = XMVectorGetX(vCameraPos);
	vNewCam.z = XMVectorGetZ(vCameraPos) - m_CameraEyeZ;


	_vector vNewLerpLook = XMVectorLerp(vPrevLook, vNewLook, Ratio);
	
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vNewCam));
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vNewLerpLook));
}

CCamera_Static * CCamera_Static::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Static*		pInstance = new CCamera_Static(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Static");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Static::Clone(void * pArg)
{
	CCamera_Static*		pInstance = new CCamera_Static(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Static");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Static::Free()
{
	__super::Free();

}
