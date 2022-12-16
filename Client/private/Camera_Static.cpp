#include "stdafx.h"
#include "..\public\Camera_Static.h"

#include "GameInstance.h"
#include "PlayerController.h"

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
		CameraDesc.vEye = _float4(0.f, 20.f, -10.f, 1.f);
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

	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);
	m_pCurTaget = pPlayerController->Get_Captin();

	assert(m_pCurTaget != nullptr && " CCamera_Static::Last_Initialize");

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
	HRESULT hr = 0;
	CPlayer* pPlayer = pPlayerController->Get_Captin();

	RELEASE_INSTANCE(CPlayerController);
	
	hr = FAILED(m_pCurTaget == nullptr);
	if (hr == E_FAIL)
		assert("Error");

	/*if (lstrcmp(pPlayer->Get_ObjectName(), m_pCurTaget->Get_ObjectName()))
	{
		m_bLerp = true;
	}

	if (!m_bLerp)
	{*/
	//	m_pCurTaget = pPlayer;
		NormalCameraActive();
	//}
	//else			// 0.2 초가 적당하다.
	//{
	//	m_fLerpTimer += (_float)(TimeDelta* 1.f);
	//	if (m_fLerpTimer <= 2.f)
	//	{
	//		_double Ratio = m_fLerpTimer;
	//		Lerp_CameraActive((_float)Ratio, pPlayer);
	//	}
	//	else
	//	{
	//		m_bLerp = false;
	//		m_fLerpTimer = 0.f;
	//		m_pCurTaget = pPlayer;
	//	}

	//}
	

	return S_OK;
}

void CCamera_Static::NormalCameraActive()
{
	_vector vPlayerPos = m_pCurTaget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = vPlayerPos - CameraPos;

	_float4 PlayerVecotr;
	XMStoreFloat4(&PlayerVecotr, vPlayerPos);


	PlayerVecotr.x += m_CameraDistanceX;
	PlayerVecotr.y += m_CameraDistanceY;
	PlayerVecotr.z -= m_CameraDistanceZ;


	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&PlayerVecotr));

}

void CCamera_Static::Lerp_CameraActive(_float Ratio, CPlayer* pCurCaptin)
{
	_vector vPrevCapPos = m_pCurTaget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vCurCapPos = pCurCaptin->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_vector vLerpVector = XMVectorLerp(vCurCapPos,vPrevCapPos, Ratio);
	_vector vLook = vLerpVector - CameraPos;

	_float4 PlayerVecotr;
	XMStoreFloat4(&PlayerVecotr, vLerpVector);

	PlayerVecotr.x += m_CameraDistanceX;
	PlayerVecotr.y += m_CameraDistanceY;
	PlayerVecotr.z -= m_CameraDistanceZ;


	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&PlayerVecotr));

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
