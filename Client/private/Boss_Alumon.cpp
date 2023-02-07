#include "stdafx.h"
#include "..\public\Boss_Alumon.h"

#include "GameInstance.h"
#include "BossFsm.h"
#include "CombatController.h"
#include "Weapon.h"
#include "Buff_Effect.h"
#include "Damage_Font_Manager.h"
#include "Explain_FontMgr.h"
#include "Status.h"
#include "HitBoxObject.h"
#include "Buff_Image.h"
#include "Client_Manager.h"
#include "Attack_Effect_Rect.h"
#include "Mesh_Effect.h"
#include "SoundPlayer.h"


CBoss_Alumon::CBoss_Alumon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CBoss_Alumon::CBoss_Alumon(const CBoss_Alumon & rhs)
	: CMonster(rhs)
	, m_pShaderCom{ nullptr }
	, m_pRendererCom{ nullptr }
	, m_pModelCom{ nullptr }
	, m_pColliderCom{ nullptr }
	, m_pStatusCom{ nullptr }
	//, m_pFsmCom{ nullptr }
{
	m_BossParts.clear();
}

CGameObject * CBoss_Alumon::Get_Weapon_Or_SkillBody()
{
	for (auto& pParts : m_BossParts)
	{
		if (dynamic_cast<CHitBoxObject*>(pParts) != nullptr && 
			m_eWeaponType == dynamic_cast<CHitBoxObject*>(pParts)->Get_Type())
		{
			static_cast<CHitBoxObject*>(pParts)->Set_WeaponOption(m_iWeaponOption);
			static_cast<CHitBoxObject*>(pParts)->Set_WeaponDamage(m_iStateDamage);
			static_cast<CHitBoxObject*>(pParts)->Set_HitNum(m_iHitCount);

			return pParts;
		}
	}

	return nullptr;
}

_bool CBoss_Alumon::Calculator_HitColl(CGameObject * pWeapon)
{
	_bool bResult = false;
	CHitBoxObject* pCurActorWepon = static_cast<CHitBoxObject*>(pWeapon);

	if (nullptr == pCurActorWepon)		//나중에 아래것으로
		return false;

	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{
		m_iGetDamageNum = pCurActorWepon->Get_WeaponDamage();
		m_iHitWeaponOption = static_cast<CHitBoxObject::WEAPON_OPTIONAL>(pCurActorWepon->Get_WeaponOption());
		if (pCurActorWepon->Get_HitNum() == 100)	/* 가리손 궁극기 때문에  */
		{
			m_fHitPerSecond = 3.f;
			m_bIsHeavyHit = false;
			m_iSign *= -1;
			return true;
		}
		if (pCurActorWepon->Get_HitNum() > 1)
		{
			m_bIs_Multi_Hit = true;
			m_bOnceCreate = false;
		}
		m_fHitPerSecond = 1.f;
		m_iSign = 1;

		Calculator_HitDamage();
		bResult = true;
	}
	return bResult;
}

_bool CBoss_Alumon::IsCollMouse()
{
	return m_pColliderCom->Collision_Mouse(g_hWnd);
}

_bool CBoss_Alumon::Is_Dead()
{
	if (__super::Is_Dead())
		return true;
	
	return false;
}

HRESULT CBoss_Alumon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CBoss_Alumon::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Boss_Alumon");
	m_vecBuffImage.reserve(5);

	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);
	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-30.f));
	m_pTransformCom->Set_Scaled(_float3(4.f,4.f, 4.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(37.f, 0.f, -4.f, 1.f));
	m_pModelCom->Set_AnimIndex(0);


	m_bHaveSkill2 = true;
	m_bHaveUltimate = true;
	m_bDefence = true;
	m_bHaveBuff = true;
	m_isWideBuff = true;
	m_bMonsterHaveUseItem = true;
	return S_OK;
}

HRESULT CBoss_Alumon::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pFsmCom = CBossFsm::Create(this);
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_bLast_Initlize = true;
	return S_OK;
}

void CBoss_Alumon::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	m_pFsmCom->Tick(TimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_pStatusCom->Get_Dead())
	{
		m_bIsDead = true;
	}
	else
	{
		for (auto &pParts : m_BossParts)
		{
			if (nullptr != pParts)
				pParts->Tick(TimeDelta);
		}
		for (auto &pParts : m_pEffectParts)
		{
			if (nullptr != pParts)
				pParts->Tick(TimeDelta);
		}

		for (auto& pBuffImage : m_vecBuffImage)
		{
			if (pBuffImage != nullptr)
				pBuffImage->Tick(TimeDelta);
		}

	}
	//CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	//queue<pair<_uint, _double>> Temp;
	//CClient_Manager::Make_Anim_Queue(Temp, ANIM_CHAR7);
	//if (ImGui::Button("TestQueue"))
	//{
	//	m_CurAnimqeue = Temp;
	//}


	//static float ffPos[3] = {};
	//static float ffScale[3] = {};
	//static char  szName[MAX_PATH] = "";
	//ImGui::InputFloat3("SkillPos", ffPos);
	//ImGui::InputFloat3("SkillScale", ffScale);
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//ImGui::InputText("TextureName", szName, MAX_PATH);
	//ImGui::InputInt("iFrameCnt", &m_iFrameCnt);
	//ImGui::InputInt("iMaxCnt", &m_iMaxCnt);

	//if (ImGui::Button("Create_Effect"))
	//{	
	//	_tchar Texture_NameTag[MAX_PATH] = TEXT("");
	//	MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName) + 1, Texture_NameTag, MAX_PATH);

	//	m_TextureTag = Texture_NameTag;
	//	m_vSkill_Pos = _float4(ffPos[0], ffPos[1], ffPos[2], 1.f);
	//	m_vTestScale = _float3(ffScale[0], ffScale[1], ffScale[2]);

	//	Create_Test_Effect();		// Test
	//	//Create_Test_MeshEffect();
	//}


	//RELEASE_INSTANCE(CGameInstance);

	m_pModelCom->Play_Animation(TimeDelta, true);
}

void CBoss_Alumon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		if ((*iter) != nullptr)
			(*iter)->Late_Tick(TimeDelta);

		if (dynamic_cast<CBuff_Effect*>((*iter)) != nullptr)
		{
			if (true == static_cast<CBuff_Effect*>(*iter)->Get_IsFinish())
			{
				Safe_Release(*iter);
				*iter = nullptr;
				iter = m_pEffectParts.erase(iter);
			}
			else
				++iter;
		}
		else
			++iter;
	}

	if (m_bModelRender && false == m_bMonster_Victroys)
	{
		if (false == m_bIsDead)
		{
			/*for (auto &pParts : m_BossParts)
			{
				if (nullptr != pParts)
					pParts->Late_Tick(TimeDelta);
			}*/

			CClient_Manager::Sort_BuffImage(m_vecBuffImage, false);
			for (auto& pVecBuffImage : m_vecBuffImage)
			{
				if (pVecBuffImage != nullptr)
					pVecBuffImage->Late_Tick(TimeDelta);
			}
			CClient_Manager::Delete_BuffImage(m_vecBuffImage, m_pStatusCom, false);
		}
	}

	if (m_bIsDead && !m_bClearScene)
	{
		for (auto& pBuffImage : m_vecBuffImage)
			Safe_Release(pBuffImage);
		m_vecBuffImage.clear();
		m_bClearScene = true;
	}

	static float vScale[3];
	ImGui::InputFloat3("VScale", vScale);
	static float vPos[3];
	ImGui::InputFloat3("vPos", vPos);
	static char  szName[MAX_PATH] = "";
	ImGui::InputText("TextureName", szName, MAX_PATH);

	if (ImGui::Button("Pos_Scale Set"))
	{
		for (auto &pParts : m_BossParts)
		{
			Safe_Release(pParts);
		}
		m_BossParts.clear();

		CGameObject*		pPartObject = nullptr;
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		CWeapon::WEAPONDESC			WeaponDesc;
		ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));
		/*_tchar Texture_NameTag[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName) + 1, Texture_NameTag, MAX_PATH);*/

		WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
		WeaponDesc.pSocket = m_pModelCom->Get_BonePtr(szName);
		WeaponDesc.pTargetTransform = m_pTransformCom;
		XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(vPos[0], vPos[1], vPos[2], 1.f));
		XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(vScale[0], vScale[1], vScale[2], 0.f));
		WeaponDesc.eType = WEAPON_SWORD;
		WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PULPLE;
		pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
		if (nullptr == pPartObject)
			return;

		m_BossParts.push_back(pPartObject);
		Safe_AddRef(WeaponDesc.pSocket);
		Safe_AddRef(m_pTransformCom);


	}

	for (auto &pParts : m_BossParts)
	{
		if (nullptr != pParts)
			pParts->Late_Tick(TimeDelta);
	}


	if (m_bModelRender	&& nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
	}
}

HRESULT CBoss_Alumon::Render()
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

HRESULT CBoss_Alumon::Render_ShadowDepth()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, 1, "g_BoneMatrices", "DN_FR_FishingRod");
	}
	return S_OK;
}

void CBoss_Alumon::Initialize_CombatSound()
{
}

void CBoss_Alumon::Fsm_Exit()
{
	_bool	bRenderTrue = true;
	m_Monster_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_pHitTarget = nullptr;
	CCombatController::GetInstance()->Set_MonsterSetTarget(false);
	CCombatController::GetInstance()->Camera_Zoom_Out();
}

void CBoss_Alumon::UltiHeavyHitExit()
{
	if (false == m_bIsDead)
		CCombatController::GetInstance()->Set_MonsterSetTarget(false);
}

void CBoss_Alumon::Combat_Tick(_double TimeDelta)
{
	if (m_iMovingDir == ANIM_DIR_STRAIGHT || m_iMovingDir == ANIM_DIR_BACK)
	{
		MovingAnimControl(TimeDelta);
	}
	else
		CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);

	Anim_Frame_Create_Control();
	Is_MovingAnim();
	CombatAnim_Move(TimeDelta);
}

_int CBoss_Alumon::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 4 
		|| m_pModelCom->Get_AnimIndex() == 36)
		m_iMovingDir = ANIM_DIR_STRAIGHT;
	else if (m_pModelCom->Get_AnimIndex() == 10 
		|| m_pModelCom->Get_AnimIndex() == 37)
		m_iMovingDir = ANIM_DIR_BACK;
	else
		m_iMovingDir = ANIM_EMD;

	return m_iMovingDir;
}

void CBoss_Alumon::CombatAnim_Move(_double TImeDelta)
{
	if (m_pHitTarget == nullptr || m_iMovingDir == ANIM_EMD)
		return;

	if (m_iMovingDir == ANIM_DIR_BACK)
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(m_vOriginPos, TImeDelta, m_ReturnDistance, m_SpeedRatio);

	else if (m_iMovingDir == ANIM_DIR_STRAIGHT)
	{
		_float4 Target;
		XMStoreFloat4(&Target, m_pHitTarget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(XMLoadFloat4(&Target), TImeDelta, m_LimitDistance, m_SpeedRatio);
		if (m_bCombatChaseTarget == true)
		{
			if (static_cast<CCombatActors*>(m_pHitTarget)->Get_UseDeffece())
			{
				m_pHitTarget->Set_FsmState(true, CGameObject::m_Defence);
			}
		}
		CCombatController::GetInstance()->Camera_Zoom_In();
	}
	else
		return;
}

void CBoss_Alumon::MovingAnimControl(_double TimeDelta)
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
			m_bIsCombatAndAnimSequnce = true;
	}
}

void CBoss_Alumon::Anim_Frame_Create_Control()
{
	if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(12, 4))
	{
		Create_Hit_Effect();
		CCombatController::GetInstance()->Camera_Shaking();

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 4.f;

		CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), m_iGetDamageNum);
		m_pStatusCom->Take_Damage(m_iGetDamageNum);
		m_bOnceCreate = true;
		m_iMultiHitNum = 0;
	}
}

void CBoss_Alumon::Combat_DeadTick(_double TimeDelta)
{
}

void CBoss_Alumon::Create_Hit_Effect()
{
	/* 다중 공격 시*/
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;

	_uint			iEffectNum = 0;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	_bool			IsDebuffing = false;
	switch (m_iHitWeaponOption)
	{
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_NONE:
		RELEASE_INSTANCE(CGameInstance);
		return;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BLUE:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_8", LEVEL_COMBAT, false);
		BuffDesc.vPosition = _float4(0.f, 1.0f, 0.f, 1.f);
		BuffDesc.vScale = _float3(8.f, 8.f, 8.f);
		iEffectNum = 1;
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_SKILL2:
		iEffectNum = 5;
		m_pStatusCom->Set_DebuffOption(CStatus::DEBUFFTYPE::DEBUFF_FIRE);		//화염
		m_DebuffName = TEXT("fire down");
		IsDebuffing = true;
		m_eCurDebuff = CStatus::DEBUFFTYPE::DEBUFF_FIRE;

		CClient_Manager::Create_BuffImage(m_vecBuffImage,
			_float4(500.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
			TEXT("Prototype_GameObject_BuffImage"), 3);
		pInstance->Play_Sound(TEXT("Common_Knolan_Skill2_Effect.wav"), 1.f, false, SOUND_TYPE_HIT);

		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_SKILL1:
		pGameObject = pInstance->Load_Effect(L"Texture_DeBuff_Mana_Effect", LEVEL_COMBAT, false);
		BuffDesc.vPosition = _float4(0.f, 1.5f, 0.f, 1.f);
		BuffDesc.vScale = _float3(6.f, 10.f, 6.f);
		m_pStatusCom->Set_DebuffOption(CStatus::DEBUFFTYPE::DEBUFF_MAGIC);		// 마방깎
		iEffectNum = 1;
		m_DebuffName = TEXT("magic down");
		static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(5);
		IsDebuffing = true;
		m_eCurDebuff = CStatus::DEBUFFTYPE::DEBUFF_MAGIC;

		CClient_Manager::Create_BuffImage(m_vecBuffImage,
			_float4(500.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
			TEXT("Prototype_GameObject_BuffImage"), 2);

		pInstance->Play_Sound(TEXT("Common_0012.wav"), 1.f, false, SOUND_TYPE_HIT);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_RED_KNOLAN_NORMAL:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
		BuffDesc.vScale = _float3(5.f, 5.f, 5.f);
		m_pStatusCom->Set_DebuffOption(CStatus::DEBUFFTYPE::DEBUFF_NONE);
		pInstance->Play_Sound(TEXT("Common_0234.wav"), 1.f, false, SOUND_TYPE_HIT);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PUNCH_HIT:
		pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_Impact_Mirror_0", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(1.5f, 1.f, -3.f, 1.f);
		BuffDesc.vScale = _float3(22.f, 22.f, 22.f);
		m_pStatusCom->Set_DebuffOption(CStatus::DEBUFFTYPE::DEBUFF_NONE);
		pInstance->Play_Sound(TEXT("Common_0059.wav"), 1.f, false, SOUND_TYPE_HIT);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_PUNCH_GUN:

		pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_Impact_Mirror_0", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(1.5f, 1.f, -3.f, 1.f);
		BuffDesc.vScale = _float3(22.f, 22.f, 22.f);
		m_pStatusCom->Set_DebuffOption(CStatus::DEBUFFTYPE::DEBUFF_ARMOR);
		m_eCurDebuff = CStatus::DEBUFFTYPE::DEBUFF_ARMOR;
		m_DebuffName = TEXT("armor down");
		CClient_Manager::Create_BuffImage(m_vecBuffImage,
			_float4(500.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
			TEXT("Prototype_GameObject_BuffImage"), 4);
		IsDebuffing = true;

		if (m_iMultiHitNum == 0)
			pInstance->Play_Sound(TEXT("Common_0059.wav"), 1.f, false, SOUND_TYPE_HIT);
		else if (m_iMultiHitNum == 1)
			pInstance->Play_Sound(TEXT("Common_0249.wav"), 1.f, false, SOUND_TYPE_HIT);
		++m_iMultiHitNum;

		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GARRISON_NORMAL:
		pInstance->Play_Sound(TEXT("Common_0047.wav"), 1.f, false, SOUND_TYPE_HIT);
		iEffectNum = 0;
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GARRISON_SKILL1:
		iEffectNum = 0;
		m_pStatusCom->Set_DebuffOption(CStatus::DEBUFFTYPE::DEBUFF_ARMOR);
		m_eCurDebuff = CStatus::DEBUFFTYPE::DEBUFF_ARMOR;
		m_DebuffName = TEXT("armor down");
		CClient_Manager::Create_BuffImage(m_vecBuffImage,
			_float4(500.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
			TEXT("Prototype_GameObject_BuffImage"), 4);
		pInstance->Play_Sound(TEXT("Common_0102.wav"), 1.f, false, SOUND_TYPE_HIT);
		IsDebuffing = true;
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GARRISON_SKILL2:
		iEffectNum = 0;
		m_pStatusCom->Set_DebuffOption(CStatus::DEBUFFTYPE::DEBUFF_BLEED);
		m_eCurDebuff = CStatus::DEBUFFTYPE::DEBUFF_BLEED;
		m_DebuffName = TEXT("bleeding");
		IsDebuffing = true;
		CClient_Manager::Create_BuffImage(m_vecBuffImage,
			_float4(500.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
			TEXT("Prototype_GameObject_BuffImage"), 1);
		pInstance->Play_Sound(TEXT("Common_0047.wav"), 1.f, false, SOUND_TYPE_HIT);
		break;
	case CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_GARRISON_Ultimate:
		m_DebuffName = TEXT("armor break");
		IsDebuffing = true;
		iEffectNum = 0;
		m_pStatusCom->Set_DebuffOption(CStatus::DEBUFFTYPE::DEBUFF_ARMOR);
		CClient_Manager::Create_BuffImage(m_vecBuffImage,
			_float4(500.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
			TEXT("Prototype_GameObject_BuffImage"), 4);
		break;
	default:
		break;
	}

	if (true == IsDebuffing)
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 6.f;
		vPos.x -= 6.f;
		CExplain_FontMgr::GetInstance()->
			Set_Debuff_Target0_Font(vPos, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());
		IsDebuffing = false;
	}

	if (iEffectNum == 0)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (iEffectNum == 1 && pGameObject != nullptr)
	{
		BuffDesc.ParentTransform = m_pTransformCom;
		BuffDesc.vAngle = 90.f;
		BuffDesc.fCoolTime = 2.f;
		BuffDesc.bIsMainTain = false;
		BuffDesc.iFrameCnt = 5;
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

	CCombatController::GetInstance()->Camera_Zoom_Out();
	RELEASE_INSTANCE(CGameInstance);
}

void CBoss_Alumon::Create_Heavy_Hit_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;

	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	pGameObject = pInstance->Load_Effect(L"Texture_Die_Effect_3", LEVEL_COMBAT, false);
	BuffDesc.vPosition = _float4(0.f, 2.0f, 0.f, 1.f);
	BuffDesc.vScale = _float3(8.f, 8.f, 8.f);
	BuffDesc.ParentTransform = m_pTransformCom;

	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 2.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;
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

	m_pStatusCom->Take_Damage(iRandom);
	CCombatController::GetInstance()->HPMp_Update(this);

	RELEASE_INSTANCE(CGameInstance);
}

void CBoss_Alumon::Create_Ultimate_StartCam_Effect()
{
}

void CBoss_Alumon::Create_Move_Target_Effect()
{
}

void CBoss_Alumon::Create_BuffEffect()
{
}

void CBoss_Alumon::Create_Skill_Ultimate_Effect()
{
}

void CBoss_Alumon::Create_Test_MeshEffect()
{
}

void CBoss_Alumon::Create_Test_Effect()
{
}

HRESULT CBoss_Alumon::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_COMBAT, TEXT("Prototype_Component_Model_Alumon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.2f, 1.7f, 1.2f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.2f, ColliderDesc.vSize.y * 0.5f, 0.f);
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	ZeroMemory(&StatusDesc, sizeof(CStatus::StatusDesc));
	StatusDesc.iHp = 1000;
	StatusDesc.iMp = 250;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusCombat"),
		(CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_Alumon::SetUp_ShaderResources()
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

	m_pShaderCom->Set_Matrix("g_matLightView", &pGameInstance->Get_Light_Matrix(TEXT("Level_Combat_Directional")));
	m_pShaderCom->Set_Matrix("g_matLightProj", &pGameInstance->Get_Light_ProjMatrix(TEXT("Level_Combat_Directional")));


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBoss_Alumon::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	// 1. 방패
	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Hero_Alumon_Shield_Model");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(1.f, 2.5f, 0.f, 0.f));
	WeaponDesc.eType = WEAPON_SHILED;
	WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_SHILED;
	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_BossParts.push_back(pPartObject);
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);
	// ~1. 방패

	// 2. 채찍
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Alumon_Weapon_Whip_Tip");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(-1.f, 0.f, 0.f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(0.f, -4.f, -2.f, 0.f));
	WeaponDesc.eType = WEAPON_WHIP;
	WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_WHIP;
	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;
	m_BossParts.push_back(pPartObject);
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);
	// ~2. 채찍

	// 3. 단검
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Alumon_Hand_R");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.5f, 0.5f, 0.3f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(0.f, -2.5f, -2.f, 0.f));
	WeaponDesc.eType = WEAPON_SWORD;
	WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_RIGHT_HAND;
	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;
	m_BossParts.push_back(pPartObject);
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);
	// ~3. 단검



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBoss_Alumon::Calculator_HitDamage()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 2.f;
	vPos.y += 4.f;

	if (Is_DebuffBlend(m_pStatusCom, m_iHitWeaponOption, &m_iGetDamageNum, m_DebuffName))
	{
		_float4 vPos2;
		XMStoreFloat4(&vPos2, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos2.y += 6.f;
		vPos2.x -= 6.f;
		CExplain_FontMgr::GetInstance()->Set_Debuff_Target0_Font(vPos2, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());

	}
	else if (m_iGetDamageNum >= 50)
	{
		m_bIsHeavyHit = true;
		CExplain_FontMgr::GetInstance()->
			Set_Explain_Target0_Font0(vPos, _float3(1.f, 1.f, 1.f), TEXT("critical"));
	}
	CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), m_iGetDamageNum);
	m_pStatusCom->Take_Damage(m_iGetDamageNum);

	if (m_pStatusCom->Get_CurStatusHpRatio() <= 0.f)
	{
		CCombatController::GetInstance()->Camera_Shaking();
		m_bIsHeavyHit = true;
	}
	else
		CCombatController::GetInstance()->UI_Shaking(true);
}

void CBoss_Alumon::Anim_Idle()
{
	m_CurAnimqeue.push({ 2, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Intro()
{
	m_CurAnimqeue.push({ 24, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_NormalAttack()
{
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_WHIP;
	m_iStateDamage = rand() % 10 + 25 + m_iStage_Buff_DamgaeUP;
	m_pMeHit_Player = nullptr;
	m_iStage_Buff_DamgaeUP = 0;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_WHIP;
	m_bRun = false;
	m_bOnceCreate = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_LimitDistance = 14.f;


	m_CurAnimqeue.push({ 4,  1.f });	// 4 앞으로 가는거
	m_CurAnimqeue.push({ 3,  1.f });	// 짦은 채찍
	m_CurAnimqeue.push({ 10, 1.f });	// 뒤로가기
	m_CurAnimqeue.push({ 11, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_NormalAttack2()
{
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SHILED;
	m_iStateDamage = rand() % 10 + 15 + m_iStage_Buff_DamgaeUP;
	m_pMeHit_Player = nullptr;
	m_iStage_Buff_DamgaeUP = 0;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_SHILED;
	m_bRun = false;
	m_bOnceCreate = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_LimitDistance = 6.5f;


	m_CurAnimqeue.push({ 36,  1.f });	//방패들고 앞으로가기
	m_CurAnimqeue.push({ 35,  1.f });	// 방패로 머리찍기
	m_CurAnimqeue.push({ 37, 1.f });	// 방패들고 뒤로가는거
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Skill1_Attack()
{
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SWORD;
	m_iStateDamage = rand() % 10 + 35 + m_iStage_Buff_DamgaeUP;
	m_pMeHit_Player = nullptr;
	m_iStage_Buff_DamgaeUP = 0;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_RIGHT_HAND;
	m_bRun = false;
	m_bOnceCreate = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_LimitDistance = 6.5f;
	m_pStatusCom->Use_SkillMp(30);

	m_CurAnimqeue.push({ 36,  1.f });
	m_CurAnimqeue.push({ 9,  1.f });		// 영혼뽑기 , 마나 회복
	m_CurAnimqeue.push({ 37, 1.f });
	m_CurAnimqeue.push({ 1, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Skill2_Attack()
{
	m_iHitCount = 2;
	m_eWeaponType = WEAPON_WHIP;
	m_iStateDamage = rand() % 10 + 35 + m_iStage_Buff_DamgaeUP;
	m_pMeHit_Player = nullptr;
	m_iStage_Buff_DamgaeUP = 0;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_WHIP;
	m_bRun = false;
	m_bOnceCreate = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_LimitDistance = 14.f;
	m_pStatusCom->Use_SkillMp(30);


	m_CurAnimqeue.push({ 4,  1.f });
	m_CurAnimqeue.push({ 40,  1.f });		// 채찍 두번 휘두르기
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 11, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Uitimate()
{
	m_bUltimateSoundCheck = false;
	m_bUltimateCam = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_pStatusCom->Use_SkillMp(50);
	m_bRun = false;
	m_bUltimateRun = true;
	m_bOnceCreate = false;
	m_SpeedRatio = 6.f;
	m_LimitDistance = 3.f;

	m_setTickForSecond = 1.f;
	m_pMeHit_Player = nullptr;
	m_bIsUseUltimate = false;
	m_bUltimateBuffRenderStop = true;
	m_bUltiWideAttack = false;
	m_bUltiAttackStart = false;
	m_bUltiAttackStop = false;
	m_fUltimateTimer = 0.f;
	m_eWeaponType = WEAPON_END;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_ULTIMATE_ONE;


	m_CurAnimqeue.push({ 31,  1.f });	//걸어가고 목적지에서 영혼흡수시작
	//m_CurAnimqeue.push({ 32,  1.f });   //	대기시간
	m_CurAnimqeue.push({ 33, 1.f });	//궁 끝내는 애님
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 11,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Uitimate2()
{
	// 뛰지않음
	m_bUltimateSoundCheck = false;
	m_bUltimateCam = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_pStatusCom->Use_SkillMp(50);
	m_bOnceCreate = false;

	m_setTickForSecond = 1.f;
	m_pMeHit_Player = nullptr;
	m_bIsUseUltimate = false;
	m_bUltimateBuffRenderStop = true;
	m_bUltiWideAttack = false;
	m_bUltiAttackStart = false;
	m_bUltiAttackStop = false;
	m_fUltimateTimer = 0.f;
	m_eWeaponType = WEAPON_END;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_ULTIMATE_TWO;


	//m_CurAnimqeue.push({ 18,  1.f });	//대기시간
	m_CurAnimqeue.push({ 17,  1.f }); // 땅바닥 내려치기
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}


void CBoss_Alumon::Anim_Defence()
{
	m_CurAnimqeue.push({ 8,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Buff()
{
	m_iStage_Buff_DamgaeUP = 10;
	m_bOnceCreate = false;
	m_useBuff = true;
	m_pStatusCom->Use_SkillMp(10);

	m_CurAnimqeue.push({ 21,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Light_Hit()
{
	++m_iHitNum;
	if (m_bIsHeavyHit)
	{
		m_CurAnimqeue.push({ 13 ,  1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_bIsHeavyHit = false;
	}
	else if (true == m_bIs_Multi_Hit)
	{
		m_CurAnimqeue.push({ 6,  1.f });
		m_CurAnimqeue.push({ 12,  1.f });	// 여기를 애님프레임 컨트롤에 넣어야됌
		m_CurAnimqeue.push({ 1,  1.f });
		m_bOnceCreate = false;
		m_bIs_Multi_Hit = false;
	}
	else
	{
		if (m_iSign == 1)
			m_CurAnimqeue.push({ 19 , m_fHitPerSecond });
		else
			m_CurAnimqeue.push({ 6 , m_fHitPerSecond });
	}

	Set_CombatAnim_Index(m_pModelCom);
	Create_Hit_Effect();
}

void CBoss_Alumon::Anim_Heavy_Hit()
{
	++m_iHitNum;
	m_bIsHeavyHit = false;
	m_CurAnimqeue.push({ 13 ,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });

	Set_CombatAnim_Index(m_pModelCom);
	Create_Heavy_Hit_Effect();
}

void CBoss_Alumon::Anim_Die()
{
	m_iTurnCanvasOption = 1;
	m_Monster_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);

	m_CurAnimqeue.push({ 7 ,  1.f });
	m_CurAnimqeue.push({ 22,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Viroty()
{
}

CBoss_Alumon * CBoss_Alumon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Alumon*		pInstance = new CBoss_Alumon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_Alumon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBoss_Alumon::Clone(void * pArg)
{
	CBoss_Alumon*		pInstance = new CBoss_Alumon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_Alumon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_Alumon::Free()
{
	__super::Free();

	for (auto& pPart : m_BossParts)
		Safe_Release(pPart);
	m_BossParts.clear();


	Safe_Release(m_pStatusCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}