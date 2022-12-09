#include "stdafx.h"
#include "..\public\Client_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "Model.h"

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







