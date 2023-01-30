#include "..\public\GameObject.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "Object_Manager.h"
#include "Level_Manager.h"

const wstring CGameObject::m_pTransformComTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_bClone(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs) 
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_bClone(true)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	GAMEOBJECTDESC  GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	if (pArg != nullptr)
		memcpy(&GameObjectDesc, pArg, sizeof(GAMEOBJECTDESC));
	

	
	if (FAILED(Add_Component(CGameInstance::Get_StaticLevelIndex(),
		CGameInstance::m_pPrototypeTransformTag, m_pTransformComTag, (CComponent**)&m_pTransformCom, &GameObjectDesc.TransformDesc)))
		return E_FAIL;
	

	return S_OK;
}



void CGameObject::Tick(_double TimeDelta)
{
}

void CGameObject::Late_Tick(_double TimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::Final_Update()
{
	for (const auto& com : m_Components)
	{
		com.second->Final_Update();
	}

}

void CGameObject::Set_parent(CGameObject* pGameObject)
{
	m_pParentObject = pGameObject;

	m_pTransformCom->Set_ParentTransform(m_pParentObject->m_pTransformCom);
	
}



CComponent * CGameObject::Get_Component(const wstring & ComponentTag)
{
	auto MyPair = find_if(m_Components.begin(), m_Components.end(), [&](auto Pair)->bool
	{
		if (Pair.first == ComponentTag)
			return true;
		return false;
	});

	if (MyPair == m_Components.end())
		return nullptr;


	return MyPair->second;
}

void CGameObject::Imgui_RenderComponentProperties()
{
	for (const auto& com : m_Components)
	{
		ImGui::Separator();
		char szName[128];
		CGameUtils::wc2c(com.first.c_str(), szName);

		ImGui::Text("%s", szName);
		com.second->Imgui_RenderProperty();

	}
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring & pPrototypeTag, const wstring & pComponentTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent*	pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	Safe_AddRef(pComponent);

	*ppOut = pComponent;

	Safe_Release(pGameInstance);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const wstring & pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), [&](auto Pair)->bool
	{
		if (pComponentTag == Pair.first)
			return true;
		return false;
	});

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

HRESULT CGameObject::Remove_component(const wstring & pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), [&](auto Pair)->bool
	{
		if (pComponentTag == Pair.first)
			return true;
		return false;
	});
	
	if (iter != m_Components.end())
	{
		Safe_Release(iter->second);
		m_Components.erase(iter);
	}

	return S_OK;
}

void CGameObject::Compute_CamDistance()
{
	_vector		vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_vector		vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPositon());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));

	RELEASE_INSTANCE(CPipeLine);
}

void CGameObject::Free()
{
//	Safe_Release(m_pParentObject);

	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

_bool CGameObject::To_Normal_Attack()
{
	return m_bState[m_Normal_Attack];
}

_bool CGameObject::To_Skill1_Attack()
{
	return m_bState[m_Skill1_Attack];
}

_bool CGameObject::To_Skill2_Attack()
{
	return  m_bState[m_Skill2_Attack];
}

_bool CGameObject::To_Uitimate()
{
	return  m_bState[m_Uitimate];
}

_bool CGameObject::To_Buff()
{
	return m_bState[m_Buff] ;
}

_bool CGameObject::To_WideAreaBuff()
{
	return  m_bState[m_WideAreaBuff];
}

_bool CGameObject::To_Use_Item()
{
	return  m_bState[m_Use_Item];
}

_bool CGameObject::To_Defence()
{
	return  m_bState[m_Defence];
}

_bool CGameObject::To_Light_Hit()
{
	return  m_bState[m_Light_Hit];
}

_bool CGameObject::To_Heavy_Hit()
{
	return  m_bState[m_Heavy_Hit];
}

_bool CGameObject::To_Flee()
{
	return  m_bState[m_Flee];
}

_bool CGameObject::To_Die()
{
	return  m_bState[m_Die];
}

_bool CGameObject::To_Viroty()
{
	return  m_bState[m_Viroty];
}
