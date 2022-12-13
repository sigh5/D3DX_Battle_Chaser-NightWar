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
	Last_Initialize();

	/*나중에 삭제바람*/
	if(m_pPlayerController->CombatScene())
		m_pPlayerController->CurrentTurn_AnimControl();
	
	if (!m_bCameraActive)
		return;

	if (!m_pPlayerController->CombatScene())
	{
		m_pPlayerController->Set_CaptinPlayer();
		Update_CameraLookPos();
		m_pPlayerController->SyncAninmation();
	}
	else
	{
		m_pPlayerController->CurrentTurn_AnimControl();
	}

	__super::Tick(TimeDelta);
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

	m_pPlayerController = GET_INSTANCE(CPlayerController);
	m_pPlayerController->Initialize();

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

HRESULT CCamera_Static::Update_CameraLookPos()
{
	if (m_pPlayerController->Get_Captin() == nullptr)
		return S_OK;
	
	_vector vPlayerPos = m_pPlayerController->Get_Captin()->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_vector vLook = vPlayerPos - CameraPos;

	_float4 PlayerVecotr;
	XMStoreFloat4(&PlayerVecotr, vPlayerPos);

	ImGui::InputFloat("m_CameraDistanceX", &m_CameraDistanceX);
	ImGui::InputFloat("m_CameraDistanceY", &m_CameraDistanceY);
	ImGui::InputFloat("m_CameraDistanceZ", &m_CameraDistanceZ);

	PlayerVecotr.x += m_CameraDistanceX;
	PlayerVecotr.y += m_CameraDistanceY;
	PlayerVecotr.z -= m_CameraDistanceZ;


	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&PlayerVecotr));


	return S_OK;
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

	Safe_Release(m_pPlayerController);

	

}
