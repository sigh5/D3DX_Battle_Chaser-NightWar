#include "stdafx.h"
#include "..\public\Client_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "Model.h"
#include "Environment_Object.h"
#include "UI.h"
#include "PlayerController.h"

#include "Model_Instancing.h"
#include "Buff_Image.h"
#include "Status.h"


_double CClient_Manager::TimeDelta = 0;
_bool	CClient_Manager::bIsCollPlayerTo3DUI[20] = { false };
_matrix	CClient_Manager::m_StaticCameraMatrix = XMMatrixIdentity();
_float	CClient_Manager::m_CameraEye_Z = 0.f;
_bool	CClient_Manager::m_bCombatWin = false;
_bool	CClient_Manager::m_bCombatlose = false;



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
		_bool b = false;
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
				assert(!  "issue" );
	}

	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);

}

void CClient_Manager::Model_Load_2(ID3D11Device * m_pDevice, ID3D11DeviceContext * m_pDeviceContext, _tchar * pDataFileName, _uint iLevel)
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
			CModel_Instancing::Create(m_pDevice, m_pDeviceContext, szModelPath, XMLoadFloat4x4(&PivotMatrix), hFile))))
			assert(!"issue");
	}

	CloseHandle(hFile);
	RELEASE_INSTANCE(CGameInstance);
}

_float CClient_Manager::GetRandomFloat(_float lowBound, _float highBound)
{
	if (lowBound >= highBound)
		return lowBound;

	float f = (rand() % 10000) * 0.0001f;

	return (f * (highBound - lowBound)) + lowBound;
}

void CClient_Manager::CaptinPlayer_ColiderUpdate(CGameObject * pGameObject, CCollider * pColider, CTransform* pTransform)
{
	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);

	if (!lstrcmp(pGameObject->Get_ObjectName(), pPlayerController->Get_Captin()->Get_ObjectName()))
	{
		pColider->Update(pTransform->Get_WorldMatrix());
	}

	RELEASE_INSTANCE(CPlayerController);
}





#ifdef _DEBUG
void CClient_Manager::Make_Anim_Queue(queue<pair<_uint, _double>>& AnimQueue, AnimType eType)
{
	ImGui::Begin("Animation QueueMaker");
	ImGui::Text("");

	static int		iAnimIndex = 0;
	static double	dAnimTickForSecond = 1;
	static queue<pair<_uint, _double>>	m_AnimQueue[ANIM_CHAR_END];

	ImGui::InputInt("AnimIndex", &iAnimIndex);
	ImGui::InputDouble("TickForSecond", &dAnimTickForSecond);

	if (ImGui::Button("Add_Index"))
	{
		m_AnimQueue[eType].push({ iAnimIndex ,dAnimTickForSecond });

	}
	if (ImGui::Button("Delte_Index"))
	{
		queue<pair<_uint, _double>> TempQueue;

		while (!m_AnimQueue[eType].empty())
		{
			_uint iDeletNum = m_AnimQueue[eType].front().first;
			_double Time = m_AnimQueue[eType].front().second;
			m_AnimQueue[eType].pop();

			if (iDeletNum == iAnimIndex)
				continue;
			TempQueue.push({ iDeletNum, Time });

		}

		m_AnimQueue[eType] = TempQueue;
	}

	AnimQueue = m_AnimQueue[eType];


	queue<pair<_uint, _double>> TempQueue;
	TempQueue = m_AnimQueue[eType];

	while (!TempQueue.empty())
	{
		string str = to_string(TempQueue.front().first);

		ImGui::Text(str.c_str());

		TempQueue.pop();
	}

	ImGui::End();
}


void CClient_Manager::Collider_Render(CGameObject * pGameObject, CCollider * pColider,CRenderer* pRenderer)
{
	CGameInstance *pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->GetCurLevelIdx() != LEVEL_GAMEPLAY)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);
	if (!lstrcmp(pGameObject->Get_ObjectName(), pPlayerController->Get_Captin()->Get_ObjectName()))
	{
		pRenderer->Add_DebugRenderGroup(pColider);
		//pColider->Render();
	}

	RELEASE_INSTANCE(CPlayerController);
	RELEASE_INSTANCE(CGameInstance);
}

void CClient_Manager::Navigation_Render(CGameObject * pGameObject, CNavigation * pNavigationCom, CRenderer* pRenderer)
{
	CGameInstance *pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->GetCurLevelIdx() != LEVEL_GAMEPLAY)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);


	if (!lstrcmp(pGameObject->Get_ObjectName(), pPlayerController->Get_Captin()->Get_ObjectName()))
	{
		pRenderer->Add_DebugRenderGroup(pNavigationCom);
	}

	RELEASE_INSTANCE(CPlayerController);
	RELEASE_INSTANCE(CGameInstance);
}



#endif


void  CClient_Manager::Create_BuffImage(vector<CGameObject*>& vecBuffImage,
	_float4 vPos,  _float3 vScale, wstring ProtoTag, _uint iTextureIndex)
{

	for (auto& pBuff : vecBuffImage)
	{
		if (nullptr != pBuff)
		{
			CTexture* pTexture = static_cast<CTexture*>(pBuff->Get_Component(TEXT("Com_Texture")));

			if (pTexture->Get_SelectTextureIndex() == iTextureIndex)
				return;
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject*	pBuffImage = nullptr;

	pBuffImage = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_BuffImage"));

	assert(nullptr != pBuffImage && "CClient_Manager::Create_BuffImage");

	vecBuffImage.push_back(pBuffImage);
	static_cast<CBuff_Image*>(pBuffImage)->Set_BuffImage_PosTransform(vPos, vScale);
	static_cast<CBuff_Image*>(pBuffImage)->Set_TextureIndex(iTextureIndex);

	RELEASE_INSTANCE(CGameInstance);
	return;
}

vector<CGameObject*>::iterator CClient_Manager::Delete_BuffImage(vector<CGameObject*>& vecBuffImage , CStatus * pStauts,_bool bIsPlayer)
{
	CStatus::DEBUFF_TYPE_Desc Desc = pStauts->Get_DebuffType();
	
	vector<_uint> iIndexs;

	if (Desc.isDebuff_FIRE == false)		//0
		iIndexs.push_back(3);
	if (Desc.isDebuff_BLEED == false)
		iIndexs.push_back(1);
	if (Desc.isDebuff_ARMOR == false)
		iIndexs.push_back(4);
	if (Desc.isDebuff_MAGIC == false)
		iIndexs.push_back(2);
	if (Desc.isBuff_Damage == false)
		iIndexs.push_back(0);

	size_t iBuffSize = vecBuffImage.size();

	if (iBuffSize == 0)
		return vecBuffImage.end();

	
	for (auto iter = vecBuffImage.begin(); iter != vecBuffImage.end();)
	{
		if(*iter !=nullptr)
		{ 
			CTexture* pTexture = static_cast<CTexture*>((*iter)->Get_Component(TEXT("Com_Texture")));

			for (_uint i = 0; i < iIndexs.size(); ++i)
			{
				if (pTexture->Get_SelectTextureIndex() == iIndexs[i])
					return iter;
			}
		}
		++iter;
	}	

	return vecBuffImage.end();
}

void CClient_Manager::Sort_BuffImage(vector<CGameObject*>& vecBuffImage,_bool bIsPlayer)
{
	
	if (vecBuffImage.size() == 0)
		return;

	if (true == bIsPlayer)
	{
		_float Xpos = -305.f;
		for (auto pBuff : vecBuffImage)
		{
			if (pBuff != nullptr)
			{
				_float4 vPos;
				XMStoreFloat4(&vPos, pBuff->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
				vPos.x = Xpos;

				pBuff->Get_Transform()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
				Xpos -= 30.f;
			}

		}
	}
	else
	{
		_float Xpos = 500.f;
		for (auto pBuff : vecBuffImage)
		{
			if (pBuff != nullptr)
			{
				_float4 vPos;
				XMStoreFloat4(&vPos, pBuff->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
				vPos.x = Xpos;

				pBuff->Get_Transform()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
				Xpos += 30.f;
			}
		
		}



	}
}




