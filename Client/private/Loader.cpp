#include "stdafx.h"
#include "..\public\Loader.h"
#include "GameInstance.h"
#include "Client_Manager.h"



#include "Level_GamePlayPreHeader.h"
#include "Effect_Point_Instancing.h"
#include "MyImage.h"

/* For.CombatScene*/
#include "SlimeKing.h"
#include "Skeleton_Naked.h"
#include "Spider_Mana.h"

#include "TurnUICanvas.h"
#include "TurnStateCanvas.h"
#include "HpMpBar.h"
#include "CombatMap.h"
#include "Buff_Image.h"


#include "Camera_Combat.h"
#include "Weapon.h"

#include "HpMpBuffCanvas.h"


/* For.CombatScene*/

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
	case LEVEL_COMBAT:
		pLoader->Loading_Combat();
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

	if (FAILED(ForGamePlay_Texture(pGameInstance)))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_HPMPFillBar_BG */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_HPMPFillBar_BG"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/HP_MPFillBar/HP_MPFillBar%d.png"),
			CTexture::TYPE_END, 6))))	
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_CharState */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_CharState"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/CharState/CharState%d.png"),
			CTexture::TYPE_END, 11))))
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

	/* For.Prototype_Component_VIBuffer_Rect_Instancing */					
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instancing"),
		CVIBuffer_Rect_Instancing::Create(m_pDevice, m_pContext, 30))))	//여기서 숫자 결정
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instancing"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 30))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션정보생성중"));
	/* For.Prototype_Component_Navigation */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/Navigation.dat")))))
		return E_FAIL;
	
	lstrcpy(m_szLoadingText, TEXT("스테이터스생성중"));
	/* For.Prototype_Component_Status */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"),
		CStatus::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	/* Model */
	CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("PlayerModels"), LEVEL_GAMEPLAY);
	//CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Map_NESW_C"), LEVEL_GAMEPLAY);
	//CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Map_NESW_A"), LEVEL_GAMEPLAY);
	/* ~Model */

	lstrcpy(m_szLoadingText, TEXT("콜라이더를 로딩중입니다. "));
	/* For.Prototype_Component_Collider_AABB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_OBB*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;
	/* For.Prototype_Component_Collider_SPHERE*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));

	if (FAILED(ForGamePlay_Shader(pGameInstance)))
		return E_FAIL;
	
	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));
	

	/* For.Prototype_GameObject_NoneAnim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	///* For.Prototype_GameObject_Hero_Gully */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hero_Gully"),
		CHero_Knolan::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Hero_Alumon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hero_Garrison"),
		CHero_Garrison::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_Hero_Calibretto */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hero_Calibretto"),
		CHero_Calibretto::Create(m_pDevice, m_pContext))))
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


	/*if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DungeonMaps"),
		CDungeonMaps::Create(m_pDevice, m_pContext))))
		return E_FAIL;*/


	/* For.Prototype_GameObject_NoneAnim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NoneAnim"),
		CNoneAnim_BG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_NoneAnim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DungeonMap"),
		CDungeonMaps::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WaterTile"),
		CWaterTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CCamera_Static"),
		CCamera_Static::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Rect_Instancing"),
		CEffect_Rect_Instancing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Effect_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect_Point_Instancing"),
		CEffect_Point_Instancing::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CCombatMap"),
		CCombatMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MyImage"),
		CMyImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HpMpBars"),
		CHpMpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HPMPBUFFCanvas"),
		CHpMpBuffCanvas::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BuffImage"),
		CBuff_Image::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Combat()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));


	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));


	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
	
	CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Monsters"), LEVEL_COMBAT);

	lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));


	lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_SlimeKing"),
		CSlimeKing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Skeleton_Naked"),
		CSkeleton_Naked::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_Spider_Mana"),
		CSpider_Mana::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CombatCamera"),
		CCamera_Combat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Weapon */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("로딩끝. "));

	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::ForGamePlay_Texture(CGameInstance* pGameInstance)
{
	/* For.Prototype_Component_Texture_Sky */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), CTexture::TYPE_DIFFUSE, 4))))
		return E_FAIL;

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

	// Canvas
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_State_Canvas"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/State_Canvas/UI_ChoiceBox%d.png"), CTexture::TYPE_END, 3))))
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

	/* For.Prototype_Component_Texture_UITrunCharUI */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_COmbatScene_Forest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/CombatScene//BG_Forest_%d.png"), CTexture::TYPE_END, 6))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_UI_State_Icon_Calibretto */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_State_Icon_Calibretto"),
		CTexture::Create(m_pDevice, m_pContext, 
			TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/CombatState/StateIcon/Calibretto/Ability_Calibretto%d.png"), 
			CTexture::TYPE_END, 10))))	//마지막은 궁
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_State_Icon_Garrison */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_State_Icon_Garrison"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/CombatState/StateIcon/Garrison/Ability_Garrison%d.png"),
			CTexture::TYPE_END, 10))))	//마지막은 궁
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_State_Icon_Garrison */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_State_Icon_Knolan"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/CombatState/StateIcon/Knolan/Ability_Knolan%d.png"),
			CTexture::TYPE_END, 18))))	//마지막은 궁
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_State_Icon_Item */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UI_State_Icon_State"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/CombatState/StateIcon/Common/Common_Icon%d.png"),
			CTexture::TYPE_END, 3))))	
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::ForGamePlay_Shader(CGameInstance * pGameInstance)
{
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNormalTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNormalTex_Phong"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_PongShader.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
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

	/* For.Prototype_Component_Shader_VtxRectInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCING_DECLARATION::Elements, VTXPOINTINSTANCING_DECLARATION::iNumElements))))
		return E_FAIL;

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
