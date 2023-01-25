#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "GameInstance.h"

#include "PlayerController.h"
#include "Client_Manager.h"
#include "Camera_Static.h"
#include "Damage_Font_Manager.h"
#include "Explain_FontMgr.h"


#include "Level_Loading.h"
#include "UI.h"
#include "Broken_Image.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pPlayerController(CPlayerController::GetInstance())
	
	

{
}

HRESULT CLevel_GamePlay::Initialize()
{

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Change_SceneData()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Environment(TEXT("Layer_Environment"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;
	
	m_pPlayerController->Reset_LateInit();

	//CExplain_FontMgr::GetInstance()->Initialize();
	
	return S_OK;

}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
	m_pPlayerController->Late_Initialize();

	__super::Tick(TimeDelta);
	
	m_pPlayerController->Player_Controll_Tick(TimeDelta);

	m_TimeAcc += TimeDelta;

#ifdef FONT_TEST
	//CExplain_FontMgr::GetInstance()->Tick(TimeDelta);
#else


#endif

	//static float BUffPos[3] = { 0.f,0.f,0.f };
	//ImGui::InputFloat3("BuffPos", BUffPos);
	//
	//if (ImGui::Button("Create_Buff_Pos"))
	//{
	//	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	//	pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY,LAYER_UI,TEXT("Prototype_GameObject_BuffImage"));
	//		
	//	_float4 vPos = _float4(BUffPos[0], BUffPos[1], 0.1f, 1.f);
	//	_float3 vScale = _float3(30.f,30.f,1.f);


	//	static_cast<CBuff_Image*>(pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, LAYER_UI, L"BUff_Image"))->Set_BuffImage_PosTransform(vPos, vScale);


	//	RELEASE_INSTANCE(CGameInstance);
	//}



#ifdef NOMODLES


#else
	Dungeon_Controll_Tick(TimeDelta);
#endif
}

void CLevel_GamePlay::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	

	//CExplain_FontMgr::GetInstance()->Late_Tick(TimeDelta);
	//if (GetKeyState(VK_SPACE) & 0x8000)
	//{
	//	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	//	Safe_AddRef(pGameInstance);
	//	pGameInstance->SceneChange_NameVectorClear();
	//	pGameInstance->Set_CopyIndexs(LEVEL_GAMEPLAY, LEVEL_COMBAT);

	//	if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_COMBAT), true)))
	//		return;

	//	Safe_Release(pGameInstance);
	//}
#ifdef NOMODLES


#else
	if (m_bSceneChange)
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		
		CGameObject* pBroken_Image = nullptr;

		pBroken_Image = pGameInstance->Get_GameObject(pGameInstance->GetCurLevelIdx(), TEXT("Layer_UI"), TEXT("Broken_Image"));

		static_cast<CBroken_Image*>(pBroken_Image)->Reset_Anim();


		CGameObject* pCam = nullptr;
		pCam = pGameInstance->Get_GameObject(pGameInstance->GetCurLevelIdx(), TEXT("Layer_Camera"), TEXT("Static_Camera"));
		CClient_Manager::m_StaticCameraMatrix =pCam->Get_Transform()->Get_WorldMatrix();
		CClient_Manager::m_CameraEye_Z = static_cast<CCamera_Static*>(pCam)->Get_CameraZ();
		
		pGameInstance->SceneChange_NameVectorClear();
		pGameInstance->Set_CopyIndexs(LEVEL_GAMEPLAY, LEVEL_COMBAT);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_COMBAT), true)))
			return;

		Safe_Release(pGameInstance);
	}

	

#endif
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	++m_iNumCallDraw;

	if (m_TimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("fps : %d"), m_iNumCallDraw);


		m_iNumCallDraw = 0;
		m_TimeAcc = 0.f;
	}

	SetWindowText(g_hWnd, m_szFPS);

	return S_OK;
}

void CLevel_GamePlay::Dungeon_Controll_Tick(_double TimeDelta)
{
	m_pPlayerController->Set_CaptinPlayer();
	m_pPlayerController->SyncAninmation();	//->여건 내가 맞춰야겠다.

}

HRESULT CLevel_GamePlay::Ready_Change_SceneData()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Change_Level(LEVEL_GAMEPLAY);

	CPlayerController::GetInstance()->Change_Scene(LEVEL_GAMEPLAY);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_SkyBox"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;



}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_CCamera_Static"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Environment(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
#ifdef NOMODLES
	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Map_3D_UI"))))
	//	return E_FAIL;

#else
	pGameInstance->Load_Object(TEXT("Map_oneData"), LEVEL_GAMEPLAY);
#endif

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

#ifdef NOMODLES


#else
	if (pGameInstance->m_bOnceCreatePlayer == false)
	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Hero_Gully"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Hero_Garrison"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Hero_Calibretto"))))
			return E_FAIL;

		m_pPlayerController->Initialize(LEVEL_GAMEPLAY);
		pGameInstance->m_bOnceCreatePlayer = true;
		
	}
#endif

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

#ifdef NOMODLES

#else
	pGameInstance->Load_Object(TEXT("DungeonUI"),LEVEL_GAMEPLAY);
	pGameInstance->Load_Object(TEXT("Inventory"), LEVEL_GAMEPLAY);
	
	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Broken_Image"))))
		return E_FAIL;*/

#endif


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
#ifdef NOMODLES

#else

#endif
	
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	pGameInstance->Clear_Light();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext,TEXT("Level_Game_Directional") ,LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.isEnable = true;
	/*LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);*/
	LightDesc.vPosition = _float4(11.f, 3.f, 2.7f, 1.f);
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 0.2f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, TEXT("Level_Game_PointLight_One"), LightDesc)))
		return E_FAIL;


	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.isEnable = true;
	/*LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);*/
	LightDesc.vPosition = _float4(11.f, 3.f, 9.3f, 1.f);
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 0.2f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, TEXT("Level_Game_PointLight_Two"), LightDesc)))
		return E_FAIL;


	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.isEnable = true;
	/*LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);*/
	LightDesc.vPosition = _float4(19.f, 3.f, 13.f, 1.f);
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 0.2f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, TEXT("Level_Game_PointLight_Three"), LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.isEnable = true;
	/*LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);*/
	LightDesc.vPosition = _float4(11.f, 3.f, 15.f, 1.f);
	LightDesc.fRange = 5.f;
	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.2f, 0.2f, 0.2f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, TEXT("Level_Game_PointLight_Four"), LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CLevel_GamePlay::Scene_Change()
{
	m_bSceneChange = true;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
	
	

}
