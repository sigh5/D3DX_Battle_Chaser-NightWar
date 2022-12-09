#include "stdafx.h"
#include "..\public\Loader.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Terrain.h"
#include "WaterTile.h"

#include "Hero_Gully.h"
#include "Hero_Garrison.h"
#include "Hero_Calibretto.h"

#include "BackGround.h"
#include "MainLogo.h"

#include "ClientBaseCanvas.h"
#include "TurnUICanvas.h"
#include "TurnStateCanvas.h"
#include "Dungeon_Canvas.h"


#include "TurnCharcterUI.h"
#include "UIButton.h"
#include "HpBar.h"

#include "NoneAnim_BG.h"

#include "Camera_Static.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingThread(void* pArg)
{	
	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());
	
	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOGO:
		pLoader->Loading_ForLogo();
		break;
	case LEVEL_GAMEPLAY:
		pLoader->Loading_ForGamePlay();
		break;
	}
	
	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	/* 로딩을 하기위한 추가적인 흐름을 만든다 (Thread).*/
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingThread, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

/* 로고를 위한 원형을 생성한다. */
HRESULT CLoader::Loading_ForLogo()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));	

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_LogoBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Logo/LogoBackGround.dds")))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Logo/BattleChasersLogo.dds")))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));
	

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));


	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));
	
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGroundLogo"),
		CMainLogo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;	

	Safe_Release(pGameInstance);

	return S_OK;
}

/* 게임플레이를 위한 원형을 생성한다. */
HRESULT CLoader::Loading_ForGamePlay()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Terraindds/MAP_Terrain_%d.dds"), CTexture::TYPE_DIFFUSE, 4))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Brush*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), CTexture::TYPE_BRUSH, 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), CTexture::TYPE_FILTER, 1))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Test_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Filter.dds"), CTexture::TYPE_FILTER, 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Cubedds"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Default0.dds"), CTexture::TYPE_END, 1))))
		return E_FAIL;


	/*나중에 지워야됌*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Base_Canvas"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/TestBaseCanvas.png"), CTexture::TYPE_END, 1))))
		return E_FAIL;

	// Canvas
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_State_Canvas"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/UI_ChoiceBox.png"), CTexture::TYPE_END, 1))))
		return E_FAIL;

	/* For.Prototype_Component_UI_Turn_canvas */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_UI_Turn_canvas"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Init_Bar_BG.png"), CTexture::TYPE_END, 1))))
		return E_FAIL;
	// ~Canvas

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_State_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/StateButton/StateButton_%d.png"), CTexture::TYPE_DIFFUSE, 5))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_UITrunCharUI */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UITrunCharUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/ComBatCharUI/ComBatCharUI_%d.png"), CTexture::TYPE_END, 6))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UITrunCharUI */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DungeonUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_Dungeon//UI_Dungeon_%d.png"), CTexture::TYPE_END, 16))))
		return E_FAIL;

	/*For.Water*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Water_WaterFall"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/FBX/WaterFall/WaterFall_%d.png"), CTexture::TYPE_END,2))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVI_Buffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Ground/Ground9nmp.bmp")))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	
	
	
	/* Model */
	CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("AllModels"), LEVEL_GAMEPLAY);
	/* ~Model */

	
	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNormalTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNormalTex_Phong"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_PongShader.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex_Tile"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_Tile.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));
	
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Hero_Gully */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hero_Gully"),
		CHero_Gully::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Hero_Alumon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hero_Garrison"),
		CHero_Garrison::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Hero_Calibretto */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hero_Calibretto"),
		CHero_Calibretto::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_BaseCanvas */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BaseCanvas"),
		CClientBaseCanvas::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TurnCanvas_UI */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TurnCanvas_UI"),
		CTurnUICanvas::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TurnStateCanvas */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TurnStateCanvas"),
		CTurnStateCanvas::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_DungeonCanvas  */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_DungeonCanvas"),
		CDungeon_Canvas::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_CharUI */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CharUI"),
		CTurnCharcterUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_TurnStateButton */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UIButton"),
		CUIButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_NoneAnim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NoneAnim"),
		CNoneAnim_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterTile"),
		CWaterTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CCamera_Static"),
		CCamera_Static::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	


	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	

	Safe_Release(pGameInstance);

	return S_OK;
}



CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);		
	CloseHandle(m_hThread);
	DeleteObject(m_hThread);
	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);	
}
