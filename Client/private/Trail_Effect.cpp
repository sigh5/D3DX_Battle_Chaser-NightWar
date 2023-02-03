#include "stdafx.h"
#include "..\public\Trail_Effect.h"
#include "GameInstance.h"
#include "Traile_Effect_Child.h"

CTrail_Effect::CTrail_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice, pContext)
{
}

CTrail_Effect::CTrail_Effect(const CTrail_Effect & rhs)
	: CHitBoxObject(rhs)
{
}

void CTrail_Effect::Set_Desc(tag_Trail_Effect_DESC & Desc, _int iTextureNum ,_int iCurPointNum)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	memcpy(&m_Desc, &Desc, sizeof(m_Desc));
	m_iTextureNum = iTextureNum;
	m_CurPointNum = iCurPointNum;


	for (auto& pChild : m_TrailEffectChild)
		Safe_Release(pChild);
	m_TrailEffectChild.clear();


	m_bIsTrailStartCheck = true;
	m_iCenterCount = 0;
	m_iCUrCOunt = 0;
	m_iDevide_Lerp_Num = 0;

	_matrix  vMatirkx = m_Desc.pGameObject->Get_Transform()->Get_WorldMatrix();
	memcpy(&vLook, &vMatirkx.r[2], sizeof(_vector));
	vLook = XMVector3Normalize(vLook);
}

void CTrail_Effect::First_Edition()
{
	_float3  Temp, Sour, dest, Temp2;
	_float	fRadius;
	Temp = m_Desc.pColider->Get_Center();
	Sour = m_Desc.pColider->Get_Center();
	fRadius = m_Desc.pColider->Get_Radius();
	dest = m_Desc.pColider->Get_Center();
	Temp2 = m_Desc.pColider->Get_Center();
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(XMLoadFloat3(&Temp), 1.f));


	Temp.x -= fRadius;
	Temp.y += fRadius;
	Temp.z -= fRadius;

	Sour.x -= fRadius;
	Sour.y -= fRadius;
	Sour.z -= fRadius;

	dest.x += fRadius;;
	dest.y += fRadius;
	dest.z += fRadius;


	Temp2.x += fRadius;
	Temp2.y -= fRadius;
	Temp2.z += fRadius;

	m_Trail_Pos[0] = m_Trail_Pos[1];
	m_Trail_Pos[3] = m_Trail_Pos[2];

	m_Trail_Pos[1] = XMVectorSetW(XMLoadFloat3(&dest), 1.f);
	m_Trail_Pos[2] = XMVectorSetW(XMLoadFloat3(&Temp2), 1.f);


}

HRESULT CTrail_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail_Effect::Initialize(void * pArg)
{
	m_ObjectName = L"TestTrail";

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	return S_OK;
}

void CTrail_Effect::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


	_int		iIndex = 0;
	for (auto& pChild : m_TrailEffectChild)
	{
		if (pChild != nullptr)
		{
			_float Sour = _float(iIndex % m_CurPointNum);
			pChild->Set_UVXRatio((Sour / m_CurPointNum));
			pChild->Tick(TimeDelta);
			if (m_iTraillPos_Index >=2 && m_bIsTrailStartCheck)
				pChild->Set_RenderActive(true);
		
		}
		iIndex++;
	}


	if (ImGui::Button("Clear"))
	{
		for (auto& pChild : m_TrailEffectChild)
		{
			Safe_Release(pChild);
		}
		m_TrailEffectChild.clear();
		m_iCenterCount = 0;
		m_iCUrCOunt = 0;
		m_iDevide_Lerp_Num = 0;
	}

	if (m_bIsTrailStartCheck)
	{
		if (m_iTraillPos_Index >= m_CurPointNum)
		{
			m_bIsTrailStartCheck = false;
			m_iTraillPos_Index = 0;
			m_iDevide_Lerp_Num = 0;
			m_iCUrCOunt = 0;
		}

		if (m_iCenterCount < m_Desc.iMaxCenterCount)
		{
			m_Collider_pos[m_iCenterCount] = XMVectorSetW(XMLoadFloat3(&m_Desc.pColider->Get_Center()), 1.f);
		}

		if (m_iCenterCount >= 1)
		{
			if (m_iTraillPos_Index >= m_CurPointNum)
				return;

			_float3 vCurCenterPos;
			if (m_iCUrCOunt != 0)
			{
				m_Colider_Trail_Pos[m_iTraillPos_Index] = XMVectorCatmullRom(m_Collider_pos[m_iCUrCOunt - 1],
					m_Collider_pos[m_iCUrCOunt],
					m_Collider_pos[m_iCUrCOunt + 1], m_Collider_pos[m_iCUrCOunt + 1], _float(m_iDevide_Lerp_Num / m_Desc.iDevideFixNum));

				m_iDevide_Lerp_Num++;
				if (m_iDevide_Lerp_Num >= m_Desc.iDevideFixNum)
				{
					m_iDevide_Lerp_Num = 0;
					m_iCUrCOunt++;
				}	
			}
			else if (m_iCUrCOunt == 0)
			{
				m_Colider_Trail_Pos[0] = m_Collider_pos[0];
			}
			_vector vDest, vTemp;
			vDest = vTemp = m_Colider_Trail_Pos[m_iTraillPos_Index];

			XMStoreFloat3(&vCurCenterPos, m_Colider_Trail_Pos[m_iTraillPos_Index++]);

			_float	fRadius;
			fRadius = m_Desc.pColider->Get_Radius();
			vTemp += vLook *fRadius *m_Desc.fSize;
			vDest -= vLook *fRadius *m_Desc.fSize;

			m_Trail_Pos[0] = m_Trail_Pos[1];
			m_Trail_Pos[3] = m_Trail_Pos[2];
			m_Trail_Pos[1] = vTemp; 
			m_Trail_Pos[2] = vDest;	

			if (m_iCUrCOunt == 0)
				++m_iCUrCOunt;

			else
			{
				switch (m_Desc.eType)
				{
				case GARRISON_NORMAL:
					Garrison_Normal_TrailTick();
					break;
				case GARRISON_SKILL1:
					Garrison_Skill1_TrailTick();
					break;
		
				default:
					break;
				}


				
			}
		}
		++m_iCenterCount;
	}
	else
	{
		for (auto& pChild : m_TrailEffectChild)
		{
			if (pChild != nullptr)
				pChild->Set_RenderActive(false);
		}
		m_bDeleteCheck = true;
	}
}

void CTrail_Effect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (false == m_bDeleteCheck)
	{
		for (auto& pChild : m_TrailEffectChild)
		{
			if (pChild != nullptr)
				pChild->Late_Tick(TimeDelta);
		}
	}
	else
	{
		for (auto& pChild : m_TrailEffectChild)
			Safe_Release(pChild);
		m_TrailEffectChild.clear();

		m_bDeleteCheck = false;

	}

}

HRESULT CTrail_Effect::Render()
{
	return S_OK;
}

HRESULT CTrail_Effect::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;


	m_bLast_Initlize = true;
	return S_OK;
}

void CTrail_Effect::Garrison_Normal_TrailTick()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Effect_Child"), &m_HitBoxDesc);
	m_TrailEffectChild.push_back(static_cast<CTraile_Effect_Child*>(pGameObject));
	CTraile_Effect_Child::Trail_Child_Pos_Desc ChildDesc;
	ZeroMemory(&ChildDesc, sizeof(ChildDesc));
	ChildDesc.m_Trail_Pos[0] = m_Trail_Pos[0];
	ChildDesc.m_Trail_Pos[3] = m_Trail_Pos[3];
	ChildDesc.m_Trail_Pos[1] = m_Trail_Pos[1];
	ChildDesc.m_Trail_Pos[2] = m_Trail_Pos[2];
	ChildDesc.iTextureNum = m_iTextureNum;
	ChildDesc.fFixRatio = 1.f / m_CurPointNum;
	ChildDesc.ColorTexture0Num = 5;
	ChildDesc.ColorTexture1Num = 2;
	static_cast<CTraile_Effect_Child*>(pGameObject)->Set_ChildDesc(ChildDesc);
	RELEASE_INSTANCE(CGameInstance);
}

void CTrail_Effect::Garrison_Skill1_TrailTick()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Trail_Effect_Child"), &m_HitBoxDesc);
	m_TrailEffectChild.push_back(static_cast<CTraile_Effect_Child*>(pGameObject));
	CTraile_Effect_Child::Trail_Child_Pos_Desc ChildDesc;
	ZeroMemory(&ChildDesc, sizeof(ChildDesc));
	ChildDesc.m_Trail_Pos[0] = m_Trail_Pos[0];
	ChildDesc.m_Trail_Pos[3] = m_Trail_Pos[3];
	ChildDesc.m_Trail_Pos[1] = m_Trail_Pos[1];
	ChildDesc.m_Trail_Pos[2] = m_Trail_Pos[2];
	ChildDesc.iTextureNum = m_iTextureNum;
	ChildDesc.fFixRatio = 1.f / m_CurPointNum;
	ChildDesc.ColorTexture0Num = 5;
	ChildDesc.ColorTexture1Num = 2;
	static_cast<CTraile_Effect_Child*>(pGameObject)->Set_ChildDesc(ChildDesc);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CTrail_Effect::SetUp_Components()
{
	return S_OK;
}

HRESULT CTrail_Effect::SetUp_ShaderResources()
{
	return S_OK;
}

CTrail_Effect * CTrail_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrail_Effect*		pInstance = new CTrail_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCam_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrail_Effect::Clone(void * pArg)
{
	CTrail_Effect*		pInstance = new CTrail_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCam_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrail_Effect::Free()
{
	__super::Free();

	for (auto& pChild : m_TrailEffectChild)
		Safe_Release(pChild);
	m_TrailEffectChild.clear();

}
