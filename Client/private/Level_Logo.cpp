#include "stdafx.h"
#include "..\public\Level_Logo.h"

#include "Level_Loading.h"
#include "GameInstance.h"
#include "Imgui_PropertyEditor.h"
#include "Logo_Button.h"

CLevel_Logo::CLevel_Logo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Logo(TEXT("Layer_Logo"))))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiTabObject(CImgui_PropertyEditor::Create());

	CGameInstance::GetInstance()->Play_Sound(TEXT("01_Battle_Chasers_Theme.wav"), 0.5f, true, SOUND_BGM);
	
	CGameInstance::GetInstance()->Load_SoundFile("../Bin/Resources/Sound/Monsters_Sound/", TEXT("Monster_"));
	CGameInstance::GetInstance()->Load_SoundFile("../Bin/Resources/Sound/Kalibretto_Sound/", TEXT("Calibretto_"));
	CGameInstance::GetInstance()->Load_SoundFile("../Bin/Resources/Sound/GarrisonSound/", TEXT("Garrison_"));
	CGameInstance::GetInstance()->Load_SoundFile("../Bin/Resources/Sound/Knolan_Sound/", TEXT("Knolan_"));
	CGameInstance::GetInstance()->Load_SoundFile("../Bin/Resources/Sound/CommonSound/", TEXT("Common_"));

	
	//pGameInstace->Load_SoundFile("../Bin/Resources/Sound/Effect_Sound/");

	//ShowCursor(false);

	return S_OK;
}

void CLevel_Logo::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	pGameStartButton->Tick((TimeDelta));
		
}

void CLevel_Logo::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	pGameStartButton->Late_Tick((TimeDelta));

#ifdef _DEBUG
	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		pGameInstance->Stop_Sound(SOUND_BGM);
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;

		Safe_Release(pGameInstance);

	}
#else
	if (static_cast<CLogo_Button*>(pGameStartButton)->Get_GameStart())
	{
		CGameInstance*		pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		pGameInstance->Stop_Sound(SOUND_BGM);
		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
			return;

		Safe_Release(pGameInstance);
}

#endif // _DEBUG







}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : LOGO"));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const wstring& pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_BackGround"))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_Logo(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_BackGroundLogo"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Mouse"))))
		return E_FAIL;

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_LogoButton"))))
	//	return E_FAIL;

	pGameStartButton = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_LogoButton"));


	Safe_Release(pGameInstance);

	return S_OK;
}



CLevel_Logo * CLevel_Logo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Logo*		pInstance = new CLevel_Logo(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
	Safe_Release(pGameStartButton);
}
