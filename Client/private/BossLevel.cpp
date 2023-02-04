#include "stdafx.h"
#include "..\public\BossLevel.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Client_Manager.h"

#include "PlayerController.h"
#include "TrunWinCanvas.h"

#include "SoundPlayer.h"
#include "CombatController.h"

CBossLevel::CBossLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
	, m_pCombatController(CCombatController::GetInstance())
{
}

HRESULT CBossLevel::Initialize()
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

void CBossLevel::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_dCombatIntroTimer += TimeDelta *1.0;

	CombatBoss_Intro();
	__super::Tick(TimeDelta);
	CombatBoss_Control_Tick(TimeDelta);
	CSoundPlayer::GetInstance()->Tick(TimeDelta);

	m_TimeAcc += TimeDelta;
}

void CBossLevel::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	m_pCombatController->Late_Tick(TimeDelta);

}

HRESULT CBossLevel::Render()
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

HRESULT CBossLevel::Ready_Change_SceneData()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Change_Level(LEVEL_COMBAT_BOSS);
	CPlayerController::GetInstance()->Change_Scene(LEVEL_COMBAT_BOSS);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBossLevel::Ready_Layer_BackGround(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT_BOSS, pLayerTag, TEXT("Prototype_GameObject_SkyBox"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBossLevel::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT_BOSS, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBossLevel::Ready_Layer_Player(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/**/

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBossLevel::Ready_Layer_Monster(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);




	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBossLevel::Ready_Layer_UI(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);




	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBossLevel::Ready_Lights()
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

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, TEXT("Level_BOSS_Directional"), LightDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBossLevel::CombatBoss_Control_Tick(_double TimeDelta)
{
	m_pCombatController->CurrentTurn_ActorControl(TimeDelta);
}

void CBossLevel::CombatBoss_Intro()
{
	if (m_bIntroFinish)
		return;

	if (m_dCombatIntroTimer >= 0.15f)
	{
		m_pCombatController->Set_CombatIntro(true);
		m_bIntroFinish = true;
	}
}


CBossLevel * CBossLevel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBossLevel*		pInstance = new CBossLevel(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CBossLevel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossLevel::Free()
{
	__super::Free();
}
