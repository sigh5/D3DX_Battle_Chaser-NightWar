#include "stdafx.h"
#include "..\public\Camera_Static.h"

#include "Player.h"
#include "GameInstance.h"
#include "Layer.h"

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

	if (!m_bCameraActive)
		return;
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	Update_CameraLookPos();

	Set_CaptinPlayer();

	RELEASE_INSTANCE(CGameInstance);


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

	CGameInstance* pGameInstace = GET_INSTANCE(CGameInstance);

	CGameObject *pTarget = nullptr;
	_int iIndex = 0;
	for (auto& Pair : pGameInstace->Get_Layer()[pGameInstace->GetCurLevelIdx()])
	{
		for (auto& obj : Pair.second->GetGameObjects())
		{
			if (nullptr != dynamic_cast<CPlayer*>(obj))
			{
				m_TargetList.push_back({ obj->Get_ObjectName(),obj });
				Safe_AddRef(m_TargetList.back().second);

				if(iIndex !=0)
					dynamic_cast<CPlayer*>(obj)->Set_FollowTarget(pTarget);
				
				pTarget = obj;
				++iIndex;
			}
		}
		
	}

	m_ChaseTargetTag = TEXT("Hero_Gully");	// 첫대장은 무조건 굴리
	m_bLast_Initlize = true;
	
	RELEASE_INSTANCE(CGameInstance);
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

	auto iter = find_if(m_TargetList.begin(), m_TargetList.end(), [&](auto MyPair)->bool
	{
		if (MyPair.first == m_ChaseTargetTag.c_str())
			return true;
		return false;
	});

	if (iter == m_TargetList.end())
		return;


	m_pCaptinPlayer = iter->second;
}

void CCamera_Static::Set_CaptinPlayer()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	

	if (pGameInstance->Key_Down(DIK_TAB))
	{		
		CGameObject* pPlayer = m_pCaptinPlayer;

		for (auto iter = m_TargetList.begin(); iter != m_TargetList.end();)
		{
			if (iter->first == m_pCaptinPlayer->Get_ObjectName())
			{
				static_cast<CPlayer*>(iter->second)->Set_ControlInput(false);
				iter = m_TargetList.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		m_pCaptinPlayer = m_TargetList.front().second;
		dynamic_cast<CPlayer*>(m_pCaptinPlayer)->Set_ControlInput(true);
		m_TargetList.push_back({pPlayer->Get_ObjectName() ,pPlayer });
	}








	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CCamera_Static::SetUp_Components()
{
	
	return S_OK;
}

HRESULT CCamera_Static::Update_CameraLookPos()
{
	if (m_pCaptinPlayer == nullptr)
		return S_OK;
	
	_vector vPlayerPos = m_pCaptinPlayer->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector CameraPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_vector vLook = vPlayerPos - CameraPos;

	_float4 PlayerVecotr;
	XMStoreFloat4(&PlayerVecotr, vPlayerPos);

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

	for (auto &MyPair : m_TargetList)
		Safe_Release(MyPair.second);

	

}
