#include "..\public\Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Canvas.h"
#include "Level_Manager.h"
IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])	
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

HRESULT CObject_Manager::Loading_Objects()
{
	// 로딩 레벨 삭제
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	for (auto &Pair : m_pLayers[pGameInstance->Get_StaticLevelIndex() - 1])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[pGameInstance->Get_StaticLevelIndex() - 1].clear();
	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring& pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(pPrototypeTag, pPrototype);
	
	return S_OK;
}

HRESULT CObject_Manager::Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void * pArg)
{
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*		pGameObject = pPrototype->Clone(pArg);

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

CGameObject * CObject_Manager::Get_GameObject(_uint iLevelIndex, const wstring & pLayerTag, const wstring & pObjectNameTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	for (auto& iter : pLayer->GetGameObjects())
	{
		if (iter != nullptr && !lstrcmpW(iter->Get_ObjectName(), pObjectNameTag.c_str()))
			return iter;
	}

	return nullptr;
}

void CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if(nullptr != Pair.second)
				Pair.second->Tick(TimeDelta);			
		}
	}	
}

void CObject_Manager::Late_Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Tick(TimeDelta);
		}
	}
}

void CObject_Manager::Final_Update()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Final_Update();
		}
	}
}

void CObject_Manager::Imgui_SelectParentViewer(_uint iLevel, OUT CGameObject *& pSelectedObject)
{
	const LAYERS& targetLevel = m_pLayers[iLevel];
	ImGui::Begin("Select_ParentObject");
	ImGui::NewLine();
	bool bFound = false;
	if (ImGui::CollapsingHeader("Select_ObjectTab", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNode("GameObjects"))  // for object loop listbox
		{
			for (auto& Pair : targetLevel)
			{
				if (ImGui::BeginListBox("##"))
				{
					char szobjectTag[128];
				
					for (auto& obj : Pair.second->GetGameObjects())
					{
						if (obj != nullptr)
							CGameUtils::wc2c((obj->Get_ObjectName()), szobjectTag);
						
						const bool bSelected = pSelectedObject != obj;
						if (ImGui::Selectable(szobjectTag, bSelected))
						{
							if (dynamic_cast<CCanvas*>(obj) == nullptr || dynamic_cast<CUI*>(pSelectedObject) == nullptr)
								return;

							dynamic_cast<CCanvas*>(obj)->Add_ChildUI(dynamic_cast<CUI*>(pSelectedObject));

						}
					}
					ImGui::EndListBox();
				}
			}
			ImGui::TreePop();
		}
	}
	
	if (pSelectedObject != nullptr  &&pSelectedObject->Get_parentName() != nullptr)
		ImGui::Text("%s", typeid(*pSelectedObject->Get_parentName()).name());

	if (ImGui::CollapsingHeader("Create_Object", ImGuiTreeNodeFlags_DefaultOpen))
	{
		CLevel_Manager* pLevelManager = GET_INSTANCE(CLevel_Manager);

		ImGui::InputText("Layer_Name ", m_szLayerName, MAX_PATH);
		ImGui::InputText("Proto_Name ", m_szProtoName, MAX_PATH);
		ImGui::InputText("Texture_Name ", m_szTexturName, MAX_PATH);
		ImGui::InputText("ObjectName ", m_szObjectName, MAX_PATH);


		ImGui::InputFloat("Speed", &m_fTransformSpeed);
		ImGui::InputFloat("Rotation", &m_fTransformRotation);

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
			Clone_GameObject_UseImgui(pLevelManager->GetCurLevelIdx(), szLayerName, szProtoName, &pGameObject, &m_Desc);
		
			if (pGameObject == nullptr)
				return ;

			pGameObject->Set_ProtoName(szProtoName);
			pGameObject->Set_ObjectName(szObjectName);

			m_vecNameArray.push_back(szObjectName);
			m_vecNameArray.push_back(szLayerName);
			m_vecNameArray.push_back(szProtoName);
			m_vecNameArray.push_back(szTextureName);
		}

		if (ImGui::Button("CreateMapObject"))
		{
			
			CGameObject::GAMEOBJECTDESC	m_Desc;
			m_Desc.TransformDesc.fSpeedPerSec = m_fTransformSpeed;
			m_Desc.TransformDesc.fRotationPerSec = m_fTransformRotation;
			
			_tchar* szLayerName = new _tchar[MAX_PATH];
			_tchar* szProtoName = new _tchar[MAX_PATH];
			_tchar* szTextureName = new _tchar[MAX_PATH];
			_tchar* szObjectName = new _tchar[MAX_PATH];

			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szLayerName, sizeof(char[256]), szLayerName, sizeof(_tchar[256]));
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szProtoName, sizeof(char[256]), szProtoName, sizeof(_tchar[256]));
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szTexturName, sizeof(char[256]), szTextureName, sizeof(_tchar[256]));
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szObjectName, sizeof(char[256]), szObjectName, sizeof(_tchar[256]));
			
			//m_Desc.ObjectName = szObjectName;

			CGameObject* pGameObject = nullptr;

			Clone_GameObject_UseImgui(pLevelManager->GetCurLevelIdx(), szLayerName, szProtoName, &pGameObject,&m_Desc);

			if (nullptr == pGameObject)
				MSG_BOX("Error Imgui CloneGameObject");

			pGameObject->Set_ObjectName(szObjectName);


			m_vecNameArray.push_back(szLayerName);
			m_vecNameArray.push_back(szProtoName);
			m_vecNameArray.push_back(szTextureName);
			m_vecNameArray.push_back(szObjectName);
		}


		RELEASE_INSTANCE(CLevel_Manager);
	}

	ImGui::End();
}

HRESULT CObject_Manager::Clone_GameObject_UseImgui(_uint iLevelIndex, const wstring & pLayerTag, const wstring & pPrototypeTag, OUT CGameObject** ppGameObject, void * pArg)
{
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*		pGameObject = pPrototype->Clone(pArg);

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);


	*ppGameObject = pGameObject;

	return S_OK;
}

void CObject_Manager::Imgui_PickingSelcetObject(_uint iLevel, OUT CGameObject *& pSelectedObject)
{
	// TreeNode 켜져있을때만가능
	const LAYERS& targetLevel = m_pLayers[iLevel];

	if (ImGui::IsMouseClicked(0))
	{
		for (auto& Pair : targetLevel)
		{
			for (auto& obj : Pair.second->GetGameObjects())
			{
				if (obj->Piciking_GameObject())
				{
					pSelectedObject = obj;
				}

			}
		}
	}
	
}

void CObject_Manager::Imgui_Save()
{
	if (ImGui::Button("UISave"))
	{
		_tchar* szName = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szSaveDataname, sizeof(char[256]), szName, sizeof(_tchar[256]));

		_tchar szPath[MAX_PATH] = TEXT("../../Data/");

		lstrcat(szPath, szName);
		lstrcpy(szName, szPath);
		lstrcat(szName, TEXT(".dat"));

		HANDLE      hFile = CreateFile(szName,
			GENERIC_WRITE,
			NULL,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			return;
		}

		DWORD   dwByte = 0;

		CLevel_Manager* pLevelManager = GET_INSTANCE(CLevel_Manager);
		_tchar *LayerTag = new _tchar[MAX_PATH];
		_tchar *ProtoName = new _tchar[MAX_PATH];
		_tchar *ParentName = new _tchar[MAX_PATH];
		_tchar *TextureName = new _tchar[MAX_PATH];
		_tchar *ObjectName = new _tchar[MAX_PATH];

		for (_uint i = 0; i < m_iNumLevels; ++i)
		{
			for (auto& Pair : m_pLayers[i])
			{
				if (Pair.first == TEXT("Layer_UI"))
				{
					lstrcpy(LayerTag, Pair.first.c_str());

					for (auto& pObject : Pair.second->GetGameObjects())
					{
						_float4x4 Worldmatrix = _float4x4();
						lstrcpy(ProtoName, pObject->Get_ProtoName());

						CGameObject* pParent = pObject->Get_parentName();
						if (nullptr != pParent)
							lstrcpy(ParentName, pParent->Get_ObjectName());

						else
							lstrcpy(ParentName, TEXT("Nullptr"));

						lstrcpy(TextureName, dynamic_cast<CUI*>(pObject)->Get_TextureTag());
						lstrcpy(ObjectName,	 pObject->Get_ObjectName());

						XMStoreFloat4x4(&Worldmatrix, pObject->Get_Transform()->Get_LocalMatrix());
						
						WriteFile(hFile, &Worldmatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
						WriteFile(hFile, LayerTag, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, ProtoName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, ParentName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, TextureName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, ObjectName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
					
					}
				}
			}
		}

		CloseHandle(hFile);
		m_vecNameArray.push_back(ObjectName);
		m_vecNameArray.push_back(szName);
		m_vecNameArray.push_back(LayerTag);
		m_vecNameArray.push_back(ProtoName);
		m_vecNameArray.push_back(ParentName);
		m_vecNameArray.push_back(TextureName);
	
		MSG_BOX("Save_Complete");
		RELEASE_INSTANCE(CLevel_Manager);

	}
	//ImGui::SameLine();
}

void CObject_Manager::Imgui_Load()
{

	if (ImGui::Button("UILoad"))
	{
		CLevel_Manager* pLevelManager = GET_INSTANCE(CLevel_Manager);
		_tchar* szName = new _tchar[256];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szSaveDataname, sizeof(char[256]), szName, sizeof(_tchar[256]));
		m_vecNameArray.push_back(szName);

		_tchar szPath[MAX_PATH] = TEXT("../../Data/");
		lstrcat(szPath, szName);
		lstrcpy(szName, szPath);
		lstrcat(szName, TEXT(".dat"));

		HANDLE      hFile = CreateFile(szName,
			GENERIC_READ,
			NULL,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			return;
		}

		DWORD   dwByte = 0;

		_uint iObjectNumber = 0;

		
		while (true)
		{	
			_float4x4 Worldmatrix = _float4x4();
			_tchar* LayerTag = new _tchar[MAX_PATH];
			_tchar* ProtoName = new _tchar[MAX_PATH];
			_tchar* ParentName = new _tchar[MAX_PATH];
			_tchar* TextureName = new _tchar[MAX_PATH];
			_tchar *ObjectName = new _tchar[MAX_PATH];

			m_vecNameArray.push_back(ObjectName);
			m_vecNameArray.push_back(LayerTag);
			m_vecNameArray.push_back(ProtoName);
			m_vecNameArray.push_back(ParentName);
			m_vecNameArray.push_back(TextureName);

			ReadFile(hFile, &Worldmatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
			ReadFile(hFile, LayerTag, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
			ReadFile(hFile, ProtoName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
			ReadFile(hFile, ParentName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
			ReadFile(hFile, TextureName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
			ReadFile(hFile, ObjectName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		
			if (0 == dwByte)
				break;
		
			if (lstrcmp(ParentName, TEXT("Nullptr")))
			{
				CUI::UIDESC UIDesc;
				ZeroMemory(&UIDesc, sizeof(UIDesc));
				UIDesc.m_pTextureTag = TextureName;
				CGameObject* pGameObject = nullptr;
				Clone_GameObject_UseImgui(pLevelManager->GetCurLevelIdx(), LayerTag, ProtoName, &pGameObject, &UIDesc);
				(pGameObject)->Set_ProtoName(ProtoName);
				(pGameObject)->Set_ObjectName(ObjectName);
				(pGameObject)->Get_Transform()->Set_WorldMatrix(Worldmatrix);
				pGameObject->Set_parentName(ParentName);
			
			}
			else
			{
				CGameObject* pGameObject = nullptr;
				Clone_GameObject_UseImgui(pLevelManager->GetCurLevelIdx(), LayerTag, ProtoName, &pGameObject);
				pGameObject->Set_ProtoName(ProtoName);
				pGameObject->Set_ObjectName(ObjectName);
				pGameObject->Get_Transform()->Set_WorldMatrix(Worldmatrix);
			
			
			}
		}
	
		 RELEASE_INSTANCE(CLevel_Manager);

	}
}

void CObject_Manager::Imgui_ProtoViewer(const _tchar*& szSelectedProto)
{
}

void CObject_Manager::Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject)
{
	bool bFound = false;
	if (m_iNumLevels <= iLevel)
		return;

	const LAYERS& targetLevel = m_pLayers[iLevel];

	if (ImGui::TreeNode("ObjectViewer"))
	{
		for (auto& Pair : targetLevel) // for layer loop
		{
			char szLayerTag[128];
			char szobjectTag[128];
			CGameUtils::wc2c(Pair.first.c_str(), szLayerTag);
			if (ImGui::TreeNode(szLayerTag))  // for object loop listbox
			{
				if (ImGui::BeginListBox("##"))
				{
					for (auto& obj : Pair.second->GetGameObjects())
					{
						if(obj !=nullptr)
							CGameUtils::wc2c((obj->Get_ObjectName()), szobjectTag);

						const bool bSelected = pSelectedObject == obj;
						if (bSelected) 
						{
							ImGui::SetItemDefaultFocus();
							bFound = true;
						}
						
						if (ImGui::Selectable(szobjectTag, bSelected))
						{
							pSelectedObject = obj;
							bFound = true;
						}
					}
					ImGui::EndListBox();
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	ImGui::InputText("File_Name :", m_szSaveDataname,MAX_PATH);	 
	Imgui_Save();
	ImGui::SameLine();
	Imgui_Load();

	if (bFound == false)
		pSelectedObject = nullptr;
}

CGameObject * CObject_Manager::Find_Prototype(const wstring& pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), [&](auto Pair)->bool
	{
		if (Pair.first == pPrototypeTag)
			return true;

		return false;
	});

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& pLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), [&](auto Pair)->bool
	{
		if (Pair.first == pLayerTag)
			return true;

		return false;
	});

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (auto& iter : m_vecNameArray)
	{
		Safe_Delete_Array(iter);
	}

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);
	

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();


	

	
}

