#include "stdafx.h"
#include "..\public\Level_Combat.h"
#include "GameInstance.h"

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
	m_dCombatIntroTimer += TimeDelta *1.0;

	Combat_Intro();
	__super::Tick(TimeDelta);
	Combat_Control_Tick(TimeDelta);
}

void CLevel_Combat::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_Combat::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : ComBat"));

	return S_OK;
}

void CLevel_Combat::Combat_Control_Tick(_double TimeDelta)
{
	m_pCombatController->CurrentTurn_ActorControl(TimeDelta);
}

void CLevel_Combat::Combat_Intro()
{
	if (m_bIntroFinish)
		return;

	if (m_dCombatIntroTimer >= 2.f)
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
	
	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Layer_Camera(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	

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

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_SlimeKing"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Skeleton_Naked"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_COMBAT, pLayerTag, TEXT("Prototype_GameObject_Monster_Spider_Mana"))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLevel_Combat::Ready_Layer_UI(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Load_Object(TEXT("UISave"), LEVEL_COMBAT);

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