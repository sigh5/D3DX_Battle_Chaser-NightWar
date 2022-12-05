#include "stdafx.h"
#include "..\public\ToolManager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "Canvas.h"
#include "Camera.h"
#include "Environment_Object.h"
#include "Terrain.h"
#include "NoneAnim_BG.h"

IMPLEMENT_SINGLETON(CToolManager)

CToolManager::CToolManager()
{
	m_LayerName.push_back(LAYER_BACKGROUND);
	m_LayerName.push_back(LAYER_ENVIRONMENT);
	m_LayerName.push_back(LAYER_PLAYER);
	m_LayerName.push_back(LAYER_Monster);
	m_LayerName.push_back(LAYER_UI);
}

void CToolManager::Imgui_SelectParentViewer()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	ImGui::Begin("Select_ParentObject");
	ImGui::NewLine();
	bool bFound = false;

	Imgui_Select_ParentCanvas();
	Imgui_Select_ParentTerrain();
	
	Imgui_Select_LayerType();
	Imgui_Select_ProtoType();
	Imgui_Select_Texture();
	Imgui_Select_Model();

	Imgui_CreateObject();
	
	Imgui_Change_model();
	Imgui_Change_Texture();

	ImGui::End();
	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Select_ParentCanvas()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	map<const wstring, class CLayer*>* pLayer = pGameInstance->Get_Layer();
	map<const wstring, class CLayer*> targetLevel = pLayer[pGameInstance->GetCurLevelIdx()];
	if (ImGui::CollapsingHeader("Select_ObjectTab", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNode("CanvasGameObjects"))  // for object loop listbox
		{
			for (auto& Pair : targetLevel)
			{
				if (ImGui::BeginListBox("##"))
				{
					char szobjectTag[128];

					for (auto& obj : Pair.second->GetGameObjects())
					{
						if (dynamic_cast<CCanvas*>(obj) == nullptr)
							continue;

						if (obj != nullptr)
							wc2c((obj->Get_ObjectName()), szobjectTag);

						if (ImGui::Selectable(szobjectTag))
						{
							if (dynamic_cast<CUI*>(pGameInstance->Get_SelectObject()) == nullptr)
								return;

							dynamic_cast<CCanvas*>(obj)->Add_ChildUI(dynamic_cast<CUI*>(pGameInstance->Get_SelectObject()));
						}
					}
					ImGui::EndListBox();
				}
			}
			ImGui::TreePop();
		}
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Select_ParentTerrain()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	map<const wstring, class CLayer*>* pLayer = pGameInstance->Get_Layer();
	map<const wstring, class CLayer*> targetLevel = pLayer[pGameInstance->GetCurLevelIdx()];

	if (ImGui::TreeNode("SelectGround"))
	{
		for (auto& Pair : targetLevel)
		{
			if (ImGui::BeginListBox("##"))
			{
				char szobjectTag[128];

				for (auto& obj : Pair.second->GetGameObjects())
				{
					if (dynamic_cast<CUI*>(obj) != nullptr || dynamic_cast<CCamera*>(obj) != nullptr)
						continue;

					if (obj != nullptr)
						wc2c((obj->Get_ObjectName()), szobjectTag);

					if (ImGui::Selectable(szobjectTag))
					{
						m_pSelectTerrain = obj;
					}
				}
				ImGui::EndListBox();
			}
		}

		ImGui::TreePop();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_CreateObject()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (ImGui::CollapsingHeader("Create_Object", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::InputText("Layer_Name ", m_szLayerName, MAX_PATH);
		ImGui::InputText("Proto_Name ", m_szProtoName, MAX_PATH);
		ImGui::InputText("Texture_Name ", m_szTexturName, MAX_PATH);
		ImGui::InputText("Model_Name ", m_szModelName, MAX_PATH);
		ImGui::InputText("ObjectName ", m_szObjectName, MAX_PATH);

		ImGui::InputInt("ShaderPassIndex ", &m_iShaderPass);

		Imgui_Create_UI();
		Imgui_Create_Object();

		Imgui_Crate_FilterMap();

	}
	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Create_UI()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (ImGui::Button("CreateUI"))
	{
		CUI::UIDESC m_Desc;
		_tchar* szLayerName = new _tchar[MAX_PATH];
		_tchar* szProtoName = new _tchar[MAX_PATH];
		_tchar* szTextureName = new _tchar[MAX_PATH];
		_tchar* szObjectName = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szLayerName, sizeof(char[256]), szLayerName, sizeof(_tchar[256]));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szProtoName, sizeof(char[256]), szProtoName, sizeof(_tchar[256]));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szTexturName, sizeof(char[256]), szTextureName, sizeof(_tchar[256]));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szObjectName, sizeof(char[256]), szObjectName, sizeof(_tchar[256]));
		m_Desc.m_pTextureTag = szTextureName;

		CGameObject* pGameObject = nullptr;
		pGameInstance->Clone_GameObject_UseImgui(pGameInstance->GetCurLevelIdx(), szLayerName, szProtoName, &pGameObject, &m_Desc);

		if (pGameObject == nullptr)
			return;

		pGameObject->Set_ProtoName(szProtoName);
		pGameObject->Set_ObjectName(szObjectName);

		m_vecNameArray.push_back(szObjectName);
		m_vecNameArray.push_back(szLayerName);
		m_vecNameArray.push_back(szProtoName);
		m_vecNameArray.push_back(szTextureName);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Create_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	ImGui::SameLine();
	if (ImGui::Button("CreateEnviromentObj"))
	{
 		_tchar* szLayerName = new _tchar[MAX_PATH];
		_tchar* szProtoName = new _tchar[MAX_PATH];
		_tchar* szTextureName = new _tchar[MAX_PATH];
		_tchar* szModelName = new _tchar[MAX_PATH];
		_tchar* szObjectName = new _tchar[MAX_PATH];

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szLayerName, sizeof(char[256]), szLayerName, sizeof(_tchar[256]));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szProtoName, sizeof(char[256]), szProtoName, sizeof(_tchar[256]));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szTexturName, sizeof(char[256]), szTextureName, sizeof(_tchar[256]));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szObjectName, sizeof(char[256]), szObjectName, sizeof(_tchar[256]));
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szModelName, sizeof(char[256]), szModelName, sizeof(_tchar[256]));

		if (m_pSelectTerrain != nullptr)
		{
			Create_OBJECTDESC Temp;
			ZeroMemory(&Temp, sizeof(Create_OBJECTDESC));

			Temp.szLayerName = szLayerName;
			Temp.szProtoName = szProtoName;
			Temp.szTextureName = szTextureName;
			Temp.szModelName = szModelName;
			Temp.szObjectName = szObjectName;
			Temp.iShaderPass = m_iShaderPass;
			dynamic_cast<CTerrain*>(m_pSelectTerrain)->Set_MapObject(Temp);
		}	
		else
		{
			CGameObject* pGameObject = nullptr;
			CEnvironment_Object::ENVIRONMENTDESC Desc;
			ZeroMemory(&Desc, sizeof(CEnvironment_Object::ENVIRONMENTDESC));
			Desc.m_pModelTag = szModelName;
			Desc.m_pTextureTag = szTextureName;
			Desc.m_iShaderPass = m_iShaderPass;

			pGameInstance->Clone_GameObject_UseImgui(pGameInstance->GetCurLevelIdx(), szLayerName, szProtoName, &pGameObject, &Desc);
			if (nullptr == pGameObject)
				return;
			pGameObject->Set_ObjectName(szObjectName);
			pGameObject->Set_ProtoName(szProtoName);
		}
		
			m_vecNameArray.push_back(szLayerName);
			m_vecNameArray.push_back(szProtoName);
			m_vecNameArray.push_back(szTextureName);
			m_vecNameArray.push_back(szModelName);
			m_vecNameArray.push_back(szObjectName);
			

	}
	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Select_LayerType()
{
	if (ImGui::TreeNode("LayerType"))  // for object loop listbox
	{
		for (auto& iter : m_LayerName)
		{
			if (ImGui::BeginListBox("##"))
			{
				char szobjectTag[MAX_PATH];
				if (lstrcmp(iter, TEXT("")))
					wc2c(iter, szobjectTag);

				if (ImGui::Selectable(szobjectTag))
				{
					strcpy_s(m_szLayerName, szobjectTag);
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}
}

void CToolManager::Imgui_Select_ProtoType()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	map<const wstring, class CGameObject*>* pProtoType = pGameInstance->Get_ProtoTypes();

	if (ImGui::TreeNode("ProtoTypes"))  // for object loop listbox
	{
		for (auto& Pair : *pProtoType)
		{
			if (ImGui::BeginListBox("##"))
			{
				char szobjectTag[MAX_PATH];
				if (Pair.second != nullptr)
					wc2c(Pair.first.c_str(), szobjectTag);

				if (ImGui::Selectable(szobjectTag))
				{
					strcpy_s(m_szProtoName, szobjectTag);
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Select_Texture()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Imgui_ComponentViewer(pGameInstance->GetCurLevelIdx(), TextureName, COMPONENT_TYPE::TYPE_TEXTURE);
	wc2c(TextureName.c_str(), m_szTexturName);

	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Select_Model()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Imgui_ComponentViewer(pGameInstance->GetCurLevelIdx(), ModelName, COMPONENT_TYPE::TYPE_MODEL);
	wc2c(ModelName.c_str(), m_szModelName);

	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Change_model()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (ImGui::Button("Change_Model"))
	{
		_tchar* szModelName = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szModelName, sizeof(char[256]), szModelName, sizeof(_tchar[256]));
		m_vecNameArray.push_back(szModelName);

		CNoneAnim_BG* pSelectNonAnim = dynamic_cast<CNoneAnim_BG*>(pGameInstance->Get_SelectObject());

		if (nullptr != pSelectNonAnim)
			pSelectNonAnim->Change_Model( pGameInstance->GetCurLevelIdx() ,szModelName);
	}

	RELEASE_INSTANCE(CGameInstance);

}

void CToolManager::Imgui_Change_Texture()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	ImGui::SameLine();
	if (ImGui::Button("Change_Texture"))
	{
		_tchar* szTextureName = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szTexturName, sizeof(char[256]), szTextureName, sizeof(_tchar[256]));
		m_vecNameArray.push_back(szTextureName);

		CUI* pSelectNonAnim = dynamic_cast<CUI*>(pGameInstance->Get_SelectObject());

		if (nullptr != pSelectNonAnim)
			pSelectNonAnim->Change_Texture(pGameInstance->GetCurLevelIdx(), szTextureName);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CToolManager::Imgui_Crate_FilterMap()
{
	ImGui::RadioButton("NoUse_FilterMap", &m_iRadioButton, 0);
	ImGui::RadioButton("Create_FilterMap", &m_iRadioButton, 1);
}

void CToolManager::Free()
{
	for (auto& iter : m_vecNameArray)
		Safe_Delete_Array(iter);

}


