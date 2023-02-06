#include "stdafx.h"
#include "..\public\Hero_Calibretto.h"

#include "GameInstance.h"
#include "Client_Manager.h"

#include "AnimFsm.h"
#include "PlayerController.h"
#include "CombatController.h"

#include "Buff_Effect.h"
#include "Skill_TextureObj.h"
#include "Damage_Font_Manager.h"
#include "Attack_Effect_Rect.h"
#include "Explain_FontMgr.h"
#include "SoundPlayer.h"

CHero_Calibretto::CHero_Calibretto(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice, pContext)
{
}

CHero_Calibretto::CHero_Calibretto(const CHero_Calibretto & rhs)
	: CPlayer(rhs)
{
}

CGameObject * CHero_Calibretto::Get_Weapon_Or_SkillBody()
{
	for (auto& pParts : m_PlayerParts)
	{
		if (dynamic_cast<CHitBoxObject*>(pParts) != nullptr && m_eWeaponType == dynamic_cast<CHitBoxObject*>(pParts)->Get_Type())
		{
			static_cast<CHitBoxObject*>(pParts)->Set_WeaponOption(m_iWeaponOption);
			static_cast<CHitBoxObject*>(pParts)->Set_WeaponDamage(m_iStateDamage);
			static_cast<CHitBoxObject*>(pParts)->Set_HitNum(m_iHitCount);
			return pParts;
		}
	}

	return nullptr;
}

_bool CHero_Calibretto::Calculator_HitColl(CGameObject * pWeapon)
{
	CHitBoxObject* pCurActorWepon = static_cast<CHitBoxObject*>(pWeapon);
	assert(pCurActorWepon != nullptr && "CSkeleton_Naked::Calculator_HitColl");

	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{
		m_iHitWeaponOption = static_cast<CHitBoxObject::WEAPON_OPTIONAL>(pCurActorWepon->Get_WeaponOption());
		m_iGetDamageNum = pCurActorWepon->Get_WeaponDamage();
		
		if (pCurActorWepon->Get_HitNum() > 1)
		{
			m_bIs_Multi_Hit = true;
			m_bOnceCreate = false;
		}		
		Calculator_HitDamage();
		return true;
	}
	return false;
}

_int CHero_Calibretto::Get_RestHpPotion()
{
	return m_pStatusCom[DUNGEON_PLAYER]->Rest_iTemNum(CStatus::ITEM_HP_POTION);
}

_int CHero_Calibretto::Get_RestMpPotion()
{
	return m_pStatusCom[DUNGEON_PLAYER]->Rest_iTemNum(CStatus::ITEM_MP_POSION);
}

HRESULT CHero_Calibretto::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHero_Calibretto::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Hero_Calibretto");

	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);
	GameObjectDesc.TransformDesc.fSpeedPerSec = 1.5f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimIndex(0);
	return S_OK;
}

HRESULT CHero_Calibretto::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.18f, 0.f, 0.18f, 1.f));
	m_pTransformCom->Set_Scaled(_float3(0.1f, 0.1f, 0.1f));
	
	_float4 vPos;
	XMStoreFloat4(&vPos, XMVectorSet(0.18f, 0.f, 0.18f, 1.f));
	m_pNavigationCom->Set_OldPos(vPos);
	m_bLast_Initlize = true;
	return S_OK;
}

void CHero_Calibretto::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	if (m_bIsCombatScene == false)
	{
		LookAtTarget(TimeDelta, m_pNavigationCom);
		Dungeon_Tick(TimeDelta);
	}
		
	else
	{
		Combat_Initialize();
		m_pFsmCom->Tick(TimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	
		if (m_pStatusCom[COMBAT_PLAYER]->Get_Dead())
		{
			m_bIsDead = true;
		}
		else
		{
			for (auto& pBuffImage : m_vecBuffImage)
			{
				if (pBuffImage != nullptr)
					pBuffImage->Tick(TimeDelta);
			}
		}

		if (nullptr != m_pFog)
			m_pFog->Tick(TimeDelta);
		
		if (nullptr != m_pUltimateEffect)
			m_pUltimateEffect->Tick(TimeDelta);
		

		if (nullptr != m_pFullscreenEffect)
			m_pFullscreenEffect->Tick(TimeDelta);

	
		//static float ffPos[3] = {};
		//static float ffScale[3] = {};
		//static char  szName[MAX_PATH] = "";
		//ImGui::InputFloat3("SkillPos", ffPos);
		//ImGui::InputFloat3("SkillScale", ffScale);

		//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//ImGui::InputText("TextureName", szName, MAX_PATH);

		////if (ImGui::Button("Create_Skill"))
		////{
		////	_tchar Texture_NameTag[MAX_PATH] = TEXT("");
		////	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName) + 1, Texture_NameTag, MAX_PATH);

		////	m_TextureTag = Texture_NameTag;
		////	m_vSkill_Pos = _float4(ffPos[0], ffPos[1], ffPos[2], 1.f);
		////	m_vTestScale = _float3(ffScale[0], ffScale[1], ffScale[2]);

		////	Create_Test_TextureObj();		// Test

		////}
		//if (ImGui::Button("Create_Effect"))
		//{
		//	_tchar Texture_NameTag[MAX_PATH] = TEXT("");
		//	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName) + 1, Texture_NameTag, MAX_PATH);

		//	m_TextureTag = Texture_NameTag;
		//	m_vSkill_Pos = _float4(ffPos[0], ffPos[1], ffPos[2], 1.f);
		//	m_vTestScale = _float3(ffScale[0], ffScale[1], ffScale[2]);

		//	Create_Test_Effect();		// Test
		//}

		//RELEASE_INSTANCE(CGameInstance);

	
	}

	m_pModelCom->Play_Animation(TimeDelta, m_bIsCombatScene);
}

void CHero_Calibretto::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);




	if(m_bModelRender)
	{ 
		for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
		{
			if ((*iter) != nullptr)
				(*iter)->Late_Tick(TimeDelta);

			if (true == static_cast<CBuff_Effect*>(*iter)->Get_IsFinish())
			{
				Safe_Release(*iter);
				iter = m_pEffectParts.erase(iter);
			}
			else if (m_bUltimateStop == true && true == static_cast<CAttack_Effect_Rect*>(*iter)->Get_MainTain())
			{
				Safe_Release(*iter);
				iter = m_pEffectParts.erase(iter);
			}
			else if (m_bBuffEffectStop == true && true == static_cast<CBuff_Effect*>(*iter)->Get_MainTain())
			{
				Safe_Release(*iter);
				iter = m_pEffectParts.erase(iter);
			}

			else
				++iter;
		}

		if (nullptr != m_pLazorEffect)
		{
			m_pLazorEffect->Late_Tick(TimeDelta);

			if (true == m_bLazorStop && true == static_cast<CBuff_Effect*>(m_pLazorEffect)->Get_MainTain())
			{
				Safe_Release(m_pLazorEffect);
				m_pLazorEffect = nullptr;
			}
		}
			
		if (m_bIsCombatScene == true )
		{
			if (!m_bIsDead && false == m_bUltimateBuffRenderStop && CClient_Manager::m_bCombatWin == false)
			{
				for (auto &pParts : m_PlayerParts)
				{
					pParts->Late_Tick(TimeDelta);
				}

				CClient_Manager::Sort_BuffImage(m_vecBuffImage, true);
				for (auto& pBuffImage : m_vecBuffImage)
				{
					if (pBuffImage != nullptr)
						pBuffImage->Late_Tick(TimeDelta);
				}
				CClient_Manager::Delete_BuffImage(m_vecBuffImage, m_pStatusCom[COMBAT_PLAYER], false);
			}
		}

		if (nullptr != m_pFog)
		{
			m_pFog->Late_Tick(TimeDelta);
			if (static_cast<CBuff_Effect*>(m_pFog)->Get_IsFinish())
			{
				Safe_Release(m_pFog);
				m_pFog = nullptr;
			}
		}

		if (nullptr != m_pUltimateEffect)
			m_pUltimateEffect->Late_Tick(TimeDelta);

		if (nullptr != m_pFullscreenEffect)
		{
			m_pFullscreenEffect->Late_Tick(TimeDelta);
			if (static_cast<CBuff_Effect*>(m_pFullscreenEffect)->Get_IsFinish())
			{
				Safe_Release(m_pFullscreenEffect);
				m_pFullscreenEffect = nullptr;
			}
		}
	}

	if (m_bModelRender	&& nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
#ifdef _DEBUG
		CClient_Manager::Collider_Render(this, m_pColliderCom, m_pRendererCom);
		CClient_Manager::Navigation_Render(this, m_pNavigationCom, m_pRendererCom);
		if (m_bIsCombatScene)
			m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
#endif
	
	}
}

HRESULT CHero_Calibretto::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_bIsCombatScene)
	{
		m_pModelCom->Set_NoRenderMeshIndex(10);	//  베이스무기  그리지말기
		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (_uint i = m_iNonRenderMeshIndex; i < iNumMeshes; ++i)
		{
			if (i == 11)
				continue;
			if (i == 12)
				continue;
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices", "DN_FR_FishingRod");
		}
	}
	else
	{
		m_pModelCom->Set_NoRenderMeshIndex(2);

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (_uint i = m_iNonRenderMeshIndex; i < iNumMeshes; ++i)
		{
			m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
			m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices", "DN_FR_FishingRod");
		}
	}

	return S_OK;
}

HRESULT CHero_Calibretto::Render_ShadowDepth()
{
	if (m_bIsCombatScene == false)
		return S_OK;

	if (FAILED(__super::Render()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Set_Matrix("g_matLightView", &CGameInstance::GetInstance()->Get_Light_Matrix(TEXT("Level_Combat_Directional")));
	m_pShaderCom->Set_Matrix("g_matLightProj", &CGameInstance::GetInstance()->Get_Light_ProjMatrix(TEXT("Level_Combat_Directional")));

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = m_iNonRenderMeshIndex; i < iNumMeshes; ++i)
	{
		if (i == 11)
			continue;
		if (i == 12)
			continue;

		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, 1, "g_BoneMatrices", "DN_FR_FishingRod");
	}
	return S_OK;
}


void CHero_Calibretto::Change_Level_Data(_uint iLevleIdx)
{
	CGameInstance *pGameInstance = GET_INSTANCE(CGameInstance);

	Safe_Release(m_pModelCom);
	Remove_component(TEXT("Com_Model"));
	if (LEVEL_GAMEPLAY == iLevleIdx)
	{
		_float3 vScale = m_pStatusCom[DUNGEON_PLAYER]->Get_DungeonScale();
		_float4 vPos = m_pStatusCom[DUNGEON_PLAYER]->Get_DungeonPos();
		
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		m_pTransformCom->Set_Scaled(vScale);
		m_pTransformCom->Set_TransfromDesc(1.5f, 90.f);

		m_iNonRenderMeshIndex = 0;
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CalibrettoDungeon"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert(! "CHero_Calibretto Change_Level_Data : LEVEL_COMBAT ");

		m_bIsCombatScene = false;

	}
	else if (LEVEL_COMBAT == iLevleIdx)
	{
	
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float3 vScale = m_pTransformCom->Get_Scaled();
		m_pStatusCom[DUNGEON_PLAYER]->Set_Dungeon_PosScale(vPos, vScale);
		m_pTransformCom->Set_TransfromDesc(7.f, 90.f);
	
		if (m_bCombat_LastInit)
		{
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
			vPos = m_pStatusCom[COMBAT_PLAYER]->Get_CombatPos();
			vScale = m_pStatusCom[COMBAT_PLAYER]->Get_CombatScale();
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
			m_pTransformCom->Set_Scaled(vScale);
		}
	

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CalibrettoCombat"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert(! "CHero_Calibretto Change_Level_Data : LEVEL_COMBAT ");
		m_bIsCombatScene = true;
		
	}
	else
		return;


	RELEASE_INSTANCE(CGameInstance);
}

_uint CHero_Calibretto::Get_AnimationIndex()
{
	return m_pModelCom->Get_AnimIndex();
}

void CHero_Calibretto::AnimMove()
{
	m_pModelCom->Set_AnimIndex(m_iAnimIndex);
}

void CHero_Calibretto::HighLightChar()
{
	HIGHLIGHT_UIDESC Desc;
	ZeroMemory(&Desc, sizeof HIGHLIGHT_UIDESC);

	Desc.iNumSizeX = 133.f;
	Desc.iNumSizeY = 134.f;;
	Desc.yPos = 5.f;
	Desc.iTextureIndex = 8;
	lstrcpy(Desc.szObjectName, TEXT("UIButton4"));
	m_Hero_DungeonUIDelegeter.broadcast(Desc);

}

void CHero_Calibretto::NormalLightCharUI()
{
	HIGHLIGHT_UIDESC Desc;
	ZeroMemory(&Desc, sizeof HIGHLIGHT_UIDESC);

	Desc.iNumSizeX = 133.f;
	Desc.iNumSizeY = 123.f;
	Desc.yPos = 0.f;
	Desc.iTextureIndex = 5;
	lstrcpy(Desc.szObjectName, TEXT("UIButton4"));
	m_Hero_DungeonUIDelegeter.broadcast(Desc);
}

void CHero_Calibretto::Dungeon_Tick(_double TimeDelta)
{
	if (IsCaptin() && !CPlayer::KeyInput(TimeDelta, m_pNavigationCom))
		m_iAnimIndex = 0;
	CGameInstance*pGameInstance = GET_INSTANCE(CGameInstance);

	if (false == m_bIsWalk && m_bRun == true)
	{
		m_fWalkSoundTimer += (_float)TimeDelta;
		if (m_fWalkSoundTimer >= 0.3f)
		{
			pGameInstance->Stop_Sound(SOUND_CALIBRETTO_EFFECT);
			pGameInstance->Play_Sound(TEXT("Calibretto_0020.wav"), 0.6f, false, SOUND_CALIBRETTO_EFFECT);
			m_fWalkSoundTimer = 0.f;
		}
	}
	else
		pGameInstance->Stop_Sound(SOUND_CALIBRETTO_EFFECT);
	
	AnimMove();
	CClient_Manager::CaptinPlayer_ColiderUpdate(this, m_pColliderCom, m_pTransformCom);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Anim_Frame_Create_Control()
{
	if (!m_bCreateSmoke && m_pModelCom->Control_KeyFrame_Create(1, 22))
	{
		Create_Skill1_Bullet_End();
		m_bCreateSmoke = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(7, 52))
	{
		Create_Buff_Effect();
		CCombatController::GetInstance()->Camera_Zoom_Out();
		m_bOnceCreate = true;
	}
	else if (!m_bOnceCreateBuff && m_pModelCom->Control_KeyFrame_Create(7, 90))
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.x -= 2.f;
		vPos.y += 9.f;
		CExplain_FontMgr::GetInstance()->Set_Explain_Target2_Font0(vPos,
			_float3(1.f, 1.f, 1.f), TEXT("damage up"));

		Create_Buff_MainTain_Effect();
		m_bOnceCreateBuff = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(12, 4))
	{
		CCombatController::GetInstance()->Camera_Shaking();
		Create_Hit_Effect();
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.x -= 1.f;
		vPos.y += 9.f;
		_int iRandom = rand() % 5;
		CDamage_Font_Manager::GetInstance()->Set_Damage_Target2_Font(vPos, _float3(2.f, 2.f, 2.f), iRandom + m_iGetDamageNum, 1.5f, 1.2f);
		m_bOnceCreate = true;

		m_iMultiHitNum = 0;
	}
	else if (!m_bRun && m_pModelCom->Control_KeyFrame_Create(15, 1) )
	{
		Create_Move_Target_Effect();
		m_bRun = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(16, 20))
	{
		Create_Normal_Attack_Effect();
		m_bOnceCreate = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(25, 12) )
	{
		Create_Skill1_Attack_Effect();
		m_bOnceCreate = true;
	}
	else if (!m_bBulletShoot && m_pModelCom->Control_KeyFrame_Create(25, 38))
	{
		Create_Skill1_Bullet();
		m_bBulletShoot = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(28, 3))
	{
		Create_Skill2_Beam();
		m_bOnceCreate = true;
	}
	else if (!m_bLazorStop && m_pModelCom->Control_KeyFrame_Create(28, 58))
	{
		m_bLazorStop = true;

		if(nullptr != m_pHitTarget)
			CCombatController::GetInstance()->Wide_Attack(false, 86);
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(40, 50))
	{
		Create_WideBuffEffect();
		m_bOnceCreate = true;
	}
	else if (!m_bRun && m_pModelCom->Control_KeyFrame_Create(40, 90))	
	{
		Create_Wide_BuffEffect_Second();
		m_bRun = true;
	}
	else if (!m_bFogStart && m_pModelCom->Control_KeyFrame_Create(46, 3))
	{
		Create_Ultimate_StartFog_CamEffect();
		Create_Ultimate_StartCam_Effect();
		CCombatController::GetInstance()->Load_CamBG2();
		m_bFogStart = true;
	}
	else if (!m_bRun && m_pModelCom->Control_KeyFrame_Create(46, 200))	
	{
		Safe_Release(m_pFog);
		Safe_Release(m_pUltimateEffect);
		m_pFog = nullptr;
		m_pUltimateEffect = nullptr;
		m_bUltimateBuffRenderStop = false;
		CCombatController::GetInstance()->Set_Ultimate_End(true);
		CCombatController::GetInstance()->Camera_Zoom_In();
		m_bRun = true;
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 10.f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));

	}	
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(46, 210))	
	{
		m_bOnceCreate = true;
		Create_Skill_Ultimate_Effect0();
		CGameInstance::GetInstance()->Set_Timedelta(TEXT("Timer_60"), 0.5f);
	}
	else if (!m_bUltimateStop && m_pModelCom->Control_KeyFrame_Create(46, 250))	
	{
		m_bUltimateStop = true;
		CGameInstance::GetInstance()->Set_Timedelta(TEXT("Timer_60"), 1.0f);
		Create_FullScreenEffect();
	}
	else if (m_bUltimateStop && m_pModelCom->Control_KeyFrame_Create(46, 260))	
	{
		Create_Ultimate_End_Effect();
		CCombatController::GetInstance()->Wide_Attack(false,50);
		
		m_bUltimateStop = false;
	}
	else if (!m_bRecoverHeight && m_pModelCom->Control_KeyFrame_Create(46, 400))	// 대신 쓴 변수
	{
		m_bRecoverHeight = true;
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y -= 10.f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
	}
	else
		return;
}

HRESULT CHero_Calibretto::Combat_Initialize()
{
	if (m_bCombat_LastInit)
		return S_OK;

	m_pFsmCom = CAnimFsm::Create(this, ANIM_CHAR3);

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
	m_pTransformCom->Set_Scaled(_float3(4.f, 4.f, 4.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(7.f, 0.f, 38.f, 1.f));
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_pTransformCom->Set_TransfromDesc(7.f, 90.f);


	if (FAILED(Ready_Parts_Combat()))
		return E_FAIL;
	
	_float4 vPos;
	XMStoreFloat4(&vPos, XMVectorSet(7.f, 0.f, 38.f, 1.f));
	_float3 vScale = _float3(4.f, 4.f, 4.f);
	m_pStatusCom[COMBAT_PLAYER]->Set_Combat_PosScale(vPos, vScale);

	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_HP_POTION, 10);

	
	m_bDefence = false;
	m_isWideBuff = true;

	m_bCombat_LastInit = true;
	return S_OK;
}

void CHero_Calibretto::Combat_Tick(_double TimeDelta)
{
	Is_MovingAnim();
	CombatAnim_Move(TimeDelta);

	if (bResult == ANIM_DIR_STRAIGHT || bResult == ANIM_DIR_BACK)
	{
		MovingAnimControl(TimeDelta);
	}
	else
		CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);

	Anim_Frame_Create_Control();

	if (nullptr != m_pLazorEffect)
		m_pLazorEffect->Tick(TimeDelta);

	for (auto &pEffect : m_pEffectParts)
		pEffect->Tick(TimeDelta);

	for (auto& pParts : m_PlayerParts)
		pParts->Tick(TimeDelta);


}

void CHero_Calibretto::Combat_DeadTick(_double TimeDelta)
{
	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
}

HRESULT CHero_Calibretto::SetUp_Components()
{

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CalibrettoDungeon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(2.0f, 2.5f, 1.2f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIDESC));

	NaviDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
		(CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	StatusDesc.iHp = 400;
	StatusDesc.iMp = 100;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusDungeon"),
		(CComponent**)&m_pStatusCom[DUNGEON_PLAYER], &StatusDesc)))
		return E_FAIL;

	/*m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_HP_POTION, 20);
	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_MP_POSION, 10);*/


	return S_OK;
}

HRESULT CHero_Calibretto::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CHero_Calibretto::Ready_Parts_Dungeon()
{
	return S_OK;
}

HRESULT CHero_Calibretto::Ready_Parts_Combat()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();

	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Calibretto_Hand_R");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(1.7f, 0.f, -1.1f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(0.5f, 3.f, 0.5f, 0.f));
	WeaponDesc.eType = WEAPON_HAND;
	WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PUNCH_HIT;

	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);


	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	ZeroMemory(&StatusDesc, sizeof(CStatus::StatusDesc));
	StatusDesc.iHp = 10000;
	StatusDesc.iMp = 250;
	StatusDesc.iExp = 0;
	StatusDesc.iLevel = 1;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusCombat"),
		(CComponent**)&m_pStatusCom[COMBAT_PLAYER], &StatusDesc)))
		return E_FAIL;




	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CHero_Calibretto::Anim_Idle()
{
	m_CurAnimqeue.push({ 2,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Intro()
{
	m_iNonRenderMeshIndex = 0;
	m_CurAnimqeue.push({ 17, m_IntroTimer });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::AnimNormalAttack()
{
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStateDamage = rand() % 10 + 20 + m_iStage_Buff_DamgaeUP;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_HAND;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PUNCH_HIT;
	m_bOnceCreate = false;
	m_bRun = false;

	m_SpeedRatio = 8.f;
	m_LimitDistance = 8.f;
	m_ReturnDistance = 0.4f;
	m_setTickForSecond = 0.9f;
	m_iStage_Buff_DamgaeUP = 0;
	m_bBuffEffectStop = true;

	m_CurAnimqeue.push({ 15, 1.f });
	m_CurAnimqeue.push({ 3,	 1.f });
	m_CurAnimqeue.push({ 16, 1.f });
	m_CurAnimqeue.push({ 9,  1.f });
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Skill1_Attack()
{
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStateDamage = rand() % 10 + 15 + m_iStage_Buff_DamgaeUP;		// 펀치하고 총갈기기
	m_iHitCount = 2;
	m_eWeaponType = WEAPON_HAND;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PUNCH_GUN;
	m_bOnceCreate = false;
	m_bRun = false;
	m_bBulletShoot = false;
	m_bCreateSmoke = false;

	m_SpeedRatio = 6.f;
	m_LimitDistance = 8.f;
	m_ReturnDistance = 0.4f;
	m_setTickForSecond = 0.9f;
	m_iStage_Buff_DamgaeUP = 0;
	m_bBuffEffectStop = true;
	//m_vSkill_Pos= 
	//

	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(30);
	m_CurAnimqeue.push({ 15, 1.f });
	m_CurAnimqeue.push({ 3,	 1.f });
	m_CurAnimqeue.push({ 25, 1.f });		
	m_CurAnimqeue.push({ 9,  1.f });
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Skill2_Attack()
{
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStateDamage = rand() % 10 + 50 + m_iStage_Buff_DamgaeUP;				//레이저빔
	m_SpeedRatio = 8.f;
	m_LimitDistance = 20.f;
	m_ReturnDistance = 0.4f;
	m_setTickForSecond = 0.9f;
	m_iStateDamage = 20;			// 20*2 
	m_bOnceCreate = false;
	m_bRun = false;
	m_bBulletShoot = false;
	m_bCreateSmoke = false;
	m_iStage_Buff_DamgaeUP = 0;
	m_bLazorStop = false;
	m_bBuffEffectStop = true;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_CurAnimqeue.push({ 28, 1.f });		// 찐 공격궁 Brust??
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Uitimate()
{
	m_bUltimateBuffRenderStop = true;
	m_bFogStart = false;
	m_bFullScreenEffect = false;
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_bUltimateCam = false;
	m_bBuffEffectStop = true;
	m_iStateDamage = 50;			// 20*2 
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(100);
	m_bOnceCreate = false;
	m_bRun = false;
	m_bIsUseUltimate = false;
	m_bRecoverHeight = false;
	m_bUltimateStop = false;
	
	m_CurAnimqeue.push({ 46, 1.f }); // 위에서 로켓쏘기
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Buff()
{
	CClient_Manager::Create_BuffImage(m_vecBuffImage,
		_float4(-305.f, -330.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
		TEXT("Prototype_GameObject_BuffImage"), 0);
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, true);


	m_iStage_Buff_DamgaeUP = 10;
	m_bOnceCreate = false;
	m_bOnceCreateBuff = false;
	m_bBuffEffectStop = false;

	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_CurAnimqeue.push({ 7,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_WideAreaBuff()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(50);
	m_CurAnimqeue.push({ 40, 1.f });		// 팀원 대폭힐			
	m_CurAnimqeue.push({ 1,  1.f });
	
	m_bOnceCreate = false;
	m_bRun = false;
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Use_Item()
{
	m_CurAnimqeue.push({ 19,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Light_Hit()
{
	if (m_bIsHeavyHit)
	{
		m_CurAnimqeue.push({ 11, 1.f }); //,10
		m_bIsHeavyHit = false;
	}
	else if (true == m_bIs_Multi_Hit)
	{
		m_CurAnimqeue.push({ 4,  1.f });
		m_CurAnimqeue.push({ 12,  1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_bOnceCreate = false;
		m_bIs_Multi_Hit = false;

		m_iMultiHitNum = 2;
	}
	else 
	{
		m_CurAnimqeue.push({ 5 , 1.f });		// 4 or 5
		m_CurAnimqeue.push({ 1,  1.f });
	}

	Set_CombatAnim_Index(m_pModelCom);
	Create_Hit_Effect();
}

void CHero_Calibretto::Anim_Heavy_Hit()
{
	m_bIsHeavyHit = false;

	m_CurAnimqeue.push({ 13, 0.7f });		// 11,12,13,14
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);

	Create_Heavy_Hit_Effect();
}

void CHero_Calibretto::Anim_Flee()
{

	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		Safe_Release(*iter);
		iter = m_pEffectParts.erase(iter);
	}
	m_pEffectParts.clear();


	for (auto& pBuffImage : m_vecBuffImage)
		Safe_Release(pBuffImage);

	m_CurAnimqeue.push({ 43, 1.f });
	m_CurAnimqeue.push({ 42, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Die()
{
	m_iTurnCanvasOption = 1;
	m_Hero_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_CurAnimqeue.push({ 6,  1.f });
	m_CurAnimqeue.push({ 35, 1.f });
	m_bIsDead = true;

	Set_CombatAnim_Index(m_pModelCom);

	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		Safe_Release(*iter);
		iter = m_pEffectParts.erase(iter);
	}
	m_pEffectParts.clear();


	for (auto& pBuffImage : m_vecBuffImage)
		Safe_Release(pBuffImage);
	m_vecBuffImage.clear();
}

void CHero_Calibretto::Anim_Viroty()
{
	_int iRandHp = rand() % 3 + 1;
	_int iRandMp = rand() % 2 + 1;

	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_HP_POTION, iRandHp);
	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_MP_POSION, iRandMp);

	while (!m_CurAnimqeue.empty())
	{
		m_CurAnimqeue.pop();
	}

	if (m_bIsDead)
	{
		m_CurAnimqeue.push({ 44,  1.f });
		m_CurAnimqeue.push({ 45, 1.f });
	}
	else
	{
		m_CurAnimqeue.push({ 30,  1.f });
		m_CurAnimqeue.push({ 37, 1.f });
	}
	
	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		Safe_Release(*iter);
		iter = m_pEffectParts.erase(iter);
	}
	m_pEffectParts.clear();


	for (auto& pBuffImage : m_vecBuffImage)
		Safe_Release(pBuffImage);
	m_vecBuffImage.clear();


	Set_CombatAnim_Index(m_pModelCom);
}

CHero_Calibretto * CHero_Calibretto::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHero_Calibretto*		pInstance = new CHero_Calibretto(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHero_Calibretto");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHero_Calibretto::Clone(void * pArg)
{
	CHero_Calibretto*		pInstance = new CHero_Calibretto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHero_Calibretto");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHero_Calibretto::Free()
{
	__super::Free();

	for (auto& pPart : m_PlayerParts)
		Safe_Release(pPart);
	m_PlayerParts.clear();

	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		Safe_Release(*iter);
		iter = m_pEffectParts.erase(iter);
	}
	m_pEffectParts.clear();

	for (_uint i = 0; i < MAPTYPE_END; ++i)
		Safe_Release(m_pStatusCom[i]);


	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pFog);
	Safe_Release(m_pUltimateEffect);
	Safe_Release(m_pLazorEffect);
	Safe_Release(m_pFullscreenEffect);
}


_bool CHero_Calibretto::Is_Dead()
{
	if (__super::Is_Dead())
	{
		return true;
	}

	return false;
}

_int CHero_Calibretto::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 3)
	{
		bResult = ANIM_DIR_STRAIGHT;
	}
	else if (m_pModelCom->Get_AnimIndex() == 9)
	{
		bResult = ANIM_DIR_BACK;
		CCombatController::GetInstance()->Camera_Zoom_Out();
	}
	else
		bResult = ANIM_EMD;

	return bResult;
}

void CHero_Calibretto::CombatAnim_Move(_double TImeDelta)
{
	if (m_pHitTarget == nullptr || bResult == ANIM_EMD)
		return;

	if (bResult == ANIM_DIR_BACK)
	{
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(m_vOriginPos, TImeDelta, m_ReturnDistance, m_SpeedRatio);
		return;
	}
	else if (bResult == ANIM_DIR_STRAIGHT)
	{
		_float4 Target;
		XMStoreFloat4(&Target, m_pHitTarget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(XMLoadFloat4(&Target), TImeDelta, m_LimitDistance, m_SpeedRatio);
		CCombatController::GetInstance()->Camera_Zoom_In();
	}
	else
		return;
}

void CHero_Calibretto::MovingAnimControl(_double TimeDelta)
{
	if (!m_CurAnimqeue.empty() && m_pModelCom->Get_Finished(m_pModelCom->Get_AnimIndex()))
	{
		m_bIsCombatAndAnimSequnce = false;
		m_iOldAnim = m_pModelCom->Get_AnimIndex();

		if (m_bCombatChaseTarget == false)
			return;

		_uint i = m_CurAnimqeue.front().first;
		m_pModelCom->Set_AnimIndex(i);
		m_pModelCom->Set_AnimTickTime(m_CurAnimqeue.front().second);
		m_CurAnimqeue.pop();
		m_bFinishOption = ANIM_CONTROL_NEXT;

		if (m_CurAnimqeue.empty())
		{
			m_bIsCombatAndAnimSequnce = true;
		}
	}
}

void CHero_Calibretto::Fsm_Exit()
{
	m_Hero_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);

	_bool	bRenderTrue = true;
	m_Hero_CombatStateCanvasDelegeter.broadcast(bRenderTrue);
	m_pHitTarget = nullptr;
	CCombatController::GetInstance()->Camera_Zoom_Out();

}

void CHero_Calibretto::Intro_Exit()
{
	m_iNonRenderMeshIndex = 8;
}

void CHero_Calibretto::Create_Test_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, true);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = m_vSkill_Pos;
	BuffDesc.vScale = m_vTestScale;
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 4.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	
	static_cast<CBuff_Effect*>(pGameObject)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);

}

void CHero_Calibretto::Create_Test_Rect_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CAttack_Effect_Rect::Attack_Effec_Client EffectDesc;
	ZeroMemory(&EffectDesc, sizeof(EffectDesc));

	pGameObject = pInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, true);

	EffectDesc.fCoolTime = 5.f;
	EffectDesc.bIsMainTain = false;
	EffectDesc.iFrameCnt = 5;


	static_cast<CAttack_Effect_Rect*>(pGameObject)->Set_Client_BuffDesc(EffectDesc);
	static_cast<CAttack_Effect_Rect*>(pGameObject)->Set_Glow(true,TEXT("Prototype_Component_Texture_bretto_Lazer_Ultimate_Rect"),0);

	RELEASE_INSTANCE(CGameInstance);


}

void CHero_Calibretto::Create_Test_TextureObj()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pSkillParts = nullptr;

	CSkill_TextureObj::Skill_Texture_Client			SkillDesc;
	ZeroMemory(&SkillDesc, sizeof(CSkill_TextureObj::Skill_Texture_Client));

	XMStoreFloat4x4(&SkillDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	SkillDesc.ParentTransform = m_pTransformCom;
	SkillDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Calibretto_Hand_L");
	SkillDesc.eType = WEAPON_SKILL;
	SkillDesc.vScale = m_vTestScale;
	SkillDesc.eDir = Skill_DIR_DOWN;
	SkillDesc.vPosition = m_vSkill_Pos;
	SkillDesc.vAngle = 90.f;

	CGameObject * pGameObject = pGameInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, true);
	static_cast<CSkill_TextureObj*>(pGameObject)->Set_Skill_Texture_Client(SkillDesc);


	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Skill1_Bullet()
{		
	if (m_pHitTarget == nullptr)
		return;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(CBuff_Effect::BuffEffcet_Client));
	CBone* pSocket = m_pModelCom->Get_BonePtr("Bone_Calibretto_Hand_L");

	for (_uint i = 0; i < 5; ++i)
	{
		_int iRandomNum = rand() % 5;
		ZeroMemory(&BuffDesc, sizeof(BuffDesc));
		pGameObject = pInstance->Load_Effect(L"Texture_bretto_Real_Bullet_Effect_0", LEVEL_COMBAT, false);
		BuffDesc.ParentTransform = m_pTransformCom;
		BuffDesc.vPosition = _float4(_float(iRandomNum*0.2f), 0.3f, 0.6f, 1.f);
		BuffDesc.vScale = _float3(4.f, 4.f, 4.f);
		BuffDesc.vAngle = 90.f;
		BuffDesc.fCoolTime = 5.f;
		BuffDesc.bIsMainTain = false;
		BuffDesc.iFrameCnt = 3;
		BuffDesc.bIsUp = false;
		static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc, pSocket, m_pModelCom->Get_PivotFloat4x4());
		m_pEffectParts.push_back(pGameObject);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Skill1_Bullet_End()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	CBone* pSocket = m_pModelCom->Get_BonePtr("Bone_Calibretto_Hand_L");

	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_smoke_0_Effect", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-1.5f, 0.f, 3.f,1.f);		//m_vSkill_Pos;
	BuffDesc.vScale = _float3(5.f, 5.f, 5.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;

	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc, pSocket, m_pModelCom->Get_PivotFloat4x4());
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Skill2_Beam()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	m_bLazorStop = false;

	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	CBone* pSocket = m_pModelCom->Get_BonePtr("Bone_Calibretto_Hand_L");

	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pLazorEffect = nullptr;
	m_pLazorEffect = pInstance->Load_Effect(L"Texture_bretto_laser_Bullet_Effect_2", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(2.5f, -0.3f, 12.5f, 1.f);		//m_vSkill_Pos;
	BuffDesc.vScale	   = _float3(80.f, 30.f, 80.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 10.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;

	static_cast<CBuff_Effect*>(m_pLazorEffect)->Set_Client_BuffDesc(BuffDesc, pSocket, m_pModelCom->Get_PivotFloat4x4());
	static_cast<CBuff_Effect*>(m_pLazorEffect)->Set_Glow(true, L"Prototype_Component_Texture_bretto_laser_Bullet",0);
	//static_cast<CBuff_Effect*>(m_pLazorEffect)->Set_Dissove(true, L"Prototype_Component_Texture_Dissoive", 3);
	static_cast<CBuff_Effect*>(m_pLazorEffect)->Set_ShaderPass(8);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Skill2_Beam_End()
{
}

void CHero_Calibretto::Create_Buff_MainTain_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	pGameObject = pInstance->Load_Effect(TEXT("Texture_Buff_Effect_Power_2"), LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-1.5f,2.5f,-0.5f,1.f);
	BuffDesc.vScale = _float3(2.5f, 2.5f, 2.5f);
	BuffDesc.vAngle = -90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;
	BuffDesc.bIsStraight = false;
	m_bLazorStop = false;

	static_cast<CBuff_Effect*>(pGameObject)->Is_Particle_Effect(15);
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);

}

void CHero_Calibretto::Create_Normal_Attack_Effect()
{
	if (m_pHitTarget == nullptr)
		return;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_Punch_Flash_0", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(1.f, 1.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);

}

void CHero_Calibretto::Create_Skill1_Attack_Effect()
{
	if (m_pHitTarget == nullptr)
		return;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_Punch_Flash_0", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(1.f, 1.f, -1.f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 6;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);

}

void CHero_Calibretto::Create_Skill2_Attack_Effect()
{
}

void CHero_Calibretto::Create_Buff_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_Matrix_Effect_2", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-1.f, 1.8f, -0.5f, 1.f);
	BuffDesc.vScale = _float3(24.f, 24.f, 24.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_WideBuffEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_Bird_buff", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-2.5f, 6.f, 0.8f, 1.f);
	BuffDesc.vScale = _float3(20.f, 20.f, 20.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Wide_BuffEffect_Second()
{
	CCombatController* pCombatCtr = GET_INSTANCE(CCombatController);
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	map<const wstring, CGameObject*> CurMap = *(pCombatCtr->Get_CurActorMap());

	for (auto& pActor : CurMap)
	{
		if (dynamic_cast<CPlayer*>(pActor.second) != nullptr)
		{
			pGameObject = pInstance->Load_Effect(L"Texture_bretto_heal_spread_0_Effect", LEVEL_COMBAT, false);
			BuffDesc.ParentTransform = pActor.second->Get_Transform();
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(20.f, 20.f, 20.f);
			BuffDesc.vAngle = 90.f;
			BuffDesc.fCoolTime = 5.f;
			BuffDesc.bIsMainTain = false;
			BuffDesc.iFrameCnt = 5;
			BuffDesc.bIsUp = false;
			static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
			m_pEffectParts.push_back(pGameObject);


			CStatus* pStatus = static_cast<CStatus*>(pActor.second->Get_Component(TEXT("Com_StatusCombat")));
			assert(nullptr != pStatus && "CHero_Calibretto::Create_Wide_BuffEffect_Second");
			static_cast<CCombatActors*>(pActor.second)->WideBuff_Status(pStatus, 0, 50);
			CCombatController::GetInstance()->HPMp_Update(pActor.second);
		}
	}

	RELEASE_INSTANCE(CCombatController);
	RELEASE_INSTANCE(CGameInstance);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 2.f;
	vPos.y += 9.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target2_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("hp up"));


}

void CHero_Calibretto::Create_Skill_Ultimate_Effect0()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CAttack_Effect_Rect::Attack_Effec_Client EffectDesc;
	ZeroMemory(&EffectDesc, sizeof(EffectDesc));

	pGameObject = pInstance->Load_Effect(TEXT("Texture_bretto_Glow_Ultimate_Rect"), LEVEL_COMBAT, false);

	EffectDesc.fCoolTime = 5.f;
	EffectDesc.bIsMainTain = true;
	EffectDesc.iFrameCnt = 1;


	static_cast<CAttack_Effect_Rect*>(pGameObject)->Set_Client_BuffDesc(EffectDesc);
	static_cast<CAttack_Effect_Rect*>(pGameObject)->Set_Glow(true, TEXT("Prototype_Component_Texture_bretto_Lazer_Ultimate_Rect"), 0);

	m_pEffectParts.push_back(pGameObject);


	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Ultimate_End_Effect()
{
	if (m_pHitTarget == nullptr)
		return;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	_int		iSign = 1;
	_int		iRandomCnt = 0;
	_int		iRandomScale = 0;
	for (_uint i = 0; i < 50; ++i)
	{
		iRandomScale = rand() % 10 + 7;
		pGameObject = pInstance->Load_Effect(L"Texture_Garrsion_Fire_bot_Height_Effect_0", LEVEL_COMBAT, false);
		BuffDesc.ParentTransform = m_pHitTarget->Get_Transform();
		BuffDesc.vPosition = _float4(_float(rand() % 3 * iSign), _float(rand() % 2), _float(rand() % 3 * iSign), 1.f);
		BuffDesc.vScale = _float3(_float(iRandomScale), _float(iRandomScale), _float(iRandomScale));
		BuffDesc.vAngle = -90.f;
		BuffDesc.fCoolTime = 5.f;
		BuffDesc.bIsMainTain = false;
		BuffDesc.iFrameCnt = rand() % 10 + 4;
		BuffDesc.bIsUp = true;
		static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
		m_pEffectParts.push_back(pGameObject);
		iSign *= -1;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Move_Target_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Jump_Left_To_Right_0", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-1.f, 0.f, -4.f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 1;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Heavy_Hit_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject*   pGameObject = nullptr;

	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	pGameObject = pInstance->Load_Effect(L"Texture_Die_Effect_0", LEVEL_COMBAT, false);
	BuffDesc.vPosition = _float4(0.5f, 1.5f, 1.f, 1.f);
	BuffDesc.vScale = _float3(13.f, 13.f, 13.f);
	BuffDesc.ParentTransform = m_pTransformCom;

	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 2.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 7;
	BuffDesc.bIsUp = true;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	_int iRandom = rand() % 10 + m_iWideAttackDamgae;
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 1.f;
	vPos.y += 9.f;
	CDamage_Font_Manager::GetInstance()->Set_Damage_Target2_Font(vPos, _float3(2.f, 2.f, 2.f), iRandom);


	CExplain_FontMgr::GetInstance()->Set_Explain_Target2_Font0(vPos,
		_float3(2.f, 2.f, 2.f), TEXT("critical"));

	m_pStatusCom[COMBAT_PLAYER]->Take_Damage(iRandom);
	CCombatController::GetInstance()->HPMp_Update(this);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Ultimate_StartCam_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	m_pUltimateEffect = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pUltimateEffect = pInstance->Load_Effect(TEXT("UltiCam_Sprites_Effect_Lighting"), LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(36.5f, -3.18f, 25.f, 1.f);
	BuffDesc.vScale = _float3(50.f, 50.f, 50.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 7.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;

	static_cast<CBuff_Effect*>(m_pUltimateEffect)->Set_CamEffect(BuffDesc);
	//static_cast<CBuff_Effect*>(m_pUltimateEffect)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);



}

void CHero_Calibretto::Create_Ultimate_StartFog_CamEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	m_pFog = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pFog = pInstance->Load_Effect(TEXT("UltiCam_Sprites_Effect_Cloud"), LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(28.5f, 5.787f, 42.918f, 1.f);
	BuffDesc.vScale = _float3(120.f, 100.f, 120.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 8;
	BuffDesc.bIsUp = false;

	static_cast<CBuff_Effect*>(m_pFog)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(m_pFog)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_FullScreenEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	m_pFullscreenEffect = nullptr;

	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pFullscreenEffect = pInstance->Load_Effect(TEXT("Blood_Rect_Effect"), LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-5.422f, 20.646f, -9.382f, 1.f);
	BuffDesc.vScale = _float3(1.f, 1.f, 1.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 3;
	BuffDesc.bIsUp = false;


	static_cast<CBuff_Effect*>(m_pFullscreenEffect)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(m_pFullscreenEffect)->Set_ShaderPass(7);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Create_Hit_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	CHitBoxObject::WEAPON_OPTIONAL WeaponOption = static_cast<CHitBoxObject::WEAPON_OPTIONAL>(m_iHitWeaponOption);
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	switch (WeaponOption)
	{
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_NONE:
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BLUE:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_8", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
		BuffDesc.vScale = _float3(5.f, 5.f, 5.f);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PULPLE:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_10", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(0.5f, 1.5f, 1.f, 1.f);
		BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
		
		if (m_iMultiHitNum == 2 || m_iMultiHitNum == 0)
		{
			pInstance->Play_Sound(TEXT("Common_0059.wav"), 1.f, false, SOUND_TYPE_HIT);
		}
		if (m_iMultiHitNum == 3)
		{
			pInstance->Play_Sound(TEXT("Common_0249.wav"), 1.f, false, SOUND_TYPE_HIT);
		}
		m_iMultiHitNum++;
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GREEN:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_9", LEVEL_COMBAT, false);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SPIDER_ATTACK:
		pGameObject = pInstance->Load_Effect(L"Texture_Bleeding_Effect_3", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(1.f, 1.f, 0.5f, 1.f);		// 이미지 교체필요
		BuffDesc.vScale = _float3(13.f, 13.f, 13.f);
		pInstance->Play_Sound(TEXT("Common_0047.wav"), 1.f, false, SOUND_TYPE_HIT);
		static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(5);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SPIDER_HEAD:
		pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_2", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(0.5f, 1.5f, 1.f, 1.f);
		BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SLIME_KING_HEAD:
		pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_4", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(0.5f, 1.5f, 1.f, 1.f);
		BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SLIME_KING_BREATH:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_9", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(0.5f, 1.5f, 1.f, 1.f);
		BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
		break;



	default:
		break;
	}

	// 3개정도 생성하고 랜덤위치하고 아래에서 위로 올라가는 것처럼 만들기

	if (pGameObject == nullptr)
		RELEASE_INSTANCE(CGameInstance);

	if (iEffectNum == 1)
	{
		BuffDesc.ParentTransform = m_pTransformCom;

		BuffDesc.vAngle = 90.f;
		BuffDesc.fCoolTime = 2.f;
		BuffDesc.bIsMainTain = false;
		BuffDesc.iFrameCnt = 4;
		BuffDesc.bIsUp = false;
		static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
		m_pEffectParts.push_back(pGameObject);
	}

	else
	{
		_int iSignNum = 1;
		_int iRandScaleNum = rand() % 10 + 5;
		for (_uint i = 0; i < iEffectNum; ++i)
		{
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
			BuffDesc.ParentTransform = m_pTransformCom;
			BuffDesc.vPosition = _float4(_float(rand() % 2 * iSignNum), 1, _float(rand() % 2 * iSignNum), 1.f);
			BuffDesc.vScale = _float3(_float(iRandScaleNum), _float(iRandScaleNum), _float(iRandScaleNum));
			BuffDesc.vAngle = 90.f;
			BuffDesc.fCoolTime = 2.f;
			BuffDesc.bIsMainTain = false;
			BuffDesc.iFrameCnt = rand() % 5 + 3;
			BuffDesc.bIsUp = false;
			static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
			m_pEffectParts.push_back(pGameObject);
			iSignNum *= -1;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Calibretto::Use_HpPotion()
{
	_int iRandNum = rand() % 20 + 20;
	
	m_pStatusCom[COMBAT_PLAYER]->Incrase_Hp(iRandNum);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 1.f;
	vPos.y += 9.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target2_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("hp up"));


	CDamage_Font_Manager::GetInstance()->Set_HPMPFont_0(vPos, _float3(2.f, 2.f, 2.f), iRandNum);
	
	m_pStatusCom[DUNGEON_PLAYER]->Use_Item(CStatus::ITEM_HP_POTION);


	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_heal_spread_0_Effect", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 4.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(15.f, 15.f, 15.f);
	BuffDesc.vAngle = -90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	m_bOnceCreate = false;

	RELEASE_INSTANCE(CGameInstance);

}

void CHero_Calibretto::Use_MpPotion()
{
	_int iRandNum = rand() % 15 + 30;

	m_pStatusCom[DUNGEON_PLAYER]->Use_Item(CStatus::ITEM_MP_POSION);
	m_pStatusCom[COMBAT_PLAYER]->Incrase_Mp(iRandNum);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 1.f;
	vPos.y += 9.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target2_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("mp up"));


	CDamage_Font_Manager::GetInstance()->Set_HPMPFont_0(vPos, _float3(2.f, 2.f, 2.f), iRandNum);

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_heal_spread_1_Effect", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 4.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = -90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	
	RELEASE_INSTANCE(CGameInstance);

}

void CHero_Calibretto::Boss_Ultimate_Anim()
{
	m_CurAnimqeue.push({ 5 , 3.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Create_Wide_Debuff(CStatus::DEBUFFTYPE eDebuffOption)
{
	_uint iTextureNum = 0;

	switch (eDebuffOption)
	{
	case Engine::CStatus::DEBUFF_FIRE:
		break;
	case Engine::CStatus::DEBUFF_BLEED:
		iTextureNum = 1;
		m_DebuffName = TEXT("bleeding");
		break;
	case Engine::CStatus::DEBUFF_ARMOR:
		iTextureNum = 4;
		m_DebuffName = TEXT("armor down");
		break;
	case Engine::CStatus::DEBUFF_MAGIC:
		iTextureNum = 2;
		m_DebuffName = TEXT("magic down");
		break;
	case Engine::CStatus::BUFF_DAMAGE:
		break;
	case Engine::CStatus::DEBUFF_NONE:
		break;
	default:
		break;
	}

	CClient_Manager::Create_BuffImage(m_vecBuffImage,
		_float4(-305.f, -330.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
		TEXT("Prototype_GameObject_BuffImage"), iTextureNum);
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(eDebuffOption, true);
	
	_float4 vPos2;
	XMStoreFloat4(&vPos2, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos2.y += 8.f;
	CExplain_FontMgr::GetInstance()->
		Set_Debuff_Target2_Font(vPos2, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());
}

void CHero_Calibretto::Calculator_HitDamage()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 1.f;
	vPos.y += 9.f;
	
	if (Is_DebuffBlend(m_pStatusCom[COMBAT_PLAYER], m_iHitWeaponOption, &m_iGetDamageNum, m_DebuffName))
	{
		_float4 vPos2;
		XMStoreFloat4(&vPos2, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos2.y += 8.f;
		CExplain_FontMgr::GetInstance()->
			Set_Debuff_Target2_Font(vPos2, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());

	}
	else if (m_iGetDamageNum >= 50)
	{
		m_bIsHeavyHit = true;
		CExplain_FontMgr::GetInstance()->
			Set_Explain_Target2_Font1(vPos, _float3(1.f, 1.f, 1.f), TEXT("critical"));
	}

	CDamage_Font_Manager::GetInstance()->Set_Damage_Target2_Font(vPos, _float3(2.f, 2.f, 2.f), m_iGetDamageNum, 1.5f, 1.2f);

	m_pStatusCom[COMBAT_PLAYER]->Take_Damage(m_iGetDamageNum);

	if (m_pStatusCom[COMBAT_PLAYER]->Get_CurStatusHpRatio() <= 0.f)
	{
		CCombatController::GetInstance()->Camera_Shaking();
		m_bIsHeavyHit = true;
	}
	else
		CCombatController::GetInstance()->UI_Shaking(true);

}

void CHero_Calibretto::Initialize_CombatSound()
{
	CSoundPlayer::Anim_Model_SoundDesc SoundDesc;
	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 1;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0046.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 4;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0048.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);


	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 5;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0048.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 6;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0016.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 7;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Buff2.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 7;
	SoundDesc.iFrame = 30;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Buff_Second.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 8;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0009.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 9;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Return.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 11;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0021.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 12;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0016.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 13;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0020.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));		//Move 
	SoundDesc.iAnimIndex = 15;
	SoundDesc.iFrame = 2;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Return.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 16;
	SoundDesc.iFrame = 20;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0030.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 17;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0019.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 19;
	SoundDesc.iFrame = 20;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Potion.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 19;
	SoundDesc.iFrame = 20;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0057.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);


	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 25;
	SoundDesc.iFrame = 12;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0030.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 25;
	SoundDesc.iFrame = 6;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Gun_Hit_Gun.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 28;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0009.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 28;
	SoundDesc.iFrame = 58;
	SoundDesc.iSoundChannel = SOUND_TYPE_HIT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Explosion_Lazor.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 30;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0053.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 40;
	SoundDesc.iFrame = 23;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_0022.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 40;
	SoundDesc.iFrame = 90;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Wide_Buff_end.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);


	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 46;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_CALIBRETTO_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Ultimate.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);


	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 46;
	SoundDesc.iFrame = 257;
	SoundDesc.iSoundChannel = SOUND_ULTIBGM;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Calibretto_Explosion_Ultimate_End.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

}


