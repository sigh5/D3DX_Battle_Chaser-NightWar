#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "GameInstance.h"
#include "UI.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;


	return S_OK;

}

void CLevel_GamePlay::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
		
}

void CLevel_GamePlay::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : GAMEPLAY"));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Tile"))))
		return E_FAIL;
*/
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

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Hero_Gully"))))
	//	return E_FAIL;

	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Hero_Alumon"))))
		return E_FAIL;*/

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring & pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	//
	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_TurnStateCanvas"))))
		return E_FAIL;*/
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
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
