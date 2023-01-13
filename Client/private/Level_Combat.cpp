#include "stdafx.h"
#include "..\public\Level_Combat.h"
#include "GameInstance.h"

#include "Level_Loading.h"
#include "Client_Manager.h"
#include "CombatController.h"
#include "PlayerController.h"


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
	
	return S_OK;
}

void CLevel_Combat::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_dCombatIntroTimer += TimeDelta *1.0;

	Combat_Intro();
	__super::Tick(TimeDelta);
	Combat_Control_Tick(TimeDelta);

	


	m_TimeAcc += TimeDelta;

}

void CLevel_Combat::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pCombatController->Late_Tick(TimeDelta);

	if (true == CClient_Manager::m_bCombatWin)
	{
		m_fSceneChaneTimer += _float(TimeDelta);
		m_pCombatController->Render_StopCanvas();
	}
	if(m_fSceneChaneTimer >=5.f)
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		pGameInstance->SceneChange_NameVectorClear();
		m_pCombatController->Scene_Chane_Safe_Release();
		pGameInstance->Set_CopyIndexs(LEVEL_COMBAT,LEVEL_GAMEPLAY);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY), true)))
			return;

		Safe_Release(pGameInstance);

		CClient_Manager::m_bCombatWin = false;
		m_fSceneChaneTimer = 0.f;
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

	if (ImGui::Button("Ultimate_ON"))
	{
		m_pCombatController->Ultimate_Camera_On();
	}
	
	if (ImGui::Button("Ultimate_OFF"))
	{
		m_pCombatController->Ultimate_Camera_Off();
	}


}

void CLevel_Combat::Combat_Intro()
{
	if (m_bIntroFinish)
		return;

	if (m_dCombatIntroTimer >= 0.2f)
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
	

	

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_CombatCamera"))))
		return E_FAIL;

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
		break;

	case 2:
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Skeleton_Naked"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Spider_Mana"))))
			return E_FAIL;
		break;

	case 3:
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_SlimeKing"))))
			return E_FAIL;
		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Skeleton_Naked"))))
			return E_FAIL;

		if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Spider_Mana"))))
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

	pGameInstance->Load_Object(TEXT("CombatSceneForest1"), LEVEL_COMBAT);
	
	
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
	
	default:
		break;
	}

	pGameInstance->Load_Object(TEXT("UI_Combat_State"), LEVEL_COMBAT);
	


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC		LightDesc;
	ZeroMemory(&LightDesc, sizeof(LightDesc));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

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
