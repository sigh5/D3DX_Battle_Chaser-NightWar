#include "..\public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(TimeDelta);
	}
		
}

void CLayer::Late_Tick(_double TimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(TimeDelta);
	}
}

void CLayer::Final_Update()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Final_Update();
	}
}

void CLayer::Delete_GameObject(CGameObject *& pObject)
{
	if (pObject != nullptr)
	{
		Safe_Release(pObject);
		m_GameObjects.remove(pObject);
		bool b = false;
	}
}

void CLayer::CopyData(list<class CGameObject*>& Temp)
{
	for (auto iter= Temp.begin(); iter != Temp.end();)
	{
		m_GameObjects.push_back(*iter);
		iter = Temp.erase(iter);
	}
	Temp.clear();
}

void CLayer::Change_Level(_uint iLevleIdx)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Change_Level_Data(iLevleIdx);
	}

}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
