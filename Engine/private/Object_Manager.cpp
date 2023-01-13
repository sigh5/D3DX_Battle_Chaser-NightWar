#include "..\public\Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Canvas.h"
#include "Level_Manager.h"
#include "Component_Manager.h"
#include "Camera.h"
#include "Environment_Object.h"
#include <utility>
#include "Texture.h"
#include "HitBoxObject.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ComponentPtr(pComponentTag, iIndex);
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	m_LayerName.push_back(LAYER_BACKGROUND);
	m_LayerName.push_back(LAYER_ENVIRONMENT);
	m_LayerName.push_back(LAYER_PLAYER);
	m_LayerName.push_back(LAYER_Monster);
	m_LayerName.push_back(LAYER_UI);
	m_LayerName.push_back(LAYER_EFFECT);
	// Todo  Layer는 상수로 추가했음


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

HRESULT CObject_Manager::Copy_Data(_uint iLevelPreIndex, _uint iLevelCur)
{
	if (iLevelCur >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelPreIndex])
	{
		if (Pair.first == LAYER_PLAYER)
		{
			CLayer * pLayer = CLayer::Create();
			pLayer->CopyData(Pair.second->GetGameObjects());
			m_pLayers[iLevelCur].emplace(Pair.first, pLayer);
		}
	}

	for (auto& Pair : m_pLayers[iLevelPreIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelPreIndex].clear();
	return S_OK;
}

HRESULT CObject_Manager::Loading_Objects()
{
	// 로딩 레벨 삭제
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	for (auto &Pair : m_pLayers[0])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[0].clear();
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

CGameObject * CObject_Manager::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	CGameObject*		pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject*		pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

CGameObject* CObject_Manager::Clone_UI(_uint iLevel, const wstring& pLayerTag ,  CGameObject * pGameObject)
{
	CGameObject*		pPrototype = Find_Prototype(pGameObject->Get_ProtoName());
	void *pArg = nullptr;

	CUI::UIDESC m_UIdesc;
	ZeroMemory(&m_UIdesc, sizeof(m_UIdesc));
	
	lstrcpy(m_UIdesc.m_pTextureTag, dynamic_cast<CUI*>(pGameObject)->Get_TextureTag());

	CGameObject*		pCloneGameObject = pPrototype->Clone(&m_UIdesc);
	if (pCloneGameObject == nullptr)
		return nullptr;
	
	_tchar* szObjectName= new _tchar[MAX_PATH];
	m_vecNameArray.push_back(szObjectName);

	static _int iNum = 20;

	lstrcpy(szObjectName, pGameObject->Get_ObjectName());
	lstrcat(szObjectName, std::to_wstring(iNum++).c_str());
	CTexture* pOriginObjectTexture = static_cast<CTexture*>(pGameObject->Get_Component(TEXT("Com_Texture")));

	pCloneGameObject->Set_ObjectName(szObjectName);
	static_cast<CTexture*>(pCloneGameObject->Get_Component(TEXT("Com_Texture")))->Set_SelectTextureIndex(pOriginObjectTexture->Get_SelectTextureIndex());
	pCloneGameObject->Set_ProtoName(pGameObject->Get_ProtoName());
	pCloneGameObject->Set_parent(pGameObject->Get_parentName());

	CLayer*		pLayer = Find_Layer(iLevel, pLayerTag);

	if (pLayer == nullptr)
		return nullptr;

	pLayer->Add_GameObject(pCloneGameObject);

	pCloneGameObject->Get_Transform()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -500.f, 0.f, 1.f));

	return pCloneGameObject;
}


CGameObject * CObject_Manager::Get_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjectNameTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	for (auto& iter : pLayer->GetGameObjects())
	{
		if (iter != nullptr && !lstrcmp(iter->Get_ObjectName(), pObjectNameTag.c_str()))
			return iter;
	}

	return nullptr;
}

void CObject_Manager::DeleteGameObject(_uint iLevelIndex ,const wstring& ObjName )
{
	const LAYERS& targetLevel = m_pLayers[iLevelIndex];

	
	for (auto& Pair : targetLevel)
	{
		for (auto& obj : Pair.second->GetGameObjects())
		{
			if (nullptr != obj)
			{
				if (!lstrcmp(obj->Get_ObjectName(), ObjName.c_str()))
					{
						Pair.second->Delete_GameObject(obj);
						return;
					}
				}
			}

	}
}

void CObject_Manager::Change_Level(_uint iLevleIdx)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Change_Level(iLevleIdx);
		}
	}
}

void CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
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

void CObject_Manager::Imgui_Swap_Render_Sorting(_uint iLevel, CGameObject* pSelectedObject)
{
	// OnlyUI
	static int iWantIndex = 0;
	ImGui::InputInt("SwapIndex", &iWantIndex);


	if (ImGui::Button("Swap"))
	{
		const LAYERS& targetLevel = m_pLayers[iLevel];

		CGameObject* pWantIndex = nullptr;

		list<class CGameObject*>::iterator  SwapIter;
		list<class CGameObject*>::iterator  SelcetIter;
		_uint		iAddIndex = 0;
		_uint		iSelectIndex = 0;
	
		for (auto& Pair : targetLevel) // for layer loop
		{
			if(Pair.first != LAYER_UI)
				continue;

			for (auto iter = Pair.second->GetGameObjects().begin(); iter != Pair.second->GetGameObjects().end(); ++iter )
			{
				if (iWantIndex == iAddIndex)
				{
					SwapIter = iter;
				}

				if (!lstrcmp((*iter)->Get_ObjectName(), pSelectedObject->Get_ObjectName()))
				{
					SelcetIter = iter;
				}
				++iAddIndex;
			}

			Pair.second->GetGameObjects().splice(SwapIter, Pair.second->GetGameObjects(),SelcetIter);
		}
	}


}

void CObject_Manager::Imgui_RemoveObject(_uint iLevel, OUT CGameObject ** ppGameObject)
{
	const LAYERS& targetLevel = m_pLayers[iLevel];

	if (ImGui::Button("Remove"))
	{
		for (auto& Pair : targetLevel)
		{
			for (auto& obj : Pair.second->GetGameObjects())
			{
				if (nullptr != obj)
				{
					if (!lstrcmp((*ppGameObject)->Get_ObjectName(), obj->Get_ObjectName()))
					{
						*ppGameObject = nullptr;
						Pair.second->Delete_GameObject(obj);
						return;
					}
				}
			}

		}	
	}
}

void CObject_Manager::Imgui_Save()
{
	if (ImGui::Button("Save_Scene"))
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
		_tchar *ModelName = new _tchar[MAX_PATH];
		_tchar *ObjectName = new _tchar[MAX_PATH];
		_uint	iShaderPass = 0;
		_uint	iTextureIndex = 0; // 일단 UI 한정
		for (_uint i = 0; i < m_iNumLevels; ++i)
		{
			for (auto& Pair : m_pLayers[i])
			{
				lstrcpy(LayerTag, Pair.first.c_str());

				if(!lstrcmp(LayerTag , TEXT("Layer_Camera")) || !lstrcmp(LayerTag, TEXT("Layer_BackGround")))
					continue;

				for (auto& pObject : Pair.second->GetGameObjects())
				{
					_float4x4 Worldmatrix = _float4x4();
					lstrcpy(ProtoName, pObject->Get_ProtoName());
					lstrcpy(ObjectName, pObject->Get_ObjectName());
					XMStoreFloat4x4(&Worldmatrix, pObject->Get_Transform()->Get_LocalMatrix());
					/*if (Pair.first == TEXT("Layer_BackGround"))
					{
						lstrcpy(ParentName, TEXT("Nullptr"));
						lstrcpy(TextureName, TEXT(""));
					}*/
					
					if (Pair.first == TEXT("Layer_Environment"))
					{
						lstrcpy(ParentName, TEXT("Nullptr"));
						lstrcpy(TextureName, dynamic_cast<CEnvironment_Object*>(pObject)->Get_TextureTag());
						if (!lstrcmp(TextureName, TEXT("")))
							lstrcpy(TextureName , TEXT("Nullptr"));
						lstrcpy(ModelName, dynamic_cast<CEnvironment_Object*>(pObject)->Get_ModelTag());
						iShaderPass = dynamic_cast<CEnvironment_Object*>(pObject)->Get_ShaderPass();
					}


					if (Pair.first == TEXT("Layer_UI"))
					{
						CGameObject* pParent = pObject->Get_parentName();
						if (nullptr != pParent)
							lstrcpy(ParentName, pParent->Get_ObjectName());
						else
							lstrcpy(ParentName, TEXT("Nullptr"));

						lstrcpy(TextureName, dynamic_cast<CUI*>(pObject)->Get_TextureTag());
						lstrcpy(ModelName, TEXT("Nullptr"));
						
						iShaderPass = static_cast<CUI*>(pObject)->Get_Shaking_Option();

						CTexture* pTexture =dynamic_cast<CTexture*>(pObject->Get_Component(TEXT("Com_Texture")));
						iTextureIndex = pTexture->Get_SelectTextureIndex();
					}

				
					WriteFile(hFile, &Worldmatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
					WriteFile(hFile, LayerTag, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
					WriteFile(hFile, ProtoName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
					WriteFile(hFile, ParentName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
					WriteFile(hFile, TextureName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
					WriteFile(hFile, ModelName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
					WriteFile(hFile, ObjectName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
					WriteFile(hFile, &iShaderPass, sizeof(_uint), &dwByte, nullptr);
					WriteFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
				}
			}
		}


		CloseHandle(hFile);
		m_vecNameArray.push_back(ModelName);
		m_vecNameArray.push_back(ObjectName);
		m_vecNameArray.push_back(szName);
		m_vecNameArray.push_back(LayerTag);
		m_vecNameArray.push_back(ProtoName);
		m_vecNameArray.push_back(ParentName);
		m_vecNameArray.push_back(TextureName);

		MSG_BOX("Save_Complete");
		RELEASE_INSTANCE(CLevel_Manager);

	}



}

void CObject_Manager::Imgui_Save_Effect()
{
	if (ImGui::Button("Save_Scene_Effect"))
	{
		_tchar* szName = new _tchar[MAX_PATH];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szSaveDataname, sizeof(char[256]), szName, sizeof(_tchar[256]));

		_tchar szPath[MAX_PATH] = TEXT("../../Data/Skill_Effect/");

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
		_tchar *ModelName = new _tchar[MAX_PATH];
		_tchar *ObjectName = new _tchar[MAX_PATH];
		_uint	iShaderPass = 0;
		_uint	iTextureIndex = 0; // 일단 Effect 한정
		for (_uint i = 0; i < m_iNumLevels; ++i)
		{
			for (auto& Pair : m_pLayers[i])
			{
				lstrcpy(LayerTag, Pair.first.c_str());

				if (!lstrcmp(LayerTag, TEXT("Layer_Camera")) || !lstrcmp(LayerTag, TEXT("Layer_BackGround")))
					continue;

				for (auto& pObject : Pair.second->GetGameObjects())
				{
					_float4x4 Worldmatrix = _float4x4();
					lstrcpy(ProtoName, pObject->Get_ProtoName());
					lstrcpy(ObjectName, pObject->Get_ObjectName());
					XMStoreFloat4x4(&Worldmatrix, pObject->Get_Transform()->Get_LocalMatrix());

					if (Pair.first == LAYER_EFFECT)
					{
						CGameObject* pParent = pObject->Get_parentName();
						if (nullptr != pParent)
							lstrcpy(ParentName, pParent->Get_ObjectName());
						else
							lstrcpy(ParentName, TEXT("Nullptr"));

						CHitBoxObject::HitBoxObject Desc;
						Desc = dynamic_cast<CHitBoxObject*>(pObject)->Get_HitBoxDesc();

						lstrcpy(TextureName, Desc.HitBoxOrigin_Desc.m_pTextureTag);
						lstrcpy(ModelName, Desc.HitBoxOrigin_Desc.m_pModelTag);
						iShaderPass = Desc.HitBoxOrigin_Desc.m_iShaderPass;

						CTexture* pTexture = dynamic_cast<CTexture*>(pObject->Get_Component(TEXT("Com_Texture")));
						iTextureIndex = pTexture->Get_SelectTextureIndex();

						CVIBuffer_Point_Instancing::VIBUffer_Point_TextureDesc PointDesc = Desc.Poing_Desc;

						_int iFrameCnt = PointDesc.m_iFrameCnt;
						_float fTextureMax_Width_Cnt = PointDesc.m_iTextureMax_Width_Cnt;
						_float fTextureMax_Height_Cnt = PointDesc.m_iTextureMax_Height_Cnt;

						WriteFile(hFile, &iFrameCnt, sizeof(_int), &dwByte, nullptr);
						WriteFile(hFile, &fTextureMax_Width_Cnt, sizeof(_float), &dwByte, nullptr);
						WriteFile(hFile, &fTextureMax_Height_Cnt, sizeof(_float), &dwByte, nullptr);
						WriteFile(hFile, &Worldmatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
						WriteFile(hFile, LayerTag, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, ProtoName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, ParentName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, TextureName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, ModelName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, ObjectName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
						WriteFile(hFile, &iShaderPass, sizeof(_uint), &dwByte, nullptr);
						WriteFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
					}

					
				}
			}
		}
		m_vecNameArray.push_back(ModelName);
		m_vecNameArray.push_back(ObjectName);
		m_vecNameArray.push_back(szName);
		m_vecNameArray.push_back(LayerTag);
		m_vecNameArray.push_back(ProtoName);
		m_vecNameArray.push_back(ParentName);
		m_vecNameArray.push_back(TextureName);
		MSG_BOX("Save_Complete");
		CloseHandle(hFile);
		RELEASE_INSTANCE(CLevel_Manager);
	}
}

CGameObject* CObject_Manager::Load_Object(const _tchar *pDataFileName, _uint iCurLevel)
{

	CLevel_Manager* pLevelManager = GET_INSTANCE(CLevel_Manager);
	_tchar* szName = new _tchar[256];
	m_vecNameArray.push_back(szName);
	_tchar szPath[MAX_PATH] = TEXT("../../Data/");
	CGameObject* pGameObject = nullptr;
	if (0 == iCurLevel)
		iCurLevel = pLevelManager->GetCurLevelIdx();

	if(pDataFileName ==nullptr)
	{ 
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szSaveDataname, sizeof(char[256]), szName, sizeof(_tchar[256]));
		lstrcat(szPath, szName);
		lstrcpy(szName, szPath);
		lstrcat(szName, TEXT(".dat"));
	}
	else
	{
		lstrcat(szPath, pDataFileName);
		lstrcpy(szName, szPath);
		lstrcat(szName, TEXT(".dat"));
	}
	
	HANDLE      hFile = CreateFile(szName,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return nullptr;
	}

	DWORD   dwByte = 0;

	_uint iObjectNumber = 0;


	while (true)
	{
		_float4x4 Worldmatrix = _float4x4();
		_tchar* LayerTag    = new _tchar[MAX_PATH];
		_tchar* ProtoName  = new _tchar[MAX_PATH];
		_tchar* ParentName  = new _tchar[MAX_PATH];
		_tchar* TextureName  = new _tchar[MAX_PATH];
		_tchar *ObjectName  = new _tchar[MAX_PATH];
		_tchar *ModelName    = new _tchar[MAX_PATH];
		_uint	iShaderPass = 0;
		_uint	iTextureIndex = 0; // 일단 UI 한정

		m_vecNameArray2.push_back(ModelName);
		m_vecNameArray2.push_back(ObjectName);
		m_vecNameArray2.push_back(LayerTag);
		m_vecNameArray2.push_back(ProtoName);
		m_vecNameArray2.push_back(ParentName);
		m_vecNameArray2.push_back(TextureName);



		ReadFile(hFile, &Worldmatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
		ReadFile(hFile, LayerTag, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, ProtoName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, ParentName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, TextureName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, ModelName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, ObjectName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, &iShaderPass, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		if (!lstrcmp(LayerTag, TEXT("Layer_Camera")) || !lstrcmp(LayerTag, TEXT("Layer_BackGround")))
			continue;

		/*if (!lstrcmp(LayerTag, TEXT("Layer_BackGround")))
		{
		CGameObject* pGameObject = nullptr;
		Clone_GameObject_UseImgui(pLevelManager->GetCurLevelIdx(), LayerTag, ProtoName, &pGameObject);
		(pGameObject)->Set_ProtoName(ProtoName);
		(pGameObject)->Set_ObjectName(ObjectName);
		(pGameObject)->Get_Transform()->Set_WorldMatrix(Worldmatrix);
		}
		*/
		if (!lstrcmp(LayerTag, TEXT("Layer_Environment")))
		{
			CEnvironment_Object::ENVIRONMENTDESC Desc;
			ZeroMemory(&Desc, sizeof(Desc));
			lstrcpy(Desc.m_pModelTag, ModelName);
			Desc.m_iShaderPass = iShaderPass;
			lstrcpy(Desc.m_pTextureTag, TextureName);
			
			Clone_GameObject_UseImgui(iCurLevel, LayerTag, ProtoName, &pGameObject, &Desc);
			if (pGameObject == nullptr)
				assert(!"????");
			(pGameObject)->Set_ProtoName(ProtoName);
			(pGameObject)->Set_ObjectName(ObjectName);
			(pGameObject)->Get_Transform()->Set_WorldMatrix(Worldmatrix);
		}


		if (!lstrcmp(LayerTag, TEXT("Layer_UI")))
		{
			CUI::UIDESC UIDesc;
			ZeroMemory(&UIDesc, sizeof(UIDesc));
			lstrcpy(UIDesc.m_pTextureTag, TextureName);

			Clone_GameObject_UseImgui(iCurLevel, LayerTag, ProtoName, &pGameObject, &UIDesc);
			if (pGameObject == nullptr)
				assert(!"????");
			(pGameObject)->Set_ProtoName(ProtoName);
			(pGameObject)->Set_ObjectName(ObjectName);
			(pGameObject)->Get_Transform()->Set_WorldMatrix(Worldmatrix);
			static_cast<CUI*>(pGameObject)->Set_Shaking_Option(iShaderPass);


			CTexture* pTexture = dynamic_cast<CTexture*>(pGameObject->Get_Component(TEXT("Com_Texture")));
			pTexture->Set_SelectTextureIndex(iTextureIndex);

			if (lstrcmp(ParentName, TEXT("Nullptr")))
				pGameObject->Set_parentName(iCurLevel,ParentName);
		}


	}

	CloseHandle(hFile);
	RELEASE_INSTANCE(CLevel_Manager);

	if (nullptr != pGameObject)
		return pGameObject;
	else
		assert(!  "Error");
	

}

CGameObject* CObject_Manager::Load_Effect(const _tchar * pDataFileName, _uint iCurLevel, _bool bIsHaveLayer)
{
	//bIsHaveLayer = true;


	CLevel_Manager* pLevelManager = GET_INSTANCE(CLevel_Manager);
	_tchar* szName = new _tchar[256];
	m_vecNameArray.push_back(szName);
	_tchar szPath[MAX_PATH] = TEXT("../../Data/Skill_Effect/");
	CGameObject* pGameObject = nullptr;
	if (0 == iCurLevel)
		iCurLevel = pLevelManager->GetCurLevelIdx();

	if (pDataFileName == nullptr)
	{
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, m_szSaveDataname, sizeof(char[256]), szName, sizeof(_tchar[256]));
		lstrcat(szPath, szName);
		lstrcpy(szName, szPath);
		lstrcat(szName, TEXT(".dat"));
	}
	else
	{
		lstrcat(szPath, pDataFileName);
		lstrcpy(szName, szPath);
		lstrcat(szName, TEXT(".dat"));
	}

	HANDLE      hFile = CreateFile(szName,
		GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		return nullptr;
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
		_tchar *ModelName = new _tchar[MAX_PATH];
		_uint	iShaderPass = 0;
		_uint	iTextureIndex = 0; // 일단 UI 한정
		_int iFrameCnt = 0;
		_float fTextureMax_Width_Cnt = 0.f;
		_float fTextureMax_Height_Cnt = 0.f;

	

		m_vecNameArray2.push_back(ModelName);
		m_vecNameArray2.push_back(ObjectName);
		m_vecNameArray2.push_back(LayerTag);
		m_vecNameArray2.push_back(ProtoName);
		m_vecNameArray2.push_back(ParentName);
		m_vecNameArray2.push_back(TextureName);


		ReadFile(hFile, &iFrameCnt, sizeof(_int), &dwByte, nullptr);
		ReadFile(hFile, &fTextureMax_Width_Cnt, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &fTextureMax_Height_Cnt, sizeof(_float), &dwByte, nullptr);
		ReadFile(hFile, &Worldmatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
		ReadFile(hFile, LayerTag, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, ProtoName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, ParentName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, TextureName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, ModelName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, ObjectName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, &iShaderPass, sizeof(_uint), &dwByte, nullptr);
		ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		if (!lstrcmp(LayerTag, TEXT("Layer_Camera")) || !lstrcmp(LayerTag, TEXT("Layer_BackGround")))
			continue;

		if (!lstrcmp(LayerTag, LAYER_EFFECT))
		{
			CHitBoxObject::HitBoxObject Desc;
			
			
			ZeroMemory(&Desc, sizeof(Desc));
			lstrcpy(Desc.HitBoxOrigin_Desc.m_pModelTag, ModelName);
			Desc.HitBoxOrigin_Desc.m_iShaderPass = iShaderPass;
			lstrcpy(Desc.HitBoxOrigin_Desc.m_pTextureTag, TextureName);
			
			
			Desc.Poing_Desc.m_iFrameCnt = iFrameCnt;
			Desc.Poing_Desc.m_iTextureMax_Height_Cnt = fTextureMax_Height_Cnt;
			Desc.Poing_Desc.m_iTextureMax_Width_Cnt = fTextureMax_Width_Cnt;
			

			CGameObject*		pPrototype = Find_Prototype(ProtoName);
			if (nullptr == pPrototype)
				return nullptr;

			if (bIsHaveLayer == true)
				Clone_GameObject_UseImgui(iCurLevel, LayerTag, ProtoName, &pGameObject, &Desc);			
			
			else
				pGameObject = pPrototype->Clone(&Desc);

			if (pGameObject == nullptr)
				assert(!"????");
			(pGameObject)->Set_ProtoName(ProtoName);
			(pGameObject)->Set_ObjectName(ObjectName);
			(pGameObject)->Get_Transform()->Set_WorldMatrix(Worldmatrix);

			CTexture* pTexture = dynamic_cast<CTexture*>(pGameObject->Get_Component(TEXT("Com_Texture")));
			pTexture->Set_SelectTextureIndex(iTextureIndex);
		}
	}

	CloseHandle(hFile);
	RELEASE_INSTANCE(CLevel_Manager);
	
	if (pGameObject == nullptr)
		MSG_BOX("Load_Effect_Error");
	return pGameObject;
}

void CObject_Manager::SceneChange_NameVectorClear()
{
	for (auto& iter : m_vecNameArray2)
	{
		Safe_Delete_Array(iter);
	}
	m_vecNameArray2.clear();
}




void CObject_Manager::Imgui_Load()
{
	if (ImGui::Button("Load_Scene"))
	{
		Load_Object();
	}
	if (ImGui::Button("Load_Effect"))
	{
		Load_Effect();
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
						if (obj != nullptr)
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

	ImGui::InputText("File_Name :", m_szSaveDataname, MAX_PATH);
	
	Imgui_RemoveObject(iLevel,&pSelectedObject);
	ImGui::NewLine();
	Imgui_Save();
	ImGui::SameLine();
	Imgui_Save_Effect();
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
	for (auto& iter : m_vecNameArray2)
	{
		Safe_Delete_Array(iter);
	}

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

