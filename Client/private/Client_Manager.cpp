#include "stdafx.h"
#include "..\public\Client_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "Model.h"
#include "Environment_Object.h"
#include "UI.h"
_double CClient_Manager::TimeDelta = 0;




void CClient_Manager::Client_Manager_Update()
{
	




}

void CClient_Manager::distance_Limit_UP(CTransform * pTransform, _float4& vLimitFunction, DIR eType)
{
	
	if (eType == DIR_UP)
	{
		_float4 vPosition;
		XMStoreFloat4(&vPosition, pTransform->Get_State(CTransform::STATE_TRANSLATION));

		if (vPosition.y >= vLimitFunction.y)
			return;

		vPosition.y += _float(TimeDelta* 50.f);
		pTransform->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPosition));
	}
	
	else if(eType == DIR_RIGHT)
	{
		_bool b = false;
	}
	


	
}

void CClient_Manager::Model_Load(ID3D11Device*	m_pDevice, ID3D11DeviceContext*	m_pDeviceContext ,_tchar* pDataFileName , _uint iLevel)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	_tchar szPath[MAX_PATH] = TEXT("../../Data/");

	lstrcat(szPath, pDataFileName);
	lstrcat(szPath, TEXT(".dat"));
	
	HANDLE      hFile = CreateFile(szPath,
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

	_tchar ProtoName[MAX_PATH] = TEXT("");
	char	szModelPath[MAX_PATH] = "";
	_float4x4	PivotMatrix;
	XMStoreFloat4x4(&PivotMatrix, XMMatrixIdentity());
	_uint	iAnimType = 0;


	while (true)
	{
		DWORD   dwByte = 0;
		ReadFile(hFile, ProtoName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
		ReadFile(hFile, szModelPath, MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &PivotMatrix, sizeof(_float4x4), &dwByte, nullptr);
		ReadFile(hFile, &iAnimType, sizeof(_uint), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		if (FAILED(pGameInstance->Add_Prototype(iLevel, ProtoName,
			CModel::Create(m_pDevice, m_pDeviceContext, CModel::LOAD_TYPE(iAnimType), szModelPath, XMLoadFloat4x4(&PivotMatrix), hFile))))
			assert("issue");
	}

	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);

}

void CClient_Manager::UI_Load(ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pDeviceContext, _tchar * pDataFileName, _uint iLevel)
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//_tchar szPath[MAX_PATH] = TEXT("../../Data/");

	//lstrcat(szPath, pDataFileName);
	//lstrcat(szPath, TEXT(".dat"));

	//HANDLE      hFile = CreateFile(szPath,
	//	GENERIC_READ,
	//	NULL,
	//	NULL,
	//	OPEN_EXISTING,
	//	FILE_ATTRIBUTE_NORMAL,
	//	NULL);

	//if (INVALID_HANDLE_VALUE == hFile)
	//{
	//	return;
	//}

	//DWORD   dwByte = 0;

	//while (true)
	//{
	//	_float4x4 Worldmatrix = _float4x4();
	//	_tchar	LayerTag[MAX_PATH] = TEXT("");
	//	_tchar	ProtoName[MAX_PATH] = TEXT("");
	//	_tchar	ParentName[MAX_PATH] = TEXT("");
	//	_tchar	TextureName[MAX_PATH] = TEXT("");
	//	_tchar	ObjectName[MAX_PATH] = TEXT("");
	//	_tchar	ModelName[MAX_PATH] = TEXT("");
	//	_uint	iShaderPass = 0;
	//	_uint	iTextureIndex = 0; // 일단 UI 한정


	//	ReadFile(hFile, &Worldmatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
	//	ReadFile(hFile, LayerTag, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
	//	ReadFile(hFile, ProtoName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
	//	ReadFile(hFile, ParentName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
	//	ReadFile(hFile, TextureName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
	//	ReadFile(hFile, ModelName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
	//	ReadFile(hFile, ObjectName, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);
	//	ReadFile(hFile, &iShaderPass, sizeof(_uint), &dwByte, nullptr);
	//	ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
	//	if (0 == dwByte)
	//		break;

	//	if (!lstrcmp(LayerTag, TEXT("Layer_Camera")) || !lstrcmp(LayerTag, TEXT("Layer_BackGround")))
	//			continue;

	//	/*if (!lstrcmp(LayerTag, TEXT("Layer_BackGround")))
	//	{
	//	CGameObject* pGameObject = nullptr;
	//	Clone_GameObject_UseImgui(pLevelManager->GetCurLevelIdx(), LayerTag, ProtoName, &pGameObject);
	//	(pGameObject)->Set_ProtoName(ProtoName);
	//	(pGameObject)->Set_ObjectName(ObjectName);
	//	(pGameObject)->Get_Transform()->Set_WorldMatrix(Worldmatrix);
	//	}
	//	*/
	//	if (!lstrcmp(LayerTag, TEXT("Layer_Environment")))
	//	{
	//		CEnvironment_Object::ENVIRONMENTDESC Desc;
	//		ZeroMemory(&Desc, sizeof(Desc));
	//		lstrcpy(Desc.m_pModelTag, ModelName);
	//		Desc.m_iShaderPass = iShaderPass;
	//		lstrcpy(Desc.m_pTextureTag, TextureName);
	//		CGameObject* pGameObject = nullptr;
	//		pGameInstance->Clone_GameObject_UseImgui(iLevel, LayerTag, ProtoName, &pGameObject, &Desc);
	//		(pGameObject)->Set_ProtoName(ProtoName);
	//		(pGameObject)->Set_ObjectName(ObjectName);
	//		(pGameObject)->Get_Transform()->Set_WorldMatrix(Worldmatrix);
	//	}


	//	if (!lstrcmp(LayerTag, TEXT("Layer_UI")))
	//	{
	//		CUI::UIDESC UIDesc;
	//		ZeroMemory(&UIDesc, sizeof(UIDesc));
	//		lstrcpy(UIDesc.m_pTextureTag, TextureName);

	//		CGameObject* pGameObject = nullptr;
	//		pGameInstance->Clone_GameObject_UseImgui(iLevel, LayerTag, ProtoName, &pGameObject, &UIDesc);
	//		(pGameObject)->Set_ProtoName(ProtoName);
	//		(pGameObject)->Set_ObjectName(ObjectName);
	//		(pGameObject)->Get_Transform()->Set_WorldMatrix(Worldmatrix);

	//		CTexture* pTexture = dynamic_cast<CTexture*>(pGameObject->Get_Component(TEXT("Com_Texture")));
	//		pTexture->Set_SelectTextureIndex(iTextureIndex);

	//		if (lstrcmp(ParentName, TEXT("Nullptr")))
	//			pGameObject->Set_parentName(ParentName);
	//	}

	//}

	//CloseHandle(hFile);
	//MSG_BOX("Load");
	//RELEASE_INSTANCE(CGameInstance);

}
	









