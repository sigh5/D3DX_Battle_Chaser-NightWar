#include "stdafx.h"
#include "..\public\Hero_Knolan.h"

#include "GameInstance.h"
#include "Client_Manager.h"

#include "PlayerController.h"
#include "CombatController.h"
#include "AnimFsm.h"
#include "Weapon.h"

#include "Skill_Object.h"
#include "Skill_TextureObj.h"
#include "Bone.h"
#include "Buff_Effect.h"
#include "ToolManager.h"
#include "Damage_Font_Manager.h"
#include "Explain_FontMgr.h"

CHero_Knolan::CHero_Knolan(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice, pContext)
{
}

CHero_Knolan::CHero_Knolan(const CHero_Knolan & rhs)
	: CPlayer(rhs)
{
}

CGameObject * CHero_Knolan::Get_Weapon_Or_SkillBody()
{
	for (auto& pParts : m_PlayerParts)
	{
		if (dynamic_cast<CHitBoxObject*>(pParts) != nullptr && 
			m_eWeaponType == dynamic_cast<CHitBoxObject*>(pParts)->Get_Type())
		{
			static_cast<CHitBoxObject*>(pParts)->Set_WeaponDamage(m_iStateDamage);
			static_cast<CHitBoxObject*>(pParts)->Set_HitNum(m_iHitCount);
			return pParts;
		}
	}

	return nullptr;
}

_bool CHero_Knolan::Calculator_HitColl(CGameObject * pWeapon)
{
	CHitBoxObject* pCurActorWepon = static_cast<CHitBoxObject*>(pWeapon);
	assert(pCurActorWepon != nullptr && "CHero_Knolan::Calculator_HitColl");

	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{
		m_iGetDamageNum = pCurActorWepon->Get_WeaponDamage();
		m_iHitWeaponOption = static_cast<CHitBoxObject::WEAPON_OPTIONAL>(pCurActorWepon->Get_WeaponOption());

		if (m_bUseDefence == true)
		{
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			vPos.y += 4.f;
			m_pStatusCom[COMBAT_PLAYER]->Take_Damage(_int(m_iGetDamageNum*0.5f));
			CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), _int(m_iGetDamageNum*0.5));

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

HRESULT CHero_Knolan::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHero_Knolan::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Hero_Gully");

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

HRESULT CHero_Knolan::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bControlKeyInput = true;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.23f, 0.f, 0.23f, 1.f));

	m_pTransformCom->Set_Scaled(_float3(0.1f, 0.1f, 0.1f));
	_float4 vPos;
	XMStoreFloat4(&vPos, XMVectorSet(0.23f, 0.f, 0.23f, 1.f));
	m_pNavigationCom->Set_OldPos(vPos);
	m_bLast_Initlize = true;
	return S_OK;
}

void CHero_Knolan::Tick(_double TimeDelta)
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

		if (m_pStatusCom[COMBAT_PLAYER]->Get_Dead() && !m_bIsDead)
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

		if (nullptr != m_pFullscreenEffect)
			m_pFullscreenEffect->Tick(TimeDelta);

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

		//	m_TextureTag = Texture_NameTag;
		//	m_vSkill_Pos = _float4(ffPos[0], ffPos[1], ffPos[2], 1.f);
		//	m_vTestScale = _float3(ffScale[0], ffScale[1], ffScale[2]);

		//	Create_Test_Effect();		// Test용

		//}
		//RELEASE_INSTANCE(CGameInstance);
	}



	m_pModelCom->Play_Animation(TimeDelta, m_bIsCombatScene);
}

void CHero_Knolan::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	for (auto iter = m_PlayerParts.begin(); iter != m_PlayerParts.end();)
	{
		(*iter)->Late_Tick(TimeDelta);

		if (static_cast<CCollider*>((*iter)->Get_Component(L"Com_Collider"))->Get_Coll())
		{
			Safe_Release(*iter);
			iter = m_PlayerParts.erase(iter);
		}
		else
			++iter;
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

		if (m_bIsCombatScene == true)
		{
			if (!m_bIsDead && false ==m_bUltimateBuffRenderStop && CClient_Manager::m_bCombatWin == false)
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

	if (nullptr != m_pFullscreenEffect)
	{
		m_pFullscreenEffect->Late_Tick(TimeDelta);
		if (static_cast<CBuff_Effect*>(m_pFullscreenEffect)->Get_IsFinish())
		{
			Safe_Release(m_pFullscreenEffect);
			m_pFullscreenEffect = nullptr;
		}
	}

	if (m_bModelRender	&& nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#ifdef  _DEBUG
		CClient_Manager::Collider_Render(this, m_pColliderCom, m_pRendererCom);
		CClient_Manager::Navigation_Render(this, m_pNavigationCom, m_pRendererCom);
		if (m_bIsCombatScene)
			m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
#endif //  _DEBUG


	}
}

HRESULT CHero_Knolan::Render()
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
	//#ifdef _DEBUG
	//	CClient_Manager::Collider_Render(this, m_pColliderCom);
	//	CClient_Manager::Navigation_Render(this, m_pNavigationCom);
	//	if (m_bIsCombatScene)
	//		m_pColliderCom->Render();
	//
	//#endif
	return S_OK;
}

void CHero_Knolan::Change_Level_Data(_uint iLevleIdx)
{
	/* 모델 로드는 게임플레이 레벨에서하고있음*/
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
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_KnolanDungeon"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert(!"Change_Level_Data : LEVEL_COMBAT");

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

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_KnolanCombat"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert(!"Change_Level_Data : LEVEL_COMBAT");
		m_bIsCombatScene = true;

	}
	else
		return;
	// maybe 다른씬?
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Fsm_Exit()
{
	m_Hero_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	_bool	bRenderTrue = true;
	m_Hero_CombatStateCanvasDelegeter.broadcast(bRenderTrue);
	m_pHitTarget = nullptr;
	CCombatController::GetInstance()->Camera_Zoom_Out();
}

void CHero_Knolan::Intro_Exit()
{
	_bool	bRenderTrue = true;
	m_Hero_CombatStateCanvasDelegeter.broadcast(bRenderTrue);
	CCombatController::GetInstance()->Status_CanvasInit();
}

void CHero_Knolan::Defence_Exit()
{
	if (false == m_bIsDefenceTimer)
		return;

	m_fDefencTimer += (_float)(CClient_Manager::TimeDelta);

	if (m_fDefencTimer >= 1.f)
	{
		m_bIsDefenceTimer = false;
	}

}

_uint CHero_Knolan::Get_AnimationIndex()
{
	return m_pModelCom->Get_AnimIndex();
}

void CHero_Knolan::AnimMove()
{
	m_pModelCom->Set_AnimIndex(m_iAnimIndex);
}

void CHero_Knolan::HighLightChar()
{
	HIGHLIGHT_UIDESC Desc;
	ZeroMemory(&Desc, sizeof HIGHLIGHT_UIDESC);

	Desc.iNumSizeX = 133.f;
	Desc.iNumSizeY = 134.f;;
	Desc.yPos = 5.f;
	Desc.iTextureIndex = 8;
	lstrcpy(Desc.szObjectName, TEXT("UIButton3"));
	m_Hero_DungeonUIDelegeter.broadcast(Desc);
}

void CHero_Knolan::NormalLightCharUI()
{
	HIGHLIGHT_UIDESC Desc;
	ZeroMemory(&Desc, sizeof HIGHLIGHT_UIDESC);

	Desc.iNumSizeX = 133.f;
	Desc.iNumSizeY = 123.f;
	Desc.yPos = 0.f;
	Desc.iTextureIndex = 5;
	lstrcpy(Desc.szObjectName, TEXT("UIButton3"));
	m_Hero_DungeonUIDelegeter.broadcast(Desc);

}

void CHero_Knolan::Dungeon_Tick(_double TimeDelta)
{
	if (IsCaptin() && !CPlayer::KeyInput(TimeDelta, m_pNavigationCom))
		m_iAnimIndex = 0;
	AnimMove();
	CClient_Manager::CaptinPlayer_ColiderUpdate(this, m_pColliderCom, m_pTransformCom);
}

HRESULT CHero_Knolan::Combat_Initialize()
{
	if (m_bCombat_LastInit)
		return S_OK;

	m_pFsmCom = CAnimFsm::Create(this, ANIM_CHAR1);
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
	m_pTransformCom->Set_Scaled(_float3(3.f, 3.f, 3.f));

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-1.7f, 0.f, 23.f, 1.f));

	if (FAILED(Ready_Parts_Combat()))
		return E_FAIL;

	m_bDefence = true;
	m_isWideBuff = true;

	_float4 vPos;
	XMStoreFloat4(&vPos, XMVectorSet(-1.7f, 0.f, 23.f, 1.f));
	_float3 vScale = _float3(3.f, 3.f, 3.f);
	m_pStatusCom[COMBAT_PLAYER]->Set_Combat_PosScale(vPos, vScale);


	m_bCombat_LastInit = true;
	return S_OK;
}

void CHero_Knolan::Combat_Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	Anim_Frame_Create_Control();


	for (auto &pEffect : m_pEffectParts)
	{
		pEffect->Tick(TimeDelta);
	}

	size_t PartSize = m_PlayerParts.size();
	for (size_t i = 0; i < PartSize; ++i)
	{
		m_PlayerParts[i]->Tick(TimeDelta);
	}

	for (size_t i = 0; i < PartSize; ++i)
	{
		if (dynamic_cast<CSkill_TextureObj*>(m_PlayerParts[i]) != nullptr)
		{
			if (static_cast<CSkill_TextureObj*>(m_PlayerParts[i])->Hit_CountIncrease())
				m_iHitCount = 1;
		}
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

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Combat_DeadTick(_double TimeDelta)
{
	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
}

void CHero_Knolan::Create_Skill_Texture()
{
	if (m_pHitTarget == nullptr)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pSkillParts = nullptr;

	CSkill_TextureObj::Skill_Texture_Client			SkillDesc;
	ZeroMemory(&SkillDesc, sizeof(CSkill_TextureObj::Skill_Texture_Client));

	XMStoreFloat4x4(&SkillDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	SkillDesc.ParentTransform = m_pTransformCom;
	SkillDesc.pSocket = m_pModelCom->Get_BonePtr(m_BoneTag.c_str());
	SkillDesc.eType = WEAPON_SKILL;
	SkillDesc.vScale = m_vSkill_Scale;

	SkillDesc.pTraget = m_pHitTarget->Get_Transform();

	SkillDesc.eDir = m_SkillDir;
	SkillDesc.vPosition = m_vSkill_Pos;
	SkillDesc.vAngle = 90.f;
	SkillDesc.iWeaponOption = m_iWeaponOption;

	pSkillParts = pGameInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, false);
	static_cast<CSkill_TextureObj*>(pSkillParts)->Set_Skill_Texture_Client(SkillDesc);

	if (m_SkillDir == Skill_DIR_LISING)
	{
		static_cast<CSkill_TextureObj*>(pSkillParts)->Set_Glow(true, TEXT("Prototype_Component_Texture_Knolan_Golf"), 1);
		static_cast<CSkill_TextureObj*>(pSkillParts)->Set_ShaderPass(6);
	}


	assert(pSkillParts != nullptr && "Create_SkillFire");
	m_PlayerParts.push_back(pSkillParts);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_Skill_Texture_On_Hiter()
{
	if (m_pHitTarget == nullptr)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pSkillParts = nullptr;

	CSkill_TextureObj::Skill_Texture_Client			SkillDesc;
	ZeroMemory(&SkillDesc, sizeof(CSkill_TextureObj::Skill_Texture_Client));

	XMStoreFloat4x4(&SkillDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	SkillDesc.ParentTransform = m_pTransformCom;
	SkillDesc.pSocket = m_pModelCom->Get_BonePtr(m_BoneTag.c_str());
	SkillDesc.eType = WEAPON_SKILL;
	SkillDesc.vScale = m_vSkill_Scale;
	SkillDesc.pTraget = m_pHitTarget->Get_Transform();
	SkillDesc.eDir = m_SkillDir;
	SkillDesc.vPosition = m_vSkill_Pos;
	SkillDesc.vAngle = 90.f;
	SkillDesc.iWeaponOption = m_iWeaponOption;

	pSkillParts = pGameInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, false);
	static_cast<CSkill_TextureObj*>(pSkillParts)->Set_SKill_Texture_Client_Make_Hiter(SkillDesc);

	assert(pSkillParts != nullptr && "Create_SkillFire");
	m_PlayerParts.push_back(pSkillParts);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_Hit_Effect()
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
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_SKILL2:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
			iEffectNum = 5;
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_SKILL1:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(4.f, 4.f, 4.f);
			iEffectNum = 1;
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_NORMAL:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(5.f, 5.f, 5.f);
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PULPLE:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_10", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(5.f, 5.f, 5.f);
			break;

		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SPIDER_ATTACK:
			pGameObject = pInstance->Load_Effect(L"Texture_Bleeding_Effect_3", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);		// 이미지 교체필요
			BuffDesc.vScale = _float3(8.f, 8.f, 8.f);
			static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(5);
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SPIDER_HEAD:
			pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_2", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(6.f, 6.f, 6.f);
			break;
		case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SLIME_KING_HEAD:
			pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_4", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(6.f, 6.f, 6.f);
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

void CHero_Knolan::Create_Defence_Effect_And_Action()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(30);
	m_CurAnimqeue.push({ 26, 1.f });
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
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = -90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	m_bOnceCreate = false;
	m_bCreateDefenceTimer = true;
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Use_HpPotion()
{
	_int iRandNum = rand() % 20 + 20;

	m_pStatusCom[COMBAT_PLAYER]->Incrase_Hp(iRandNum);
	m_pStatusCom[DUNGEON_PLAYER]->Use_Item(CStatus::ITEM_HP_POTION);
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 2.f;
	vPos.y += 4.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target0_Font0(vPos,
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

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Use_MpPotion()
{
	_int iRandNum = rand() % 15 + 30;

	m_pStatusCom[DUNGEON_PLAYER]->Use_Item(CStatus::ITEM_MP_POSION);
	m_pStatusCom[COMBAT_PLAYER]->Incrase_Mp(iRandNum);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 2.f;
	vPos.y += 4.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target0_Font0(vPos,
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

	RELEASE_INSTANCE(CGameInstance);



}

void CHero_Knolan::Create_Wide_Debuff(CStatus::DEBUFFTYPE eDebuffOption)
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
		_float4(-305.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
		TEXT("Prototype_GameObject_BuffImage"), iTextureNum);
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(eDebuffOption, true);

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 2.f;
	vPos.y += 4.f;
	CExplain_FontMgr::GetInstance()->
		Set_Debuff_Target0_Font(vPos, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());
}

void CHero_Knolan::Create_Defence_Area()
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


	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 2.f;
	vPos.y += 4.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target0_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("shiled"));


	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_WideBuffEffect()
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
			pGameObject = pInstance->Load_Effect(L"Texture_Buff_Effect_4", LEVEL_COMBAT, false);
			BuffDesc.ParentTransform = pActor.second->Get_Transform();
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
			BuffDesc.vAngle = -90.f;
			BuffDesc.fCoolTime = 5.f;
			BuffDesc.bIsMainTain = false;
			BuffDesc.iFrameCnt = 4;
			BuffDesc.bIsUp = false;
			static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
			m_pEffectParts.push_back(pGameObject);
			pCombatCtr->HPMp_Update(pActor.second);
		}
	}

	RELEASE_INSTANCE(CCombatController);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_Wide_BuffEffect_Second()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	CCombatController* pCombatCtr = GET_INSTANCE(CCombatController);

	map<const wstring, CGameObject*> CurMap = *(pCombatCtr->Get_CurActorMap());

	for (auto& pActor : CurMap)
	{
		if (dynamic_cast<CPlayer*>(pActor.second) != nullptr)
		{
			pGameObject = pInstance->Load_Effect(L"Texture_blue_fire_Effect_1", LEVEL_COMBAT, false);
			BuffDesc.ParentTransform = pActor.second->Get_Transform();
			BuffDesc.vPosition = _float4(-0.4f, 1.5f, 0.f, 1.f);
			BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
			BuffDesc.vAngle = -90.f;
			BuffDesc.fCoolTime = 5.f;
			BuffDesc.bIsMainTain = false;
			BuffDesc.iFrameCnt = 4;
			BuffDesc.bIsUp = true;
			static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
			m_pEffectParts.push_back(pGameObject);

			CStatus* pStatus = static_cast<CStatus*>(pActor.second->Get_Component(TEXT("Com_StatusCombat")));
			assert(nullptr != pStatus && "CHero_Calibretto::Create_Wide_BuffEffect_Second");
			static_cast<CCombatActors*>(pActor.second)->WideBuff_Status(pStatus, 1, 50);
			CCombatController::GetInstance()->HPMp_Update(pActor.second);
		}
	}

	RELEASE_INSTANCE(CCombatController);
	RELEASE_INSTANCE(CGameInstance);



	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 2.f;
	vPos.y += 4.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target0_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("mana up"));
}

void CHero_Knolan::Create_SkillFire()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pSkillParts = nullptr;

	CSkill_TextureObj::Skill_Texture_Client			SkillDesc;
	ZeroMemory(&SkillDesc, sizeof(CSkill_TextureObj::Skill_Texture_Client));

	XMStoreFloat4x4(&SkillDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	SkillDesc.ParentTransform = m_pTransformCom;
	SkillDesc.pSocket = m_pModelCom->Get_BonePtr(m_BoneTag.c_str());
	SkillDesc.eType = WEAPON_SKILL;
	SkillDesc.vScale = m_vTestScale;

	//SkillDesc.pTraget = m_pHitTarget->Get_Transform();

	SkillDesc.eDir = SKILL_TEST;
	SkillDesc.vPosition = m_vSkill_Pos;
	SkillDesc.vAngle = 90.f;

	//pSkillParts = pGameInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, false);
	CGameObject * pGameObject = pGameInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, true);
	static_cast<CSkill_TextureObj*>(pGameObject)->Set_Skill_Texture_Client(SkillDesc);

	/*assert(pSkillParts != nullptr && "Create_SkillFire");
	m_PlayerParts.push_back(pSkillParts);*/

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_Test_Effect()
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
	//static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_Skill_Ultimate_Effect0()
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
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
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

void CHero_Knolan::Create_Buff_MainTain_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	pGameObject = pInstance->Load_Effect(TEXT("Texture_Buff_Effect_Power"), LEVEL_COMBAT, false);
	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-1.0f, 0.6f, -0.6f, 1.f);
	BuffDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
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

void CHero_Knolan::Create_Ultimate_Start_CamEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	m_pFullscreenEffect = nullptr;

	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pFullscreenEffect = pInstance->Load_Effect(L"Blood_Rect_Effect", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(-6.18f,21.552f,-12.71f,1.f);
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

void CHero_Knolan::Create_Ultimate_StartFog_CamEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	m_pFog = nullptr; 
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pFog = pInstance->Load_Effect(TEXT("UltiCam_Sprites_Effect_Cloud"), LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(3.15f, 4.64f, 23.84f, 1.f);
	BuffDesc.vScale = _float3(40.f, 30.f, 40.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;


	static_cast<CBuff_Effect*>(m_pFog)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(m_pFog)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_Heavy_Hit_Effect()
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
	CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), iRandom);

	vPos.x -= 2.f;
	CExplain_FontMgr::GetInstance()->Set_Explain_Target0_Font0(vPos,
		_float3(2.f, 2.f, 2.f), TEXT("critical"));

	m_pStatusCom[COMBAT_PLAYER]->Take_Damage(iRandom);
	CCombatController::GetInstance()->HPMp_Update(this);

	RELEASE_INSTANCE(CGameInstance);

}

void CHero_Knolan::Anim_Frame_Create_Control()
{

	if (m_bUseDefence == true && !m_bOnceCreate && 
		m_pModelCom->Control_KeyFrame_Create(6, 43))
	{
		Create_Hit_Effect();
		m_bOnceCreate = true;
		m_bIs_Multi_Hit = false;

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 4.f;
		_int iRandom = rand() % 5;
		CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), iRandom + _int(m_iGetDamageNum*0.5));

	}
	else if (!m_bOnceStop && m_pModelCom->Control_KeyFrame_Create(7, 15))
	{
		Create_Skill_Texture();
		m_bOnceStop = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(7, 64))
	{
		size_t PartSize = m_PlayerParts.size();
		for (size_t i = 0; i < PartSize; ++i)
		{
			if (dynamic_cast<CSkill_TextureObj*>(m_PlayerParts[i]) != nullptr)
			{
				dynamic_cast<CSkill_TextureObj*>(m_PlayerParts[i])->Set_Shoot(true);
			}
		}
		m_bOnceCreate = true;
		m_bOnceStop = false;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(9, 4))
	{
		CCombatController::GetInstance()->Camera_Shaking();
		Create_Hit_Effect();

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 4.f;
		_int iRandom = rand() % 5;
		CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), iRandom + m_iGetDamageNum);

		m_bOnceCreate = true;
		m_bOnceStop = false;
	}
	else if (!m_bOnceStop &&m_pModelCom->Control_KeyFrame_Create(24, 10))
	{

		size_t PartSize = m_PlayerParts.size();
		for (size_t i = 0; i < PartSize; ++i)
		{
			if (dynamic_cast<CSkill_TextureObj*>(m_PlayerParts[i]) != nullptr)
			{
				dynamic_cast<CSkill_TextureObj*>(m_PlayerParts[i])->Set_ChaserBone(false);
			}
		}
		m_bOnceStop = true;
	}
	else if (!m_bOnceCreate  && m_pModelCom->Control_KeyFrame_Create(24, 68))
	{
		m_bOnceCreate = true;
		m_bOnceStop = false;
		size_t PartSize = m_PlayerParts.size();
		for (size_t i = 0; i < PartSize; ++i)
		{
			if (dynamic_cast<CSkill_TextureObj*>(m_PlayerParts[i]) != nullptr)
			{
				dynamic_cast<CSkill_TextureObj*>(m_PlayerParts[i])->Set_Shoot(true);
			}
		}
	}
	else if (!m_bOnceCreate &&m_pModelCom->Control_KeyFrame_Create(21, 53))
	{
		Create_Skill_Texture_On_Hiter();
		m_bOnceCreate = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(27, 90))
	{
		Create_Wide_BuffEffect_Second();
		m_bOnceCreate = true;
	}
	else if (!m_bFogStart && m_pModelCom->Control_KeyFrame_Create(28, 10))
	{
		Create_Ultimate_StartFog_CamEffect();
		CCombatController::GetInstance()->Load_CamBG2();
		m_bFogStart = true;
	}
	else if (!m_bIsUseUltimate && m_pModelCom->Control_KeyFrame_Create(28, 40))
	{
		Safe_Release(m_pFog);
		m_pFog = nullptr;
		m_bUltimateBuffRenderStop = false;
		CCombatController::GetInstance()->Set_Ultimate_End(true);
		m_bIsUseUltimate = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(28, 60))
	{
		Create_Skill_Ultimate_Effect0();
		m_bOnceCreate = true;
		m_bIsUseUltimate = false;
	}
	else if (!m_bUltimateCam && m_pModelCom->Control_KeyFrame_Create(28, 85))
	{
		Create_Ultimate_Start_CamEffect();
		m_bUltimateCam = true;
	}
	else if (!m_bIsUseUltimate && m_pModelCom->Control_KeyFrame_Create(28, 100))
	{
		CCombatController::GetInstance()->Wide_Attack(false, 83);
		m_bIsUseUltimate = true;
	}


	else if (m_bUseDefence == true && !m_bOnceCreate 
		&& m_pModelCom->Control_KeyFrame_Create(26, 20))
	{
		Create_Defence_Area();
		CCombatController::GetInstance()->Camera_Zoom_Out();
		m_bOnceCreate = true;
	}
	else if (m_bOriginBuff == true && !m_bOnceCreateBuff 
		&& m_pModelCom->Control_KeyFrame_Create(26, 40))
	{
		Create_Buff_MainTain_Effect();
		CCombatController::GetInstance()->Camera_Zoom_Out();
		m_bOnceCreate = true;
	}
	else
		return;
}

void CHero_Knolan::Boss_Ultimate_Anim()
{
	m_CurAnimqeue.push({ 4,  3.f });	// 3 or 4
	Set_CombatAnim_Index(m_pModelCom);
}

HRESULT CHero_Knolan::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_KnolanDungeon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.8f, 1.7f, 0.7f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.2f, ColliderDesc.vSize.y * 0.5f, 0.f);

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
	ZeroMemory(&StatusDesc, sizeof(StatusDesc));
	StatusDesc.iHp = 150;
	StatusDesc.iMp = 250;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusDungeon"),
		(CComponent**)&m_pStatusCom[DUNGEON_PLAYER], &StatusDesc)))
		return E_FAIL;

	//m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_HP_POTION,1);
	/*m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_MP_POSION, 20);
	m_pStatusCom[DUNGEON_PLAYER]->Add_ItemID(CStatus::ITEM_ULTIMATE_BOOK, 1);*/

	return S_OK;
}

HRESULT CHero_Knolan::SetUp_ShaderResources()
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

HRESULT CHero_Knolan::Ready_Parts_Combat()
{
	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	ZeroMemory(&StatusDesc, sizeof(CStatus::StatusDesc));
	StatusDesc.iHp = 20;
	StatusDesc.iMp = 250;
	StatusDesc.iExp = 0;
	StatusDesc.iLevel = 1;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusCombat"),
		(CComponent**)&m_pStatusCom[COMBAT_PLAYER], &StatusDesc)))
		return E_FAIL;


	return S_OK;
}

void CHero_Knolan::Calculator_HitDamage()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 2.f;
	vPos.y += 4.f;
	//CStatus::DEBUFF_TYPE_Desc	eDebuffType = m_pStatusCom[COMBAT_PLAYER]->Get_DebuffType();

	if (Is_DebuffBlend(m_pStatusCom[COMBAT_PLAYER], m_iHitWeaponOption, &m_iGetDamageNum, m_DebuffName))
	{
		_float4 vPos2;
		XMStoreFloat4(&vPos2, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos2.x -= 2.f;
		vPos2.y += 5.f;
		CExplain_FontMgr::GetInstance()->
			Set_Debuff_Target0_Font(vPos2, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());
	}
	else if (m_iGetDamageNum >= 50)
	{
		m_bIsHeavyHit = true;
		CExplain_FontMgr::GetInstance()->Set_Explain_Target0_Font1(vPos, _float3(1.f, 1.f, 1.f), TEXT("critical"));
	}
	CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), m_iGetDamageNum);
	m_pStatusCom[COMBAT_PLAYER]->Take_Damage(m_iGetDamageNum);

	m_pStatusCom[COMBAT_PLAYER]->Take_Damage(m_iGetDamageNum);

	if (m_pStatusCom[COMBAT_PLAYER]->Get_CurStatusHpRatio() <= 0.f)
	{
		CCombatController::GetInstance()->Camera_Shaking();
		m_bIsHeavyHit = true;
	}
	else
		CCombatController::GetInstance()->UI_Shaking(true);

}

void CHero_Knolan::Create_Buff_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Buff_Effect_5", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
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

void CHero_Knolan::Anim_Idle()
{
	m_CurAnimqeue.push({ 2,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Intro()
{
	m_CurAnimqeue.push({ 12, m_IntroTimer });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::AnimNormalAttack()
{
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStateDamage = rand() % 10 + 30 + m_iStage_Buff_DamgaeUP;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SKILL;
	m_vSkill_Pos = _float4(-0.5f, 0.5f, 2.5f, 1.f);
	m_vSkill_Scale = _float3(14.f, 14.f, 14.f);
	m_SkillDir = Skill_DIR_straight;
	m_bOnceCreate = false;
	m_BoneTag = "Weapon_Staff_Classic";
	m_TextureTag = TEXT("FireBall_Knolan");
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_NORMAL;
	CCombatController::GetInstance()->Camera_Zoom_In();
	m_iStage_Buff_DamgaeUP = 0;
	m_bBuffEffectStop = true;

	m_CurAnimqeue.push({ 7, 1.f }); //7 21(제자리),,25(뒤로점프샷)
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Skill1_Attack()
{
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStateDamage = rand() % 10 + 40 + m_iStage_Buff_DamgaeUP;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SKILL;
	m_vSkill_Pos = _float4(-1.2f, 0.6f, 1.9f, 1.f);
	m_vSkill_Scale = _float3(4.f, 4.f, 4.f);
	m_bOnceCreate = false;
	m_TextureTag = TEXT("Texture_Knolan_Golf_Skill_Texture");
	m_BoneTag = "Bone_Knolan_Hand_L";
	m_SkillDir = Skill_DIR_LISING;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_SKILL1;
	Create_Skill_Texture();
	CCombatController::GetInstance()->Camera_Zoom_In();
	m_iStage_Buff_DamgaeUP = 0;
	m_bBuffEffectStop = true;

	m_CurAnimqeue.push({ 24, 1.f }); //24(골프샷)
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Skill2_Attack()
{
	// Hiter에게 이펙트만 생성		//53 frame
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStateDamage = rand() % 10 + 50 + m_iStage_Buff_DamgaeUP;
	m_eWeaponType = WEAPON_SKILL;
	m_vSkill_Pos = _float4(0.f, 1.f, 0.f, 1.f);
	m_iHitCount = 1;
	m_vSkill_Scale = _float3(10.f, 10.f, 10.f);
	m_bOnceCreate = false;
	m_TextureTag = TEXT("Texture_Knolan_Fire_Meteo");
	m_BoneTag = "Bone_Knolan_Hand_L";
	m_SkillDir = SKILL_CREATE_HITER_POS;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(50);
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_SKILL2;
	CCombatController::GetInstance()->Camera_Zoom_In();
	m_iStage_Buff_DamgaeUP = 0;
	m_bBuffEffectStop = true;


	m_CurAnimqeue.push({ 21, 1.f }); // fireStorm
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Uitimate()
{
	m_bUltimateBuffRenderStop = true;
	m_bUltimateCam = false;
	m_bBuffEffectStop = true;
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_bFogStart = false;

	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(50);
	m_CurAnimqeue.push({ 28, 1.f }); // 	60프레임때 광역기 시전
	m_CurAnimqeue.push({ 1,  1.f });
	m_bOnceCreate = false;
	m_bIsUseUltimate = false;


	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Buff()
{
	m_iStage_Buff_DamgaeUP = 10;
	m_bBuffEffectStop = false;
	m_bOnceCreateBuff = false;
	m_bOriginBuff = true;

	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(30);
	m_CurAnimqeue.push({ 26, 1.f });	// 자 버프 
	m_CurAnimqeue.push({ 1,  1.f });

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	vPos.x -= 2.f;
	vPos.y += 4.f;


	CExplain_FontMgr::GetInstance()->Set_Explain_Target0_Font0(vPos,
		_float3(1.f, 1.f, 1.f), TEXT("damage up"));

	CClient_Manager::Create_BuffImage(m_vecBuffImage,
		_float4(-305.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
		TEXT("Prototype_GameObject_BuffImage"), 0);
	m_pStatusCom[COMBAT_PLAYER]->Set_DebuffOption(CStatus::BUFF_DAMAGE, true);

	Create_Buff_Effect();

	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_WideAreaBuff()
{
	//Frame90일때 이펙트 생성
	m_bBuffEffectStop = true;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(60);
	m_CurAnimqeue.push({ 27, 1.f });	// 마나 채워주기
	m_CurAnimqeue.push({ 1,  1.f });
	m_bOnceCreate = false;
	Create_WideBuffEffect();

	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Use_Item()
{

	m_bBuffEffectStop = true;
	m_CurAnimqeue.push({ 14, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Defence()
{
	m_CurAnimqeue.push({ 6,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);

}

void CHero_Knolan::Anim_Light_Hit()
{
	if (m_bIsHeavyHit)
	{
		m_CurAnimqeue.push({ 8,  1.f });	// 8,9,10
		m_bIsHeavyHit = false;
	}
	else if (true == m_bIs_Multi_Hit)
	{
		m_CurAnimqeue.push({ 3,  1.f });	// 3 or 4
		m_CurAnimqeue.push({ 9,  1.f });	// 3 or 4
		m_CurAnimqeue.push({ 1,  1.f });
		m_bOnceCreate = false;
		m_bIs_Multi_Hit = false;
	}
	else
	{
		m_CurAnimqeue.push({ 3,  1.f });	// 3 or 4
		m_CurAnimqeue.push({ 1,  1.f });
	}


	Set_CombatAnim_Index(m_pModelCom);
	Create_Hit_Effect();
}

void CHero_Knolan::Anim_Heavy_Hit()
{
	m_bIsHeavyHit = false;

	m_CurAnimqeue.push({ 10,  1.f });	// 8,9,10
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
	Create_Heavy_Hit_Effect();
}

void CHero_Knolan::Anim_Flee()
{

	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		Safe_Release(*iter);
		iter = m_pEffectParts.erase(iter);
	}
	m_pEffectParts.clear();


	for (auto& pBuffImage : m_vecBuffImage)
		Safe_Release(pBuffImage);

	m_CurAnimqeue.push({ 32, 1.f });
	m_CurAnimqeue.push({ 31, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Die()
{
	m_iTurnCanvasOption = 1;
	m_bIsDead = true;
	m_Hero_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_CurAnimqeue.push({ 5,  1.f });
	m_CurAnimqeue.push({ 23, 1.f });
	Set_CombatAnim_Index(m_pModelCom);


	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		Safe_Release(*iter);
		iter = m_pEffectParts.erase(iter);
	}
	m_pEffectParts.clear();
	CCombatController::GetInstance()->Camera_Shaking();


}

void CHero_Knolan::Anim_Viroty()
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
		m_CurAnimqeue.push({ 34,  1.f });
		m_CurAnimqeue.push({ 35, 1.f });

		m_pStatusCom[COMBAT_PLAYER]->Incrase_Hp(150);
	}
	else
	{
		m_CurAnimqeue.push({ 37,  1.f });
		m_CurAnimqeue.push({ 36, 1.f });
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

CHero_Knolan * CHero_Knolan::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHero_Knolan*		pInstance = new CHero_Knolan(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHero_Knolan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHero_Knolan::Clone(void * pArg)
{
	CHero_Knolan*		pInstance = new CHero_Knolan(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHero_Knolan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHero_Knolan::Free()
{
	__super::Free();

	for (_uint i = 0; i < MAPTYPE_END; ++i)
		Safe_Release(m_pStatusCom[i]);

	for (auto& pObj : m_PlayerParts)
		Safe_Release(pObj);
	m_PlayerParts.clear();

	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		Safe_Release(*iter);
		iter = m_pEffectParts.erase(iter);
	}
	m_pEffectParts.clear();

	Safe_Release(m_pFog);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

_bool CHero_Knolan::Is_Dead()
{
	if (__super::Is_Dead())
	{
		//Anim_Die();
		return true;
	}

	return false;
}

_int CHero_Knolan::Get_RestHpPotion()
{
	return m_pStatusCom[DUNGEON_PLAYER]->Rest_iTemNum(CStatus::ITEM_HP_POTION);
}

_int CHero_Knolan::Get_RestMpPotion()
{
	return m_pStatusCom[DUNGEON_PLAYER]->Rest_iTemNum(CStatus::ITEM_MP_POSION);
}


/* 플레이가 죽었을때랑 살았을때 구분해서 쓰기*/
///*살았을때*/
//m_CurAnimqeue.push({ 13 ,	1.f });
//m_CurAnimqeue.push({ 36,	1.f });

///* 죽었을때*/
////m_CurAnimqeue.push({ 33 ,0.1f });
////m_CurAnimqeue.push({ 13,0.1f });
////m_CurAnimqeue.push({ 36,0.1f });