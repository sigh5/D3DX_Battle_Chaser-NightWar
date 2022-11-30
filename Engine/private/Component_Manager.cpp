#include "..\public\Component_Manager.h"
#include "Component.h"
#include "GameUtils.h"


IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pPrototypes || 
		iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void * pArg)
{
	CComponent*		pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent*		pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;	
}

void CComponent_Manager::Imgui_TextureViewer(_uint iLevel  ,OUT wstring& TextureTag)
{
	const PROTOTYPES& ProtoType = m_pPrototypes[iLevel];

	if (ImGui::TreeNode("Textures"))  // for object loop listbox
	{
		for (auto& Pair : ProtoType)
		{
			 if( dynamic_cast<CTexture*>(Pair.second) == nullptr)
				 continue;

			if (ImGui::BeginListBox("##"))
			{
				char szobjectTag[MAX_PATH];

				if (Pair.second != nullptr )
					CGameUtils::wc2c(Pair.first.c_str(), szobjectTag);

				if (ImGui::Selectable(szobjectTag))
				{
					TextureTag = Pair.first;
				}
				ImGui::EndListBox();
			}
			
		}
		ImGui::TreePop();
	}


}

void CComponent_Manager::Imgui_ModelViewer(_uint iLevel, OUT wstring & Model_NoAnimTag)
{
	const PROTOTYPES& ProtoType = m_pPrototypes[iLevel];

	if (ImGui::TreeNode("Models_NonAnim"))  // for object loop listbox
	{
		for (auto& Pair : ProtoType)
		{
			if (dynamic_cast<CModel*>(Pair.second) == nullptr)
				continue;

			if (ImGui::BeginListBox("##"))
			{
				char szobjectTag[MAX_PATH];

				if (Pair.second != nullptr)
					CGameUtils::wc2c(Pair.first.c_str(), szobjectTag);

				if (ImGui::Selectable(szobjectTag))
				{
					Model_NoAnimTag = Pair.first;
				}
				ImGui::EndListBox();
			}
			
		}
		ImGui::TreePop();
	}

}



CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), [&](auto Pair)->bool
	{
		if (Pair.first == pPrototypeTag)
			return true;
		return false;
	});
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);

}
