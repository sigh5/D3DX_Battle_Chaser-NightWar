#include "stdafx.h"
#include "..\public\Camera_Combat.h"
#include "GameInstance.h"

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
		CameraDesc.TransformDesc.fSpeedPerSec = 20.f;
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

	_float4x4	WorldMat;	
	XMStoreFloat4x4(&WorldMat, m_matWorld);
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	return S_OK;
}

void CCamera_Combat::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
}

void CCamera_Combat::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CCamera_Combat::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;


	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 47.f);

	m_bLast_Initlize = true;
	return S_OK;
}

HRESULT CCamera_Combat::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Combat::Camera_Shaking()
{
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