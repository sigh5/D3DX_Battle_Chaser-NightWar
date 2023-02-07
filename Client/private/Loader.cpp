#include "stdafx.h"
#include "..\public\Loader.h"
#include "GameInstance.h"
#include "Client_Manager.h"



#include "Level_GamePlayPreHeader.h"
#include "Effect_Point_Instancing.h"
#include "MyImage.h"
#include "EffectFrame.h"
#include "MapOneTree.h"
#include "TrunWinCanvas.h"

/* For.CombatScene*/
#include "SlimeKing.h"
#include "Skeleton_Naked.h"
#include "Spider_Mana.h"

#include "TurnUICanvas.h"
#include "TurnStateCanvas.h"
#include "HpMpBar.h"
#include "CombatMap.h"
#include "Buff_Image.h"
#include "MapOne2D.h"

#include "Camera_Combat.h"

#include "Weapon.h"
#include "Skill_Object.h"

#include "HpMpBuffCanvas.h"
#include "MapTile.h"

#include "ChestBox.h"
#include "Map_3D_UI.h"
#include "Trun_BattleStart_UI.h"
#include "Skill_TextureObj.h"
#include "Buff_Effect.h"
#include "DamageFont.h"
#include "Attack_Effect_Rect.h"
#include "Explain_Font.h"
#include "Debuff_Font.h"
#include "Cam_Effect.h"

#include "TrunLoseCanvas.h"
#include "Exp_Bar.h"
#include "Inventory.h"
#include "Inventory_Button.h"
#include "Broken_Image.h"
#include "Mesh_Effect.h"
#include "Trail_Effect.h"
#include "Traile_Effect_Child.h"
#include "Mouse.h"

#include "Boss_Alumon.h"
#include "MeshGround.h"
#include "Light_Pos.h"

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

	

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_MouseCusor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Mouse_Cusor/CombatCusor_%d.png"), CTexture::TYPE_DIFFUSE, 5))))
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

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse"),
		CMouse::Create(m_pDevice, m_pContext))))
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

	if (false == pGameInstance->Get_IsProtoTypeCreate(LEVEL_GAMEPLAY))
	{
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
		if (FAILED(ForGamePlay_Texture(pGameInstance)))
			return E_FAIL;
		if (FAILED(ForGamePlay_Components(pGameInstance)))
			return E_FAIL;
		lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));
		/* Model */
#ifdef NOMODLES
	
	
#else  
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Battle_Start_Anim_Real"), LEVEL_GAMEPLAY);
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Polygon_Models"), LEVEL_GAMEPLAY);
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("PlayerModels"), LEVEL_GAMEPLAY);
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Skills"), LEVEL_GAMEPLAY);
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("ChestModel_Anim"), LEVEL_GAMEPLAY);
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("MapArts"), LEVEL_GAMEPLAY);
		CClient_Manager::Model_Load_2(m_pDevice, m_pContext, TEXT("MapTree"), LEVEL_GAMEPLAY);
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Monsters"), LEVEL_COMBAT);
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Missile_Model"), LEVEL_GAMEPLAY);
		/* ~Model */

		


#endif
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Light_Pos"),
			CLight_Pos::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		lstrcpy(m_szLoadingText, TEXT("셰이더를 로딩중입니다. "));
		if (FAILED(ForGamePlay_Shader(pGameInstance)))
			return E_FAIL;

		lstrcpy(m_szLoadingText, TEXT("객체원형을 생성중입니다. "));

		if (FAILED(ForGamePlay_GameObjects(pGameInstance)))
			return E_FAIL;

#ifdef _DEBUG
		
#endif // _DEBUG
		


		lstrcpy(m_szLoadingText, TEXT("로딩끝. "));
		pGameInstance->Set_ProtoTypeCreate(LEVEL_GAMEPLAY);
	}
	m_isFinished = true;



	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader::Loading_Combat()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (false == pGameInstance->Get_IsProtoTypeCreate((_uint)LEVEL_COMBAT))
	{
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_COMBAT, TEXT("Prototype_Component_Texture_CombatMapTwo"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Map2/Map2_%d.png"), CTexture::TYPE_DIFFUSE, 9))))
			return E_FAIL;

		lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));

		lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));

		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Alumon_Model"), LEVEL_COMBAT);
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("Ground_Model"), LEVEL_COMBAT); 
		CClient_Manager::Model_Load(m_pDevice, m_pContext, TEXT("CombatMapBase"), LEVEL_COMBAT);


		
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

		/* For.Prototype_GameObject_Skill_Obj */
		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Obj"),
			CSkill_Object::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Alumon"),
			CBoss_Alumon::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		lstrcpy(m_szLoadingText, TEXT("로딩끝."));
		pGameInstance->Set_ProtoTypeCreate(LEVEL_COMBAT);
	}
	else
	{
		CCombatController::GetInstance()->Reset_Timer();
	}
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
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Diffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Terraindds/TextureMap_1/MAP_Terrain_%d.dds"), CTexture::TYPE_DIFFUSE, 35))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Terraindds/TerrainNormal/MAP_Terrain_0_NRM1234.png"), CTexture::TYPE_DIFFUSE))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Tree_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Terraindds/TerrainNormal/MAP_trees_A_01_NRM.png"), CTexture::TYPE_DIFFUSE))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_FilterCanvas"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Filter_2.dds"), CTexture::TYPE_FILTER, 1))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Filter */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Filter"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Filter_%d.dds"), CTexture::TYPE_FILTER, 3))))
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_TurnBattle/Combat/ComBatCharUI/ComBatCharUI_%d.png"), CTexture::TYPE_END, 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UITrunCharUI */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DungeonUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/UI_Dungeon//UI_Dungeon_%d.png"), CTexture::TYPE_END, 16))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UITrunCharUI */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_COmbatScene_Forest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/CombatScene//BG_Forest_%d.png"), CTexture::TYPE_END, 7))))
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

	/* For.Prototype_Component_Texture_UI_State_Icon_Item */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_3DUI_MapColl"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/MapColl/MapColl_%d.png"),
			CTexture::TYPE_END, 2))))
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
	/* For.Prototype_Component_Texture_FireTexture*/
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireTexture"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/FireTexture/fire_loop_sheet.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TrunBattle_Start"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/BattleStart/TrunBattleStart_%d.png"),
			CTexture::TYPE_END, 10))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TrunBattle_Win"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/CombatWin/UI_BattleWin%d.png"),
			CTexture::TYPE_END, 8))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TrunBattle_WinCanvas"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/CombatWin/UI_BattleWinCanvas.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire_Glow"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Glow/blue_fire_4_sheet.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blue_Base"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Glow/blue_fire_m_sheet.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Font"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Font/Font_%d.png"),
			CTexture::TYPE_END, 4))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Ultimate_Banner"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Ultimate_Banner/Ultimate_Banner_%d.png"),
			CTexture::TYPE_END, 8))))
		return E_FAIL;

	
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Combat_ICON"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Buff_ICon/Buff_ICon_%d.png"),
			CTexture::TYPE_END, 5))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Combat_Reward"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Reward/ReWard_%d.png"),		//exp bar 가지고있음
			CTexture::TYPE_END, 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Inventory"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Inventory/Inventory_%d.png"),		//exp bar 가지고있음
			CTexture::TYPE_END, 19))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blocker"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/MapBlocker/MapBlocker_%d.png"),		//exp bar 가지고있음
			CTexture::TYPE_END, 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dissoive"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/dissolve/dissolve_%d.png"),		//exp bar 가지고있음
			CTexture::TYPE_END, 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CombatMapTwo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Map2/Map2_%d.png"), CTexture::TYPE_DIFFUSE, 9))))
		return E_FAIL;


	if (FAILED(ForGamePlay_Skill_and_Effect(pGameInstance)))
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
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_TerrainTile"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_TerrainTile.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModelUVMove"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelUV_Move.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex_Tile"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_Tile.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRectInstance.hlsl"), VTXRECTINSTANCE_DECLARATION::Elements, VTXRECTINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCING_DECLARATION::Elements, VTXPOINTINSTANCING_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Shader_VtxTexIncCount"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTexIncCount.hlsl"), VTXPOINTINSTANCING_DECLARATION::Elements, VTXPOINTINSTANCING_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTrail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTrail.hlsl"), VTXPOINTINSTANCING_DECLARATION::Elements, VTXPOINTINSTANCING_DECLARATION::iNumElements))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModelInstance.hlsl"), VTXMODEL_INSTAICING_DECLARATION::Elements, VTXMODEL_INSTAICING_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPoint.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Broken_Image"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred_Test.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::ForGamePlay_Components(CGameInstance * pGameInstance)
{
	lstrcpy(m_szLoadingText, TEXT("버퍼를 로딩중입니다. "));
	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVI_Buffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Ground/Ground9nmp.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_TerrainTile"),
		CVI_Buffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/MAP_Terrain_Grass_Yellow_A.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Cube */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instancing"),
		CVIBuffer_Rect_Instancing::Create(m_pDevice, m_pContext, 30))))	//여기서 숫자 결정
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Single"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 1))))	//여기서 숫자 결정
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instancing"),
		CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, 30))))
		return E_FAIL;


	/* For.Prototype_Component_VIBuffer_Point_Instancing */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
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


	return S_OK;
}

HRESULT CLoader::ForGamePlay_GameObjects(CGameInstance * pGameInstance)
{
	/* For.Prototype_GameObject_NoneAnim */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SkyBox"),
		CSkyBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/////* For.Prototype_GameObject_Hero_Gully */
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
	/* For.Prototype_GameObject_TurnStateButton */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inventory_Button"),
		CInventory_Button::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	


	/* For.Prototype_GameObject_EffectFrame */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EffectFrame"),
		CEffectFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;
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

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapTile"),
		CMapTile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_2D_MapOne"),
		CMapOne2D::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapOneTree"),
		CMapOneTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ChestBox"),
		CChestBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Map_3D_UI"),
		CMap_3D_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TurnBattleStart_UI"),
		CTrun_BattleStart_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TurnWinCanvas"),
		CTrunWinCanvas::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TrunLoseCanvas"),
		CTrunLoseCanvas::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_TextureObj"),
		CSkill_TextureObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Buff_Effect"),
		CBuff_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DamageFont"),
		CDamageFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Explain_Font"),
		CExplain_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Debuff_Font"),
		CDebuff_Font::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Attack_Effect"),
		CAttack_Effect_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cam_Effect"),
		CCam_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ExpBar"),
		CExp_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inventory_Canvas"),
		CInventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Broken_Image"),
		CBroken_Image::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mesh_Effect"),
		CMesh_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trail_Effect"),
		CTrail_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trail_Effect_Child"),
		CTraile_Effect_Child::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MeshGround"),
		CMeshGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::ForGamePlay_Skill_and_Effect(CGameInstance * pGameInstance)
{

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_laser_Bullet"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Bullet/bretto_laser_Bullet_%d.png"),
			CTexture::TYPE_END, 6))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Heal_Active"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Heal/bretto_Heal_Active_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_heal_spread"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Heal/bretto_heal_spread_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Punch_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Punch/Punch_Effect/Punch_Impact_%d.png"),
			CTexture::TYPE_END, 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Punch_Flash"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Punch/Punch_Flash/Punch_Flash_%d.png"),
			CTexture::TYPE_END, 3))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Punch_Smoke"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Punch/Smoke/calibretto punch_smoke_sheet.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Punch_particles"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Punch/calibretto_punch_particles_sheet.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_smoke"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Smoke/bretto_smoke_%d.png"),
			CTexture::TYPE_END, 3))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Beam"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Beam/bretto_Beam_%d.png"),
			CTexture::TYPE_END, 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Common_Aura"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Common/Arua/Aura_%d.png"),
			CTexture::TYPE_END, 20))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_blue_fire1"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Common/BlueFire/blue_fire_1_sheet_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_garrison_burst"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Garrison/Brust/garrison_burst.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Garrsion_Crack"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Garrison/Crack/Garrsion_Crack_%d.png"),
			CTexture::TYPE_END, 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Garrsion_Fire_bot_Height"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Garrison/Garrsion_Fire_bot/Garrsion_Fire_bot_Height_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Garrsion_Fire_bot_Width"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Garrison/Garrsion_Fire_bot/Garrsion_Fire_bot_Width_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Garrsion_knolan_crack"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Knolan/Crack/knolan_crack_%d.png"),
			CTexture::TYPE_END, 6))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireBall_Knolan"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Knolan/FireBall/Knolan_FireBall.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_IceBlast_Knolan"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Knolan/Ice_Blast/knolan_iceblast.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Knolan_Brust"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Knolan/Brust/Knolan_Brust_%d.png"),
			CTexture::TYPE_END, 6))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bite"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Monster/Monster_Bite/bite_new_%d.png"),
			CTexture::TYPE_END, 5))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Monster_Bite_Impact"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Monster/Monster_Bite_Impact/bite_impact_%d.png"),
			CTexture::TYPE_END, 3))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Common_Fire_Exhurst"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Common/FireExhust/FireExhust_%d.png"),
			CTexture::TYPE_END, 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Knolan_Fire_Meteo"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Knolan/FierMeteo/Fire_Meteo.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Common_Hit_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Common/HItEffect/Hit_Effect_%d.png"),
			CTexture::TYPE_END, 12))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Buff_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Buff/Buff_%d.png"),
			CTexture::TYPE_END, 20))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bleeding_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Bleeding_Effect/Bleeding_Effect_%d.png"),
			CTexture::TYPE_END, 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Jump_Left_To_Right"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Common/Jump/Left_to_Right/Left_to_Right_%d.png"),
			CTexture::TYPE_END, 6))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Jump_Right_To_Left"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Common/Jump/Right_to_Left/Right_to_Left_%d.png"),
			CTexture::TYPE_END, 6))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Defence_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Defece_Effect/Defece_Effect_%d.png"),
			CTexture::TYPE_END, 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Die_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Die_Effect/Die_Effect_%d.png"),
			CTexture::TYPE_END, 5))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Garrison_Ultimate_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Garrison/Ultimate_Effect/Ultimate_Effect_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Garrison_Burn_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Garrison/Burn/Garrison_Burn_%d.png"),
			CTexture::TYPE_END, 9))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Matrix_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/bretto_Matrix/bretto_Matrix_%d.png"),
			CTexture::TYPE_END, 7))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Real_Bullet_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Bullet/Bullet_%d.png"),
			CTexture::TYPE_END, 5))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Lazer_Ultimate_Rect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_RECT/Lazor/Lazer_%d.png"),
			CTexture::TYPE_END, 8))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_bretto_Glow_Ultimate_Rect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_RECT/Lazor_Glow/Glow_%d.png"),
			CTexture::TYPE_END, 8))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DeBuff_Effect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/DeBuff/DeBuff_%d.png"),
			CTexture::TYPE_END, 8))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Knolan_Golf"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Knolan/Skill_Golf/Skill_Golf_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Knolan_Ultimage_CamEffect"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Skill_Effect/Knolan/Ultimate_Effect/Ultimate_Effect_0.png"),
			CTexture::TYPE_END, 1))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Slime_Ultimate_paricle"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Breath/bubble_pop_particles_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Slime_Ultimate_Bubble"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Breath/bubble_pop_%d.png"),
			CTexture::TYPE_END, 12))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Slime_Ultimate_BubbleBase"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Bubble_Base/bubble_pop_0_%d.png"),
			CTexture::TYPE_END, 16))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Slime_Ultimate_BubbleOrigin"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Bubble_Base/Bubble_7/bubble_pop_7_%d.png"),
			CTexture::TYPE_END, 16))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Slime_Ultimate_BubbleOrigin2"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Bubble_Base/Bubble_6/bubble_pop_6_%d.png"),
			CTexture::TYPE_END, 16))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dust"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Dust_Effect/Dust_%d.png"),
			CTexture::TYPE_END, 6))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Auro03"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Auro03/Aura03_Red_%d.png"),
			CTexture::TYPE_END, 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Auro04"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Auro04/Aura04_Red_%d.png"),
			CTexture::TYPE_END, 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UltiCam"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Cam/Cam_Effect_%d.png"),
			CTexture::TYPE_END, 6))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UltiCam_Sprites"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Cam/Sprites/Sprites_%d.png"),
			CTexture::TYPE_END, 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("../Bin/Resources/Textures2D/Trail/Trail_%d.png"),
			CTexture::TYPE_END, 11))))
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
