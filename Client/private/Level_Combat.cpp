#include "stdafx.h"
#include "..\public\Level_Combat.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "Client_Manager.h"
#include "CombatController.h"
#include "PlayerController.h"
#include "TrunWinCanvas.h"
#include "TrunLoseCanvas.h"
#include "Spider_Mana.h"
#include "SoundPlayer.h"
#include "MeshGround.h"

CLevel_Combat::CLevel_Combat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pCombatController(CCombatController::GetInstance())
	
{
	
}

HRESULT CLevel_Combat::Initialize()
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

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;
	
	
	m_pCombatController->Initialize(LEVEL_COMBAT);
	


	//CGameInstance::GetInstance()->Play_Sound(TEXT("02_Nights_Curse.wav"), 0.2f, true, SOUND_BGM);



	return S_OK;
}

void CLevel_Combat::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_dCombatIntroTimer += TimeDelta *1.0;

	Combat_Intro(TimeDelta);
	__super::Tick(TimeDelta);
	Combat_Control_Tick(TimeDelta);
	CSoundPlayer::GetInstance()->Tick(TimeDelta);

	m_TimeAcc += TimeDelta;
}

void CLevel_Combat::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pCombatController->Late_Tick(TimeDelta);

	if (true == CClient_Manager::m_bCombatWin)
	{
		m_fSceneChaneTimer += _float(TimeDelta);
		//m_pCombatController->Render_StopCanvas();
	}
	if (m_fSceneChaneTimer >= 2.f)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CGameInstance::GetInstance()->Load_Object(TEXT("TrunWinUI_Data"), LEVEL_COMBAT);
		CTrunWinCanvas* pWinCanvas = static_cast<CTrunWinCanvas*>(CGameInstance::GetInstance()->Get_GameObject(pGameInstance->GetCurLevelIdx(),
			LAYER_UI, TEXT("TurnWinCanvas")));
		assert(nullptr != pWinCanvas && "CCombatController::PlayerWin");
		
		CCombatController::GetInstance()->Setting_Win_Canvas(pWinCanvas);
		pWinCanvas->Set_WinRender(true);

		CClient_Manager::m_bCombatWin = false;
		m_fSceneChaneTimer = 0.f;
		m_bSceneChange = true;
		RELEASE_INSTANCE(CGameInstance);
	}
	
	if (true == CClient_Manager::m_bCombatlose)
	{
		m_fSceneChaneTimer_lose += _float(TimeDelta);
		//m_pCombatController->Render_StopCanvas();
	}
	if (m_fSceneChaneTimer_lose >= 5.f)
	{

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CGameInstance::GetInstance()->Load_Object(TEXT("TrunLoseUI_Data"), LEVEL_COMBAT);
		CTrunLoseCanvas* pLoseCanvas = static_cast<CTrunLoseCanvas*>(CGameInstance::GetInstance()->Get_GameObject(pGameInstance->GetCurLevelIdx(),
			LAYER_UI, TEXT("Texture_TrunBattle_WinCanvas")));
		assert(nullptr != pLoseCanvas && "CCombatController::PlayerWin");
		pLoseCanvas->Set_LoseRender(true);

		CClient_Manager::m_bCombatlose = false;
		m_fSceneChaneTimer_lose = 0.f;
		m_bSceneChange = true;
		RELEASE_INSTANCE(CGameInstance);
	}


	if (m_bSceneChange &&  GetKeyState(VK_RETURN) & 0x8000)
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->SceneChange_NameVectorClear();
		m_pCombatController->Scene_Chane_Safe_Release();
		pGameInstance->Set_CopyIndexs(LEVEL_COMBAT, LEVEL_GAMEPLAY);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY), true)))
			return;

		pGameInstance->Stop_Sound(SOUND_BGM);
		CSoundPlayer::GetInstance()->Clear_allSound();
		
		CClient_Manager::m_bCombatWin = false;
		Safe_Release(pGameInstance);
		m_bSceneChange = false;
	}



	
}

HRESULT CLevel_Combat::Render()
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

void CLevel_Combat::Combat_Control_Tick(_double TimeDelta)
{
	m_pCombatController->CurrentTurn_ActorControl(TimeDelta);

	//if (ImGui::Button("Ultimate_ON"))
	//{
	//	m_pCombatController->Ultimate_Camera_On();
	//}
	//
	//if (ImGui::Button("Ultimate_OFF"))
	//{
	//	m_pCombatController->Ultimate_Camera_Off();
	//}


}

void CLevel_Combat::Combat_Intro(_double Timedelta)
{
	if (m_bIntroFinish)
	{
		return;
	}
	if (m_dCombatIntroTimer >= 0.15f)
	{
		m_pCombatController->Set_CombatIntro(true);
		m_bIntroFinish = true;
	}
}




HRESULT CLevel_Combat::Ready_Change_SceneData()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Change_Level(LEVEL_COMBAT);
	CPlayerController::GetInstance()->Change_Scene(LEVEL_COMBAT);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Layer_BackGround(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
#ifdef LEVEL_BOSSONLY
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_SkyBox"))))
		return E_FAIL;

#endif
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Light_Pos"))))
		return E_FAIL;




	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	int iMonsterSettingNum = pGameInstance->Get_Setting_MonsterScene();

	/*if (iMonsterSettingNum == 4)
	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
			return E_FAIL;
	}
	else*/
	{
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_CombatCamera"))))
			return E_FAIL;
	}
	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Layer_Player(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Layer_Monster(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	int iMonsterSettingNum = pGameInstance->Get_Setting_MonsterScene();

	switch (iMonsterSettingNum)
	{
	case 0:
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Skeleton_Naked"))))
			return E_FAIL;
		break;
	case 1:
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Spider_Mana"))))
			return E_FAIL;
		pGameInstance->
			Get_GameObject(LEVEL_COMBAT,
				pLayerTag, TEXT("Spider_Mana"))->Get_Transform()->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(25.84f, 0.f, -4.28f, 1.f));

		static_cast<CSpider_Mana*>(pGameInstance->
			Get_GameObject(LEVEL_COMBAT,
				pLayerTag, TEXT("Spider_Mana")))->Set_buff_Image_Height(-245.f);

		break;

	case 2:
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Skeleton_Naked"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Spider_Mana"))))
			return E_FAIL;
		
		static_cast<CSpider_Mana*>(pGameInstance->
			Get_GameObject(LEVEL_COMBAT,
				pLayerTag, TEXT("Spider_Mana")))->Set_buff_Image_Height(-285.f);
		break;
	case 3:
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_SlimeKing"))))
			return E_FAIL;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Skeleton_Naked"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Spider_Mana"))))
			return E_FAIL;
		

		//static_cast<CSpider_Mana*>(pGameInstance->
		//	Get_GameObject(LEVEL_COMBAT,
		//		pLayerTag, TEXT("Spider_Mana")))->Set_StopIntroSound();

		break;

	case 4:
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Boss_Alumon"))))
			return E_FAIL;

		break;

	default:
		break;
	}


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Layer_UI(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CMeshGround::Ground_ModelDesc ModelNameDesc;
	ZeroMemory(&ModelNameDesc, sizeof(ModelNameDesc));
	
	
	int iMonsterSettingNum = pGameInstance->Get_Setting_MonsterScene();

	switch (iMonsterSettingNum)
	{
	case 0:
		pGameInstance->Load_Object(TEXT("UI_TrunOne"), LEVEL_COMBAT);
		pGameInstance->Load_Object(TEXT("HP_MP_BuffCanvas0_0"), LEVEL_COMBAT);
		break;
	case 1:
		pGameInstance->Load_Object(TEXT("UI_TrunOne_2"), LEVEL_COMBAT);
		pGameInstance->Load_Object(TEXT("HP_MP_BuffCanvas0_1"), LEVEL_COMBAT);
		break;
	case 2:
		pGameInstance->Load_Object(TEXT("UI_TrunTwoMonster"), LEVEL_COMBAT);
		pGameInstance->Load_Object(TEXT("HP_MP_BuffCanvas2"), LEVEL_COMBAT);
		break;
	case 3:
		pGameInstance->Load_Object(TEXT("UI_TrunThree"), LEVEL_COMBAT);
		pGameInstance->Load_Object(TEXT("HP_MP_BuffCanvas3"), LEVEL_COMBAT);
		break;
	case 4:
		//pGameInstance->Load_Object(TEXT("UI_TrunBoss"), LEVEL_COMBAT);
		pGameInstance->Load_Object(TEXT("UI_TrunBossDebug"), LEVEL_COMBAT);
		
		pGameInstance->Load_Object(TEXT("HP_MP_BuffCanvasBoss"), LEVEL_COMBAT);
		break;
	default:
		break;
	}

	if (iMonsterSettingNum != 4)
	{
		lstrcpy(ModelNameDesc.Name, TEXT("Prototype_Component_CombatBaseGround"));
		lstrcpy(ModelNameDesc.TextureName, TEXT("Prototype_Component_Texture_COmbatScene_Forest"));
		ModelNameDesc.iTextureIndex = 4;
		pGameInstance->Load_Object(TEXT("CombatSceneForest1"), LEVEL_COMBAT);
		ModelNameDesc.iShaderPass = 2;
	}
	else
	{
		pGameInstance->Load_Object(TEXT("CombatScene2"), LEVEL_COMBAT);
		lstrcpy(ModelNameDesc.Name, TEXT("Prototype_Component_Ground"));
		lstrcpy(ModelNameDesc.TextureName, TEXT("Prototype_Component_Texture_CombatMapTwo"));
		ModelNameDesc.iTextureIndex = 3;
		ModelNameDesc.iShaderPass = 2;
	}

	pGameInstance->Load_Object(TEXT("UI_Combat_State"), LEVEL_COMBAT);
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_MeshGround"), &ModelNameDesc)))
		return E_FAIL;
		
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Clear_Light();
	LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof(LightDesc));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 0.3f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, TEXT("Level_Combat_Directional"), LightDesc)))
		return E_FAIL;
	
	/*_vector		vLightEye = XMVectorSet(-10.f, 30.f, -10.f, 1.f);
	_vector		vLightAt = XMVectorSet(100.f, 0.f, 100.f, 0.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_matrix		LightViewMatrix;
	LightViewMatrix = XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp);

	_float4x4  m_LightViewMatrix;
	XMStoreFloat4x4(&m_LightViewMatrix, LightViewMatrix);
	_matrix		LightProjMatrix;
	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(120.f), _float(g_iWinSizeX) / _float(g_iWinSizeY), 0.2f, 300.f);

	_float4x4  m_LightProjMatrix;
	XMStoreFloat4x4(&m_LightProjMatrix, LightProjMatrix);

	pGameInstance->Set_LightView_Matrirx(TEXT("Level_Combat_Directional"), m_LightViewMatrix);
	pGameInstance->Set_LightProj_Matrirx(TEXT("Level_Combat_Directional"), m_LightProjMatrix);
*/


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Combat * CLevel_Combat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Combat*		pInstance = new CLevel_Combat(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Combat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Combat::Free()
{
	__super::Free();
}
