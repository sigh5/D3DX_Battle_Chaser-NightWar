#include "stdafx.h"
#include "..\public\Hero_Garrison.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "Model.h"
#include "CombatController.h"
#include "PlayerController.h"
#include "AnimFsm.h"
#include "Weapon.h"
#include "Bone.h"
#include "Buff_Effect.h"
#include "Damage_Font_Manager.h"
#include "Explain_FontMgr.h"
#include "Trail_Effect.h"
#include "HitBoxObject.h"
#include "SoundPlayer.h"

CHero_Garrison::CHero_Garrison(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice, pContext)
{
}

CHero_Garrison::CHero_Garrison(const CHero_Garrison& rhs)
	: CPlayer(rhs)
{
}

CGameObject * CHero_Garrison::Get_Weapon_Or_SkillBody()
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

_bool CHero_Garrison::Calculator_HitColl(CGameObject * pWeapon)
{
	CHitBoxObject* pCurActorWepon = static_cast<CHitBoxObject*>(pWeapon);

	assert(pCurActorWepon != nullptr && "CHero_Garrison::Calculator_HitColl");

	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{
		m_iGetDamageNum = pCurActorWepon->Get_WeaponDamage();
		m_iHitWeaponOption = static_cast<CHitBoxObject::WEAPON_OPTIONAL>(pCurActorWepon->Get_WeaponOption());
	
		if (m_bUseDefence == true)
		{
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			vPos.y += 4.f;
			m_pStatusCom[COMBAT_PLAYER]->Take_Damage(_int(m_iGetDamageNum*0.5));
			CDamage_Font_Manager::GetInstance()->Set_Damage_Target1_Font(vPos, _float3(2.f, 2.f, 2.f), _int(m_iGetDamageNum*0.5));
			Create_Hit_Effect();
			return true;
		}

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

_int CHero_Garrison::Get_RestHpPotion()
{
	return m_pStatusCom[DUNGEON_PLAYER]->Rest_iTemNum(CStatus::ITEM_HP_POTION);
}

_int CHero_Garrison::Get_RestMpPotion()
{
	return m_pStatusCom[DUNGEON_PLAYER]->Rest_iTemNum(CStatus::ITEM_MP_POSION);
}

HRESULT CHero_Garrison::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHero_Garrison::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Hero_Alumon");
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

HRESULT CHero_Garrison::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.21f, 0.f, 0.21f, 1.f));
	m_pTransformCom->Set_Scaled(_float3(0.1f, 0.1f, 0.1f));
	m_bLast_Initlize = true;

	_float4 vPos;
	XMStoreFloat4(&vPos, XMVectorSet(0.21f, 0.f, 0.21f, 1.f));
	m_pNavigationCom->Set_OldPos(vPos);
	return S_OK;
}

void CHero_Garrison::Tick(_double TimeDelta)
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
		Combat_Init();
		m_pAnimFsm->Tick(TimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

		if (m_pStatusCom[COMBAT_PLAYER]->Get_Dead() && !m_bIsDead)
			m_bIsDead = true;
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

		if (nullptr != m_pFullscreenEffect)
			m_pFullscreenEffect->Tick(TimeDelta);

		if (nullptr != m_pTrailUVMoveEffect)
			m_pTrailUVMoveEffect->Tick(TimeDelta);

		Create_Sword_Trail();

		//static float ffPos[3] = {};
		//static float ffScale[3] = {};
		//static char  szName[MAX_PATH] = "";
		//ImGui::InputFloat3("SkillPos", ffPos);
		//ImGui::InputFloat3("SkillScale", ffScale);

		//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		//ImGui::InputText("TextureName", szName, MAX_PATH);

		//if (ImGui::Button("Create_Skill"))
		//{
		//	_tchar Texture_NameTag[MAX_PATH] = TEXT("");
		//	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName) + 1, Texture_NameTag, MAX_PATH);

		//	m_TextureTag =   Texture_NameTag;
		//	m_vSkill_Pos = _float4(ffPos[0], ffPos[1], ffPos[2], 1.f);
		//	m_vTestScale = _float3(ffScale[0], ffScale[1], ffScale[2]);

		//	Create_Test_Effect();		// Test용

		//}
		//RELEASE_INSTANCE(CGameInstance);
	}
	

	m_pModelCom->Play_Animation(TimeDelta, m_bIsCombatScene);
}


void CHero_Garrison::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	if (m_bIsCombatScene)
	{
		for (auto &pParts : m_PlayerParts)
		{
			pParts->Late_Tick(TimeDelta);
		}
	}
	if (m_bModelRender)
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
			else if (m_bOriginBuff == false && m_bUseDefence == false && true == static_cast<CBuff_Effect*>(*iter)->Get_MainTain())
			{
				Safe_Release(*iter);
				iter = m_pEffectParts.erase(iter);
			}
			else if (m_bOriginBuff == true && m_bBuffEffectStop == true && true == static_cast<CBuff_Effect*>(*iter)->Get_MainTain())
			{
				Safe_Release(*iter);
				iter = m_pEffectParts.erase(iter);
				m_bOriginBuff = false;
			}

			else
				++iter;
		}

		if (m_bIsCombatScene == true )
		{
			if (!m_bIsDead && false == m_bUltimateBuffRenderStop && CClient_Manager::m_bCombatWin == false)
			{
				CClient_Manager::Sort_BuffImage(m_vecBuffImage, true);
				for (auto iter = m_vecBuffImage.begin(); iter != m_vecBuffImage.end();)
				{
					if (*iter != nullptr)
						(*iter)->Late_Tick(TimeDelta);

					++iter;
				}
				CClient_Manager::Delete_BuffImage(m_vecBuffImage, m_pStatusCom[COMBAT_PLAYER], false);
			}
		}

		if (nullptr != m_pFog)
		{
			if (static_cast<CBuff_Effect*>(m_pFog)->Get_IsFinish())
			{
				Safe_Release(m_pFog);
				m_pFog = nullptr;
			}
			else
				m_pFog->Late_Tick(TimeDelta);
			
		}

		if (nullptr != m_pFullscreenEffect)
		{
			m_pFullscreenEffect->Late_Tick(TimeDelta);
			if (static_cast<CBuff_Effect*>(m_pFullscreenEffect)->Get_IsFinish())
			{
				Safe_Release(m_pFullscreenEffect);
				m_pFullscreenEffect = nullptr;
			}
		}

		if (nullptr != m_pTrailUVMoveEffect)
		{
			m_pTrailUVMoveEffect->Late_Tick(TimeDelta);
			if (static_cast<CBuff_Effect*>(m_pTrailUVMoveEffect)->Get_IsFinish())
			{
				Safe_Release(m_pTrailUVMoveEffect);
				m_pTrailUVMoveEffect = nullptr;
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

HRESULT CHero_Garrison::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices", "DN_FR_FishingRod");
	}

	return S_OK;
}

HRESULT CHero_Garrison::Render_ShadowDepth()
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
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, 1, "g_BoneMatrices", "DN_FR_FishingRod");
	}
	return S_OK;
}

void CHero_Garrison::Change_Level_Data(_uint iLevleIdx)
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

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GarrisonDungeon"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("CHero_Garrison Change_Level_Data : LEVEL_COMBAT ");

		m_bIsCombatScene = false;

	}
	else if (LEVEL_COMBAT == iLevleIdx)
	{

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		_float3 vScale = m_pTransformCom->Get_Scaled();
		m_pStatusCom[DUNGEON_PLAYER]->Set_Dungeon_PosScale(vPos, vScale);
		m_pTransformCom->Set_TransfromDesc(7.f, 90.f);

		if (m_bCombatInit)
		{
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
			vPos = m_pStatusCom[COMBAT_PLAYER]->Get_CombatPos();
			vScale = m_pStatusCom[COMBAT_PLAYER]->Get_CombatScale();
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
			m_pTransformCom->Set_Scaled(vScale);
		}


		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GarrisonCombat"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("CHero_Garrison Change_Level_Data : LEVEL_COMBAT ");

		m_bIsCombatScene = true;

	}
	else
		return;
	// maybe 다른씬?


	RELEASE_INSTANCE(CGameInstance);
}

_uint CHero_Garrison::Get_AnimationIndex()
{
	return m_pModelCom->Get_AnimIndex();
}

void CHero_Garrison::AnimMove()
{
	m_pModelCom->Set_AnimIndex(m_iAnimIndex);
}

void CHero_Garrison::HighLightChar()
{
	HIGHLIGHT_UIDESC Desc;
	ZeroMemory(&Desc, sizeof HIGHLIGHT_UIDESC);

	Desc.iNumSizeX = 133.f;
	Desc.iNumSizeY = 134.f;
	Desc.yPos = 5.f;
	Desc.iTextureIndex = 8;
	lstrcpy(Desc.szObjectName, TEXT("UIButton2"));
	m_Hero_DungeonUIDelegeter.broadcast(Desc);
}

void CHero_Garrison::NormalLightCharUI()
{
	HIGHLIGHT_UIDESC Desc;
	ZeroMemory(&Desc, sizeof HIGHLIGHT_UIDESC);

	Desc.iNumSizeX = 133.f;
	Desc.iNumSizeY = 123.f;
	Desc.yPos = 0.f;
	Desc.iTextureIndex = 5;
	lstrcpy(Desc.szObjectName, TEXT("UIButton2"));
	m_Hero_DungeonUIDelegeter.broadcast(Desc);
}

void CHero_Garrison::Dungeon_Tick(_double TimeDelta)
{
	if (IsCaptin() && !CPlayer::KeyInput(TimeDelta, m_pNavigationCom))
		m_iAnimIndex = 0;
	CGameInstance*pGameInstance = GET_INSTANCE(CGameInstance);
	if (false == m_bIsWalk && m_bRun == true)
	{
		m_fWalkSoundTimer += (_float)TimeDelta;
		if (m_fWalkSoundTimer >= 0.3f)
		{
			pGameInstance->Stop_Sound(SOUND_GARRISON_EFFECT);
			pGameInstance->Play_Sound(TEXT("Knolan_Walk.wav"), 0.6f, false, SOUND_GARRISON_EFFECT);
			m_fWalkSoundTimer = 0.f;
		}
	}
	else
		pGameInstance->Stop_Sound(SOUND_GARRISON_EFFECT);
	
	RELEASE_INSTANCE(CGameInstance);

	AnimMove();
	CClient_Manager::CaptinPlayer_ColiderUpdate(this, m_pColliderCom, m_pTransformCom);

}

void CHero_Garrison::Combat_Tick(_double TimeDelta)
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

	for (auto &pEffect : m_pEffectParts)
	{
		pEffect->Tick(TimeDelta);
	}

	for (auto& pParts : m_PlayerParts)
	{
		pParts->Tick(TimeDelta);
	}

	if (m_bCreateDefenceTimer == true)
	{
		m_fDefenceFsmTimer += _float(TimeDelta);
	}
	if (m_fDefenceFsmTimer >= 1.f)
	{
		m_bCreateDefenceTimer = false;
		m_fDefenceFsmTimer = 0.f;
		Fsm_Exit();
	}


}

void CHero_Garrison::Combat_Ultimate(_double TimeDelta)
{
	Is_MovingAnim();
	CombatAnim_Move_Ultimate(TimeDelta);

	if (bResult == ANIM_DIR_STRAIGHT || bResult == ANIM_DIR_BACK)
	{
		MovingAnimControl(TimeDelta);
	}
	else
		CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);

	Ultimate_Anim_Frame_Control();

	for (auto &pEffect : m_pEffectParts)
	{
		pEffect->Tick(TimeDelta);
	}

	for (auto& pParts : m_PlayerParts)
	{
		pParts->Tick(TimeDelta);
	}

}

void CHero_Garrison::Combat_DeadTick(_double TimeDelta)
{
	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
}

void CHero_Garrison::Combat_BlendAnimTick(_double TimeDelta)
{
	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	Is_Skill1MovingAnim();
	CombatAnim_Move(TimeDelta);

	Anim_Frame_Create_Control();
	for (auto &pEffect : m_pEffectParts)
	{
		pEffect->Tick(TimeDelta);
	}

	for (auto& pParts : m_PlayerParts)
	{
		pParts->Tick(TimeDelta);
	}

}

void CHero_Garrison::Fsm_Exit()
{
	_bool	bRenderTrue = true;
	m_Hero_CombatStateCanvasDelegeter.broadcast(bRenderTrue);
	m_Hero_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_pHitTarget = nullptr;
	CCombatController::GetInstance()->Camera_Zoom_Out();
}

void CHero_Garrison::Defence_Exit()
{
	if (false == m_bIsDefenceTimer)
		return;

	m_fDefencTimer += (_float)(CClient_Manager::TimeDelta);

	if (m_fDefencTimer >= 1.f)
	{
		m_bIsDefenceTimer = false;
	}
}

void CHero_Garrison::MovingAnimControl(_double TimeDelta)
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

void CHero_Garrison::Create_Hit_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	CHitBoxObject::WEAPON_OPTIONAL WeaponOption = static_cast<CHitBoxObject::WEAPON_OPTIONAL>(m_iHitWeaponOption);
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	if (m_bUseDefence)
	{
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_8", LEVEL_COMBAT, false);
		BuffDesc.vPosition = _float4(0.f, 1.f, 1.f, 1.f);
		BuffDesc.vScale = _float3(6.f, 6.f, 6.f);
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
		switch (WeaponOption)
		{
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_NONE:
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BLUE:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_8", LEVEL_COMBAT, false);
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PULPLE:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_10", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale =	 _float3(5.f, 5.f, 5.f);
			
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
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SPIDER_ATTACK:
			pGameObject = pInstance->Load_Effect(L"Texture_Bleeding_Effect_3", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.5f, 1.f);
			BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
			static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(5);
			pInstance->Play_Sound(TEXT("Common_0047.wav"), 1.f, false, SOUND_TYPE_HIT);
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SPIDER_HEAD:
			pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_2", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.25f, 1.5f, 0.25f, 1.f);
			BuffDesc.vScale = _float3(7.f, 7.f, 7.f);
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SLIME_KING_HEAD:
			pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_4", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.25f, 1.5f, 0.25f, 1.f);
			BuffDesc.vScale = _float3(7.f, 7.f, 7.f);
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SLIME_KING_BREATH:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_9", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(5.f, 5.f, 5.f);
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
	}
	RELEASE_INSTANCE(CGameInstance);

}

void CHero_Garrison::Create_Defence_Effect_And_Action()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(30);
	m_CurAnimqeue.push({ 8,   1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	m_CurAnimqeue.push({ 2,  1.f });
	Set_CombatAnim_Index(m_pModelCom);

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Buff_Effect_4", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(20.f, 15.f, 20.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	m_bOnceCreate = false;
	m_bCreateDefenceTimer = true;
	RELEASE_INSTANCE(CGameInstance);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 2.f;
	vPos.y += 6.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target1_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("damage up"));


}

void CHero_Garrison::Create_Normal_Attack_Effect()
{
	if (m_pHitTarget == nullptr)
		return;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Garrsion_Fire_bot_Height_Effect_1", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pHitTarget->Get_Transform();
	BuffDesc.vPosition = _float4(0.f, 2.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(8.f, 12.f, 8.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 1;
	BuffDesc.bIsUp = true;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Skill1_Attack_Effect()
{
	if (m_pHitTarget == nullptr)
		return;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Garrsion_Fire_bot_Height_Effect_0", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pHitTarget->Get_Transform();
	BuffDesc.vPosition = _float4(2.0f, 1.5f, -1.5f, 1.f);
	BuffDesc.vScale = _float3(10.f, 14.f, 10.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Skill2_Attack_Effect()
{
	if (m_pHitTarget == nullptr)
		return;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Bleeding_Effect_1", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pHitTarget->Get_Transform();
	BuffDesc.vPosition = _float4(1.f, 1.f, -1.8f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Ultimate_Effect()
{
	if (m_pHitTarget == nullptr)
		return;
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Garrison_Ultimate_Effect_0", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 1.f, +1.0f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;
	BuffDesc.bIsStraight = true;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_Glow(true, TEXT("Prototype_Component_Texture_Garrison_Ultimate_Effect"), 0);
	m_pEffectParts.push_back(pGameObject);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Ultimate_End_Effect()
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
	for (_uint i = 0; i < 10; ++i)
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

void CHero_Garrison::Create_BuffEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Buff_Effect_5", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(20.f, 15.f, 20.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Move_Target_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Jump_Left_To_Right_5", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-0.7f, -0.1f, -1.2f, 1.f);
	BuffDesc.vScale = _float3(4.f, 4.f, 4.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 1;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Defence_Area()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Defence_Effect_2", LEVEL_COMBAT, false);

	_uint iEffectNum = 1;
	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(7.f, 7.f, 7.f);
	BuffDesc.vAngle = -90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 6;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Buff_MainTain_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(TEXT("Texture_Buff_Effect_Power"), LEVEL_COMBAT, false);
	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-1.9f, 0.4f, -0.5f, 1.f);
	BuffDesc.vScale = _float3(2.f, 2.f, 2.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;
	BuffDesc.bIsStraight = false;
	static_cast<CBuff_Effect*>(pGameObject)->Is_Particle_Effect(15);
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Ultimate_StartCam_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(TEXT("Knolan_Ultimage_CamEffect"), LEVEL_COMBAT, false);
	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(6.5f, 6.f, 14.6f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	m_pEffectParts.push_back(pGameObject);
	static_cast<CBuff_Effect*>(pGameObject)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Ultimate_StartFog_CamEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	m_pFog = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pFog = pInstance->Load_Effect(TEXT("UltiCam_Sprites_Effect_Cloud"), LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(11.f, 4.5f, 16.f, 1.f);
	BuffDesc.vScale = _float3(30.f, 20.f, 30.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 6;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(m_pFog)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(m_pFog)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_FullScreenEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	m_pFullscreenEffect = nullptr;

	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pFullscreenEffect = pInstance->Load_Effect(TEXT("Blood_Rect_Effect"), LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-3.56f, 19.115f, -10.524f, 1.f);
	BuffDesc.vScale = _float3(1.f, 1.f, 1.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;

	static_cast<CBuff_Effect*>(m_pFullscreenEffect)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(m_pFullscreenEffect)->Set_ShaderPass(7);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Create_Heavy_Hit_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject*   pGameObject = nullptr;

	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	pGameObject = pInstance->Load_Effect(L"Texture_Die_Effect_0", LEVEL_COMBAT, false);
	BuffDesc.vPosition = _float4(0.f, 1.f, 1.f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
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
	vPos.y += 4.f;
	CDamage_Font_Manager::GetInstance()->Set_Damage_Target1_Font(vPos, _float3(2.f, 2.f, 2.f), iRandom);

	vPos.x -= 2.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target1_Font0(vPos,
		_float3(2.f, 2.f, 2.f), TEXT("critical"));

	m_pStatusCom[COMBAT_PLAYER]->Take_Damage(iRandom);
	CCombatController::GetInstance()->HPMp_Update(this);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Garrison::Boss_Ultimate_Anim()
{
	m_CurAnimqeue.push({ 27,3.f });	//6,26,27
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Frame_Create_Control()
{
	if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(3, 9))
	{
		Create_Normal_Attack_Effect();
		m_bOnceCreate = true;
	}

	else if (!m_bRun && m_pModelCom->Control_KeyFrame_Create(4, 1))
	{
		Create_Move_Target_Effect();
		m_bRun = true;
	}

	else if (m_bUseDefence == true && !m_bOnceCreate &&
		m_pModelCom->Control_KeyFrame_Create(8, 20))
	{
		Create_Defence_Area();
		CCombatController::GetInstance()->Camera_Zoom_Out();
		m_bOnceCreate = true;
	}
	else if (m_bOriginBuff == true && !m_bOnceCreateBuff &&
		m_pModelCom->Control_KeyFrame_Create(8, 40))
	{
		Create_Buff_MainTain_Effect();
		CCombatController::GetInstance()->Camera_Zoom_Out();
		m_bOnceCreateBuff = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(10, 30))
	{
		Create_Skill1_Attack_Effect();
		m_bOnceCreate = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(16, 4))
	{
		CCombatController::GetInstance()->Camera_Shaking();

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 4.f;
		_int iRandom = rand() % 5;
		CDamage_Font_Manager::GetInstance()->Set_Damage_Target1_Font(vPos, _float3(2.f, 2.f, 2.f), iRandom + m_iGetDamageNum);

		Create_Hit_Effect();
		m_bOnceCreate = true;

		m_iMultiHitNum = 0;
	}
	else if (!m_bRun && m_pModelCom->Control_KeyFrame_Create(17, 14))
	{
		Create_Move_Target_Effect();
		m_bRun = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(19, 20))
	{
		Create_Skill2_Attack_Effect();
		m_bOnceCreate = true;
	}
	else if (m_bUseDefence == true && !m_bOnceCreate &&
		m_pModelCom->Control_KeyFrame_Create(32, 43))
	{
		Create_Hit_Effect();
		m_bOnceCreate = true;
		m_bIs_Multi_Hit = false;

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 4.f;
		CDamage_Font_Manager::GetInstance()->Set_Damage_Target1_Font(vPos, _float3(2.f, 2.f, 2.f), _int(m_iGetDamageNum*0.5));
	}
	else
		return;
}

void CHero_Garrison::Use_HpPotion()
{
	_int iRandNum = rand() % 20 + 20;
	m_pStatusCom[DUNGEON_PLAYER]->Use_Item(CStatus::ITEM_HP_POTION);
	m_pStatusCom[COMBAT_PLAYER]->Incrase_Hp(iRandNum);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 2.f;
	vPos.y += 4.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target1_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("hp up"));

	vPos.y += 2.f;
	CDamage_Font_Manager::GetInstance()->Set_HPMPFont_0(vPos, _float3(1.5f, 1.5f, 1.5f), iRandNum);

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_heal_spread_0_Effect", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 2.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(6.f, 6.f, 6.f);
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

void CHero_Garrison::Use_MpPotion()
{
	_int iRandNum = rand() % 15 + 30;
	m_pStatusCom[COMBAT_PLAYER]->Incrase_Mp(iRandNum);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	m_pStatusCom[DUNGEON_PLAYER]->Use_Item(CStatus::ITEM_MP_POSION);

	vPos.x -= 2.f;
	vPos.y += 4.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target1_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("mp up"));

	vPos.y += 2.f;
	CDamage_Font_Manager::GetInstance()->Set_HPMPFont_0(vPos, _float3(1.5f, 1.5f, 1.5f), iRandNum);

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_bretto_heal_spread_1_Effect", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 2.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(6.f, 6.f, 6.f);
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

void CHero_Garrison::Calculator_HitDamage()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 2.f; vPos.y += 4.f;
	
	if (Is_DebuffBlend(m_pStatusCom[COMBAT_PLAYER], m_iHitWeaponOption, &m_iGetDamageNum, m_DebuffName))
	{
		_float4 vPos2;
		XMStoreFloat4(&vPos2, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos2.y += 6.f;
		vPos2.x -= 6.f;
		CExplain_FontMgr::GetInstance()->
			Set_Debuff_Target1_Font(vPos2, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());
	}
	else if (m_iGetDamageNum >= 50)
	{
		m_bIsHeavyHit = true;
		CExplain_FontMgr::GetInstance()->Set_Explain_Target1_Font1(vPos, _float3(1.f, 1.f, 1.f), TEXT("critical"));
	}
	CDamage_Font_Manager::GetInstance()->Set_Damage_Target1_Font(vPos, _float3(2.f, 2.f, 2.f), m_iGetDamageNum);
	m_pStatusCom[COMBAT_PLAYER]->Take_Damage(m_iGetDamageNum);

	if (m_pStatusCom[COMBAT_PLAYER]->Get_CurStatusHpRatio() <= 0.f)
	{
		CCombatController::GetInstance()->Camera_Shaking();
		m_bIsHeavyHit = true;
	}
	else
		CCombatController::GetInstance()->UI_Shaking(true);
}

void CHero_Garrison::Create_Wide_Debuff(CStatus::DEBUFFTYPE eDebuffOption)
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
		_float4(-305.f, -285.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
		TEXT("Prototype_GameObject_BuffImage"), iTextureNum);
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(eDebuffOption, true);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 2.f;
	vPos.y += 6.f;
	CExplain_FontMgr::GetInstance()->Set_Debuff_Target1_Font(vPos, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());
}

void CHero_Garrison::Initialize_CombatSound()
{
	CSoundPlayer::Anim_Model_SoundDesc SoundDesc;
	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 1;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0036.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 3;
	SoundDesc.iFrame = 10;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_Swing3.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 3;
	SoundDesc.iFrame = 10;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0059.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);
	////

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 4;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0040.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 6;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0023.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 7;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0039.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);


	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 8;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0024.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 8;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0078.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);


	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 10;
	SoundDesc.iFrame = 18;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0020.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 10;
	SoundDesc.iFrame = 18;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_NormalAttack.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);
	////

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 11;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0082.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 13;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0022.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 15;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_HeavyHit.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 16;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0021.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 17;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0049.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 19;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0034.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 19;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0059.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);
	/////

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 21;
	SoundDesc.iFrame = 35;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0106.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 22;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0089.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 23;
	SoundDesc.iFrame = 20;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0051.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 23;
	SoundDesc.iFrame = 20;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_Potion.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);


	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 26;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0044.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 30;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_VOCIE;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0084.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 30;
	SoundDesc.iFrame = 1;
	SoundDesc.iSoundChannel = SOUND_TYPE_KNOLAN_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0056.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 30;
	SoundDesc.iFrame = 10;
	SoundDesc.iSoundChannel = SOUND_ULTIBGM;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_UltiCamStart.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 30;
	SoundDesc.iFrame = 40;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_Buff.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);

	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 30;
	SoundDesc.iFrame = 295;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_Explosion_Ultimate.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);


	ZeroMemory(&SoundDesc, sizeof(SoundDesc));
	SoundDesc.iAnimIndex = 32;
	SoundDesc.iFrame = 10;
	SoundDesc.iSoundChannel = SOUND_GARRISON_EFFECT;
	lstrcpy(SoundDesc.pSoundTag, TEXT("Garrison_0036.wav"));
	CSoundPlayer::GetInstance()->Add_SoundEffect_Model(m_pModelCom, SoundDesc);
}



HRESULT CHero_Garrison::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GarrisonDungeon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.0f, 2.0f, 0.8f);
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
	StatusDesc.iHp = 250;
	StatusDesc.iMp = 150;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusDungeon"),
		(CComponent**)&m_pStatusCom[DUNGEON_PLAYER], &StatusDesc)))
		return E_FAIL;

	/*m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_HP_POTION, 15);
	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_MP_POSION, 15);*/

	return S_OK;
}

HRESULT CHero_Garrison::SetUp_ShaderResources()
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

HRESULT CHero_Garrison::Combat_Init()
{
	if (m_bCombatInit)
		return S_OK;

	m_pAnimFsm = CAnimFsm::Create(this, ANIM_CHAR2);
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
	m_pTransformCom->Set_Scaled(_float3(4.f, 4.f, 4.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(12.5f, 0.f, 22.5f, 1.f));
	m_pTransformCom->Set_TransfromDesc(7.f, 90.f);
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (FAILED(Ready_CombatParts()))
		return E_FAIL;

	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_HP_POTION, 10);
	_float4 vPos;
	XMStoreFloat4(&vPos, XMVectorSet(12.5f, 0.f, 22.5f, 1.f));
	_float3 vScale = _float3(4.f, 4.f, 4.f);
	m_pStatusCom[COMBAT_PLAYER]->Set_Combat_PosScale(vPos, vScale);


	m_bDefence = true;
	m_isWideBuff = false;
	m_bCombatInit = true;

	
	return S_OK;
}

HRESULT CHero_Garrison::Ready_CombatParts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Weapon_Sword_Classic");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.f, 0.f, -2.f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(1.5f, 3.6f, 1.5f, 0.f));
	WeaponDesc.eType = WEAPON_SWORD;
	WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BLUE;

	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;
	
	
	m_PlayerParts.push_back(pPartObject);

	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	ZeroMemory(&StatusDesc, sizeof(CStatus::StatusDesc));
	StatusDesc.iHp = 1000;
	StatusDesc.iMp = 300;
	StatusDesc.iExp = 0;
	StatusDesc.iLevel = 1;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusCombat"),
		(CComponent**)&m_pStatusCom[COMBAT_PLAYER], &StatusDesc)))
		return E_FAIL;

	_float3 vCenter = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider()->Get_Center();

	CHitBoxObject::HitBoxObject  hitBoxDesc;
	lstrcpy(hitBoxDesc.HitBoxOrigin_Desc.m_pTextureTag, TEXT("Prototype_Component_Texture_Trail"));
	hitBoxDesc.Poing_Desc.m_iFrameCnt = 2;
	hitBoxDesc.Poing_Desc.m_iTextureMax_Height_Cnt = 1;
	hitBoxDesc.Poing_Desc.m_iTextureMax_Width_Cnt = 1;

	pGameInstance->Clone_GameObject(LEVEL_COMBAT, LAYER_EFFECT,
		TEXT("Prototype_GameObject_Trail_Effect"), &hitBoxDesc);

	m_pTrailEffect = pGameInstance->
		Get_GameObject(LEVEL_COMBAT, LAYER_EFFECT, TEXT("TestTrail"));



	


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CHero_Garrison * CHero_Garrison::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHero_Garrison*		pInstance = new CHero_Garrison(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHero_Garrison::Clone(void * pArg)
{
	CHero_Garrison*		pInstance = new CHero_Garrison(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHero_Garrison::Free()
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
	Safe_Release(m_pAnimFsm);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	
	Safe_Release(m_pFog);
	Safe_Release(m_pFullscreenEffect);
	Safe_Release(m_pTrailEffect);
	Safe_Release(m_pTrailUVMoveEffect);

}

_int CHero_Garrison::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 4)
	{
		bResult = ANIM_DIR_STRAIGHT;

	}
	else if (m_pModelCom->Get_AnimIndex() == 18)
	{
		bResult = ANIM_DIR_STRAIGHT;
	}
	else if (m_pModelCom->Get_AnimIndex() == 11)
	{
		bResult = ANIM_DIR_BACK;

	}
	else
		bResult = ANIM_EMD;

	return bResult;
}

void CHero_Garrison::Is_Skill1MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 4)
		bResult = ANIM_DIR_STRAIGHT;
	else if (m_pModelCom->Get_AnimIndex() == 12)
	{
		m_pModelCom->Set_Duration(12, 0.2);
		bResult = ANIM_EMD;
	}
	else if (m_pModelCom->Get_AnimIndex() == 11)
		bResult = ANIM_DIR_BACK;
	else
		bResult = ANIM_EMD;
}

void CHero_Garrison::CombatAnim_Move(_double TImeDelta)
{
	if (bResult == ANIM_DIR_BACK)
	{
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(m_vOriginPos, TImeDelta, m_ReturnDistance, m_SpeedRatio);
		CCombatController::GetInstance()->Camera_Zoom_Out();
	}
	if (m_pHitTarget == nullptr || bResult == ANIM_EMD)
		return;

	if (bResult == ANIM_DIR_STRAIGHT)
	{
		_float4 Target;
		XMStoreFloat4(&Target, m_pHitTarget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(XMLoadFloat4(&Target), TImeDelta, m_LimitDistance, m_SpeedRatio);
		CCombatController::GetInstance()->Camera_Zoom_In();
	}
}

void CHero_Garrison::Ultimate_Anim_Frame_Control()
{
	if (!m_bFogStart && m_pModelCom->Control_KeyFrame_Create(30, 7))
	{
		//Create_Ultimate_StartFog_CamEffect();
		CCombatController::GetInstance()->Load_CamBG2();
		Create_Ultimate_StartCam_Effect();
		m_bFogStart = true;
	}
	//else if (!m_bUltimateCam && m_pModelCom->Control_KeyFrame_Create(30, 45))		//45
	//{
	//	Safe_Release(m_pFog);
	//	m_pFog = nullptr;
	//	Create_Ultimate_StartCam_Effect();
	//	m_bUltimateCam = true;
	//}
	//else if (!m_bIsUseUltimate && m_pModelCom->Control_KeyFrame_Create(30, 70))		//70
	//{
	//	m_bUltimateBuffRenderStop = false;
	//	CCombatController::GetInstance()->Set_Ultimate_End(true);
	//	CCombatController::GetInstance()->Camera_Zoom_In();
	//	m_bIsUseUltimate = true;
	//}

	else if (!m_bUltimateCam && m_pModelCom->Control_KeyFrame_Create(30, 70))		//45
	{
		/*	Safe_Release(m_pFog);
			m_pFog = nullptr;*/
		//
		m_bUltimateBuffRenderStop = false;
		CCombatController::GetInstance()->Set_Ultimate_End(true);
		CCombatController::GetInstance()->Camera_Zoom_In();
		m_bUltimateCam = true;
		m_bIsUseUltimate = true;
	}
	//else if (!m_bIsUseUltimate && m_pModelCom->Control_KeyFrame_Create(30, 70))		//70
	//{
	//	m_bUltimateBuffRenderStop = false;
	//	CCombatController::GetInstance()->Set_Ultimate_End(true);
	//	CCombatController::GetInstance()->Camera_Zoom_In();
	//	m_bIsUseUltimate = true;
	//}

	else if (!m_bUltimateHit[0] && m_pModelCom->Control_KeyFrame_Create(30, 105)) //1
	{
		Create_Ultimate_Effect();
		CCombatController::GetInstance()->UI_Shaking(true);
		m_bUltimateHit[0] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 3;
		TrailDesc.iMaxCenterCount = 7;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_NORMAL;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 6, 18);
	}
	else if (!m_bUltimateHit[1] && m_pModelCom->Control_KeyFrame_Create(30, 128)) //2
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[1] = true;
		
		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 15;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_NORMAL;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 6, 27);
	}
	else if (!m_bUltimateHit[2] && m_pModelCom->Control_KeyFrame_Create(30, 136)) //3
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[2] = true;
		
		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 36);
	}

	else if (!m_bUltimateHit[3] && m_pModelCom->Control_KeyFrame_Create(30, 148)) //4
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[3] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_NORMAL;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 6, 37);
	}
	else if (!m_bUltimateHit[4] && m_pModelCom->Control_KeyFrame_Create(30, 158)) //5
	{
		Create_Ultimate_Effect();
		CCombatController::GetInstance()->UI_Shaking(true);
		m_bUltimateHit[4] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_NORMAL;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 6, 37);

	}
	else if (!m_bUltimateHit[5] && m_pModelCom->Control_KeyFrame_Create(30, 170)) //6
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[5] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 36);
	}
	else if (!m_bUltimateHit[6] && m_pModelCom->Control_KeyFrame_Create(30, 180))//7
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[6] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 36);
	}

	else if (!m_bUltimateHit[7] && m_pModelCom->Control_KeyFrame_Create(30, 190)) //8
	{
		Create_Ultimate_Effect();
		
		m_bUltimateHit[7] = true;
		
		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 36);
	}
	else if (!m_bUltimateHit[8] && m_pModelCom->Control_KeyFrame_Create(30, 200)) //9
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[8] = true;
		CCombatController::GetInstance()->UI_Shaking(true);

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 36);
	}
	else if (!m_bUltimateHit[9] && m_pModelCom->Control_KeyFrame_Create(30, 210)) //10
	{
		Create_Ultimate_Effect();
		//CCombatController::GetInstance()->UI_Shaking(true);
		m_bUltimateHit[9] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 25;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 48);
	}
	else if (!m_bUltimateHit[10] && m_pModelCom->Control_KeyFrame_Create(30, 221)) //11
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[10] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 38);
	}
	else if (!m_bUltimateHit[11] && m_pModelCom->Control_KeyFrame_Create(30, 233)) //12
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[11] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 36);
	}
	else if (!m_bUltimateHit[12] && m_pModelCom->Control_KeyFrame_Create(30, 241)) //13
	{
		Create_Ultimate_Effect();
		m_bUltimateHit[12] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 38);
	}
	else if (!m_bUltimateHit[13] && m_pModelCom->Control_KeyFrame_Create(30, 253)) //14
	{
		Create_Ultimate_Effect();
		CCombatController::GetInstance()->UI_Shaking(true);
		m_bUltimateHit[13] = true;

		CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
		ZeroMemory(&TrailDesc, sizeof(TrailDesc));
		TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
		TrailDesc.pGameObject = m_PlayerParts[0];
		TrailDesc.iDevideFixNum = 2;
		TrailDesc.iMaxCenterCount = 20;
		TrailDesc.fSize = 1.f;
		TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
		static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 36);

	}
	else if (!m_bUltimateHit[14] && m_pModelCom->Control_KeyFrame_Create(30, 270))
	{
		m_iHitCount = 0;
		m_bUltimateHit[14] = true;

	}
	else if (!m_bFullScreenEffect && m_pModelCom->Control_KeyFrame_Create(30, 294))
	{
		Create_FullScreenEffect();
		m_bFullScreenEffect = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(30, 302))
	{
		Create_Ultimate_End_Effect();
		m_bBuffEffectStop = true;
		CCombatController::GetInstance()->Wide_Attack(false, 73);
		m_bOnceCreate = true;
	}
	else
		return;


}

void CHero_Garrison::Create_Sword_Trail()
{
	if (nullptr != m_pTrailEffect)
	{
		if (!m_bTraillEffectStartCheck&& m_pModelCom->Control_KeyFrame_Create(3, 4))
		{
			CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
			ZeroMemory(&TrailDesc, sizeof(TrailDesc));
			TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
			TrailDesc.pGameObject = m_PlayerParts[0];
			TrailDesc.iDevideFixNum = 3;
			TrailDesc.iMaxCenterCount = 7;
			TrailDesc.fSize = 1.f;
			TrailDesc.eType = CTrail_Effect::GARRISON_NORMAL;
			static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 6, 18);
			m_bTraillEffectStartCheck = true;
			m_bTrailEndCheck = false;


		}
		else if (!m_bTrailEndCheck&& m_pModelCom->Control_KeyFrame_Create(3, 10))
		{
			m_bTrailEndCheck = true;
			m_bTraillEffectStartCheck = false;
		}
		/*else if (!m_bTraillEffectStartCheck&& m_pModelCom->Control_KeyFrame_Create(10, 20))
		{
			CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
			ZeroMemory(&TrailDesc, sizeof(TrailDesc));
			TrailDesc.pColider = static_cast<CWeapon*>(m_PlayerParts[0])->Get_Colider();
			TrailDesc.pGameObject = m_PlayerParts[0];
			TrailDesc.iDevideFixNum = 10;
			TrailDesc.iMaxCenterCount = 10;
			TrailDesc.fSize = 4.f;
			TrailDesc.eType = CTrail_Effect::GARRISON_SKILL1;
			static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 6, 91);
			m_bTraillEffectStartCheck = true;
			m_bTrailEndCheck = false;
		}
		else if (!m_bTrailEndCheck&& m_pModelCom->Control_KeyFrame_Create(10, 40))
		{
			m_bTrailEndCheck = true;
			m_bTraillEffectStartCheck = false;
		}*/
		else if (!m_bTraillEffectStartCheck&& m_pModelCom->Control_KeyFrame_Create(19, 3))
		{

			CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
			_uint			iEffectNum = 1;
			CBuff_Effect::BuffEffcet_Client BuffDesc;
			ZeroMemory(&BuffDesc, sizeof(BuffDesc));
			CBone* pSocket = m_pModelCom->Get_BonePtr("Weapon_Sword_Classic");

			m_pTrailUVMoveEffect = pInstance->Load_Effect(TEXT("Texture_garrison_burst_0_Effect"), LEVEL_COMBAT, false);
			BuffDesc.ParentTransform = m_pTransformCom;
			BuffDesc.vPosition = _float4(-0.5f,0.1f,2.0f,1.f);
			BuffDesc.vScale = _float3(8.f,8.f,8.f);
			BuffDesc.vAngle = 90.f;
			BuffDesc.fCoolTime = 4.f;
			BuffDesc.bIsMainTain = false;
			BuffDesc.iFrameCnt = 5;
			BuffDesc.bIsUp = false;
			
			static_cast<CBuff_Effect*>(m_pTrailUVMoveEffect)->Set_Client_BuffDesc(BuffDesc, pSocket, m_pModelCom->Get_PivotFloat4x4());
			static_cast<CBuff_Effect*>(m_pTrailUVMoveEffect)->Set_ShaderPass(5);
			RELEASE_INSTANCE(CGameInstance);

			m_bTraillEffectStartCheck = true;
			m_bTrailEndCheck = false;
		}

		else if (!m_bTrailEndCheck&& m_pModelCom->Control_KeyFrame_Create(19, 16))
		{
			m_bTrailEndCheck = true;
			m_bTraillEffectStartCheck = false;
		}
		
		else
			return;
		

	}

}

void CHero_Garrison::CombatAnim_Move_Ultimate(_double TImeDelta)
{
	if (m_pHitTarget == nullptr)
		return;

	_float4 Target;
	XMStoreFloat4(&Target, m_pHitTarget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));

	if (m_pModelCom->Control_KeyFrame(30, 83, 94))				// 특수사항
		m_pTransformCom->CombatChaseTarget(XMLoadFloat4(&Target), TImeDelta, m_LimitDistance, m_SpeedRatio);
	else if (bResult == ANIM_DIR_BACK)
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(m_vOriginPos, TImeDelta, m_ReturnDistance, 6.f);
	else
		return;
}

void CHero_Garrison::Anim_Idle()
{
	m_CurAnimqeue.push({ 2,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Intro()
{
	m_CurAnimqeue.push({ 21, m_IntroTimer });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::AnimNormalAttack()
{
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStateDamage = rand() % 10 + 20 + m_iStage_Buff_DamgaeUP;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SWORD;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GARRISON_NORMAL;
	m_bOnceCreate = false;
	m_bRun = false;

	m_SpeedRatio = 7.f;
	m_LimitDistance = 10.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 0.9f;
	m_iStage_Buff_DamgaeUP = 0;
	m_bBuffEffectStop = true;

	m_CurAnimqeue.push({ 4,  m_setTickForSecond });
	m_CurAnimqeue.push({ 3,  1.f });		// 3에서		// 3 22에 이펙트 생성
	m_CurAnimqeue.push({ 11, m_setTickForSecond });
	m_CurAnimqeue.push({ 12, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Skill1_Attack()
{
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_bOnceCreate = false;
	m_bRun = false;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SWORD;
	m_iStateDamage = rand() % 10 + 40 + m_iStage_Buff_DamgaeUP;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(30);
	m_SpeedRatio = 8.f;
	m_LimitDistance = 10.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 0.9f;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GARRISON_SKILL1;
	m_iStage_Buff_DamgaeUP = 0;
	m_bBuffEffectStop = true;


	m_CurAnimqeue.push({ 4,  m_setTickForSecond });
	m_CurAnimqeue.push({ 12, 1.f });
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 11, m_setTickForSecond });
	m_CurAnimqeue.push({ 12, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Skill2_Attack()
{
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_bOnceCreate = false;
	m_bRun = false;
	m_eWeaponType = WEAPON_SWORD;
	m_iStateDamage = rand() % 10 + 50 + m_iStage_Buff_DamgaeUP;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_iHitCount = 1;
	m_LimitDistance = 12.f;
	m_SpeedRatio = 6.f;
	m_ReturnDistance = 0.5f;
	m_setTickForSecond = 0.9f;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GARRISON_SKILL2;
	m_bBuffEffectStop = true;
	m_iStage_Buff_DamgaeUP = 0;
	m_bTraillEffectStartCheck = false;


	m_CurAnimqeue.push({ 17, 1.0f });
	m_CurAnimqeue.push({ 18, 0.5f });
	m_CurAnimqeue.push({ 19, 1.f });
	m_CurAnimqeue.push({ 11, m_setTickForSecond });
	m_CurAnimqeue.push({ 12, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Uitimate()
{
	m_bUltimateBuffRenderStop = true;
	m_bFullScreenEffect = false;
	m_bUltimateCam = false;
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_bOnceCreate = false;
	m_bRun = false;
	m_bIsUseUltimate = false;
	m_bBuffEffectStop = true;
	m_bFogStart = false;
	for (_uint i = 0; i < 15; ++i)
		m_bUltimateHit[i] = false;

	m_iHitCount = 100;

	m_eWeaponType = WEAPON_SWORD;

	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);

	m_LimitDistance = 10.f;
	m_SpeedRatio = 6.f;
	m_ReturnDistance = 0.5f;
	m_setTickForSecond = 0.9f;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GARRISON_Ultimate;

	m_CurAnimqeue.push({ 30,  1.f });	// Key프레임 (뛰는 것)하나 찾기 83~94 는 움직여야함  //Texture_Garrison_Ultimate_Effect
	m_CurAnimqeue.push({ 11,  m_setTickForSecond });
	m_CurAnimqeue.push({ 12,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Buff()
{
	m_iStage_Buff_DamgaeUP = 10;
	m_bBuffEffectStop = false;
	m_bOnceCreateBuff = false;
	m_bOriginBuff = true;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_CurAnimqeue.push({ 8,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 2.f;
	vPos.y += 6.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target1_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("damage up"));

	CClient_Manager::Create_BuffImage(m_vecBuffImage,
		_float4(-305.f, -285.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
		TEXT("Prototype_GameObject_BuffImage"), 0);
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, true);

	Set_CombatAnim_Index(m_pModelCom);
	Create_BuffEffect();
}

void CHero_Garrison::Anim_Use_Item()
{
	m_bBuffEffectStop = true;
	m_CurAnimqeue.push({ 23,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Defence()
{
	m_CurAnimqeue.push({ 32,   1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Light_Hit()
{
	if (m_bIsHeavyHit)
	{
		m_CurAnimqeue.push({ 13 ,  1.f });
		m_bIsHeavyHit = false;
	}
	else if (true == m_bIs_Multi_Hit)
	{
		m_CurAnimqeue.push({ 6,  1.f });
		m_CurAnimqeue.push({ 16,  1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_bOnceCreate = false;
		m_bIs_Multi_Hit = false;
		
		m_iMultiHitNum = 2;
	}
	else
	{
		m_CurAnimqeue.push({ 26, 1.f });	//6,26,27
		m_CurAnimqeue.push({ 1,  1.f });
	}

	Set_CombatAnim_Index(m_pModelCom);
	Create_Hit_Effect();
}

void CHero_Garrison::Anim_Heavy_Hit()
{
	m_bIsHeavyHit = false;
	m_CurAnimqeue.push({ 15 ,  0.7f });	//13 ,14,15,16
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);

	Create_Heavy_Hit_Effect();
}

void CHero_Garrison::Anim_Flee()
{

	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		Safe_Release(*iter);
		iter = m_pEffectParts.erase(iter);
	}
	m_pEffectParts.clear();


	for (auto& pBuffImage : m_vecBuffImage)
		Safe_Release(pBuffImage);
	m_vecBuffImage.clear();


	m_CurAnimqeue.push({ 48,  1.f });
	m_CurAnimqeue.push({ 47,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Die()
{
	m_iTurnCanvasOption = 1;
	m_Hero_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_CurAnimqeue.push({ 7,  1.f });
	m_CurAnimqeue.push({ 33,  1.f });
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

void CHero_Garrison::Anim_Viroty()
{
	_int iRandHp = rand() % 3 + 1;
	_int iRandMp = rand() % 2 + 1;

	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_HP_POTION, iRandHp);
	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_MP_POSION, iRandMp);

	while (!m_CurAnimqeue.empty())
	{
		m_CurAnimqeue.pop();
	}

	if (Is_Dead())
	{
		m_CurAnimqeue.push({ 44,  1.f });
		m_CurAnimqeue.push({ 45, 1.f });

	}
	else
	{
		m_CurAnimqeue.push({ 22,  1.f });
		m_CurAnimqeue.push({ 43, 1.f });
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

void CHero_Garrison::Create_Test_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	CBone* pSocket = m_pModelCom->Get_BonePtr("Weapon_Sword_Classic");

	pGameObject = pInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, true);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = m_vSkill_Pos;
	BuffDesc.vScale = m_vTestScale;
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 4.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	
	static_cast<CBuff_Effect*>(pGameObject)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);

}

_bool CHero_Garrison::Is_Dead()
{
	if (__super::Is_Dead())
	{
		return true;
	}
	return false;
}
