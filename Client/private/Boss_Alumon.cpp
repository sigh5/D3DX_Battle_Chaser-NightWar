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
#include "Trail_Effect.h"

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

		if (nullptr != m_pFog)
			m_pFog->Tick(TimeDelta);

		Create_Whip_Trail();
	}
	
	static float ffPos[3] = {};
	static float ffScale[3] = {};
	static char  szName[MAX_PATH] = "";
	static char  szBoneName[MAX_PATH] = "";
	ImGui::InputFloat3("SkillPos", ffPos);
	ImGui::InputFloat3("SkillScale", ffScale);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	ImGui::InputText("TextureName", szName, MAX_PATH);
	ImGui::InputText("BoneName", szBoneName, MAX_PATH);
	if (ImGui::Button("Create_Effect"))
	{	
		_tchar Texture_NameTag[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, 0, szName, strlen(szName) + 1, Texture_NameTag, MAX_PATH);

		m_TextureTag = Texture_NameTag;
		m_vSkill_Pos = _float4(ffPos[0], ffPos[1], ffPos[2], 1.f);
		m_vTestScale = _float3(ffScale[0], ffScale[1], ffScale[2]);
		m_BoneTag = szBoneName;

		Create_Test_Effect();		// Test
		//Create_Test_MeshEffect();
	}

	RELEASE_INSTANCE(CGameInstance);
	m_pModelCom->Play_Animation(TimeDelta, true);

	if (m_bUltiAttackStart)
	{
		_bool bCheck = false;
		m_bUltimateOneTimer += (_float)TimeDelta;

		if (m_bUltimateOneTimer >= 5.f && !m_bUltimateActiveOne)
		{
			m_bUltimateBuffRenderStop = false;
			m_iAnimMondeShaderPass = 0;
			m_bUltimateActiveOne = true;
		}
		else if (!m_bUltimateActivetwo && m_bUltimateOneTimer >= 7.4f)
		{
			if (m_pHitTarget != nullptr)
				static_cast<CCombatActors*>(m_pHitTarget)->Set_AnimShaderPass(2);
			m_bUltimateActivetwo = true;
		}
		else if (m_bUltimateOneTimer >= 8.3f)
		{
			if (m_pHitTarget != nullptr)
			{
				static_cast<CCombatActors*>(m_pHitTarget)->Set_WideAttackDamage(80);
				static_cast<CCombatActors*>(m_pHitTarget)->Set_FsmState(true, m_Heavy_Hit);
				static_cast<CCombatActors*>(m_pHitTarget)->Set_AnimShaderPass(0);
			}
			m_bUltimateOneTimer = 0.f;
			m_bUltiAttackStart = false;
			m_bUltimateActiveOne = false;
			m_bUltimateActivetwo = false;
			m_bUltimateActiveThree = true;
			CCombatController::GetInstance()->UI_Shaking(true);
		}
	}
	
	if (m_bUltimateActiveThree)
	{
		Create_BuffEffect();
		m_bUltimateActiveThree = false;
		m_pStatusCom->Incrase_Hp(40);
		m_pStatusCom->Incrase_Mp(40);
		CCombatController::GetInstance()->HPMp_Update(this);
	}
	

	if (m_bUltimateBuffRenderStop)
	{
		if (m_fGlowStrength >= 1.f)
		{
			m_bIsChange = true;
		}
		else if (m_fGlowStrength <= 0)
		{
			m_bIsChange = false;
			//return;
		}
		if (m_bIsChange == true)
			m_fGlowStrength += (_float)TimeDelta * -1.f * 0.22f;
		else
			m_fGlowStrength += (_float)TimeDelta * 0.22f;
	}
	
	

	

	
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
			else if (m_bMaintainEffectErase && true == static_cast<CBuff_Effect*>(*iter)->Get_MainTain())
			{
				Safe_Release(*iter);
				*iter = nullptr;
				iter = m_pEffectParts.erase(iter);
			}
			else
				++iter;
		}
		else if (dynamic_cast<CAttack_Effect_Rect*>((*iter)) != nullptr
			&& static_cast<CAttack_Effect_Rect*>(*iter)->Get_IsFinish())
		{
			Safe_Release(*iter);
			*iter = nullptr;
			iter = m_pEffectParts.erase(iter);
		}
		else
			++iter;
	}


	if (m_bModelRender && false == m_bMonster_Victroys)
	{
		if (false == m_bIsDead)
		{
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


	for (auto &pParts : m_BossParts)
	{
		if (nullptr != pParts)
			pParts->Late_Tick(TimeDelta);
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
		m_pModelCom->Render(m_pShaderCom, i, m_iAnimMondeShaderPass, "g_BoneMatrices", "DN_FR_FishingRod");
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
	m_iHitCount = 0;
	m_bUltimateBuffRenderStop = false;
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

void CBoss_Alumon::Combat_UltimateTick(_double TimeDelta)
{
	if (m_iMovingDir == ANIM_DIR_STRAIGHT || m_iMovingDir == ANIM_DIR_BACK)
	{
		MovingAnimControl(TimeDelta);
	}
	else
		CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	
	Anim_Frame_Create_Control();
	Is_MovingAnim();
	CombatAnim_Move_Ultimate(TimeDelta);
}

void CBoss_Alumon::CombatAnim_Move_Ultimate(_double TimeDelta)
{
	if (m_pHitTarget == nullptr)
		return;

	/*For Dubug*/
	_float	fSpeedRatio = 0.f;
	if (!lstrcmp(m_pHitTarget->Get_ObjectName(), TEXT("Hero_Alumon")))
	{
		fSpeedRatio = 0.13f;
	}
	else if (!lstrcmp(m_pHitTarget->Get_ObjectName(), TEXT("Hero_Calibretto")))
	{
		fSpeedRatio = 0.19f;
	}
	else if (!lstrcmp(m_pHitTarget->Get_ObjectName(), TEXT("Hero_Gully")))
	{
		fSpeedRatio = 0.18f;
	}

	_float4 Target;
	XMStoreFloat4(&Target, m_pHitTarget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
	//if (!m_bOnceCreate)
	//{
	//	
	//	//CCombatController::GetInstance()->Ready_UltimateBoss();

	//	m_bOnceCreate = true;
	//}

	if (false == m_bRun	&& m_pModelCom->Get_AnimIndex() == 31)				// 특수사항
	{
		m_bUltimateOneMoveTimer += (_float)TimeDelta;
		if (m_bUltimateOneMoveTimer <= 5.0f)
		{
			m_pTransformCom->CombatChaseTarget(XMLoadFloat4(&Target), TimeDelta, m_LimitDistance, m_SpeedRatio * fSpeedRatio);
			CCombatController::GetInstance()->Camera_Zoom_In();
		}
		else
		{
			CCombatController::GetInstance()->Camera_Zoom_Out();
			CCombatController::GetInstance()->Set_Ultimate_End(true);
			m_bRun = true;
	
		}
	}
	else if (m_iMovingDir == ANIM_DIR_BACK)
	{
		//m_SpeedRatio = ;
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(m_vOriginPos, TimeDelta, m_ReturnDistance, 10.f);
	}
		
	else
		return;
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
	if (!m_bAttackColliderOn && m_pModelCom->Control_KeyFrame_Create(3, 10))
	{
		m_iHitCount = 1;		// 공격할때
		m_bAttackColliderOn = true;

	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(9, 1))
	{
		static_cast<CCombatActors*>(m_pHitTarget)->Set_AnimShaderPass(2);
		m_bOnceCreate = true;

	}
	else if (!m_bAttackColliderOn && m_pModelCom->Control_KeyFrame_Create(9, 32))
	{
		m_iHitCount = 1;		// 공격할때
		m_bAttackColliderOn = true;
		static_cast<CCombatActors*>(m_pHitTarget)->Set_AnimShaderPass(0);
		
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 6.f;
		vPos.x -= 2.f;
		CExplain_FontMgr::GetInstance()->
			Set_Debuff_Target0_Font(vPos, _float3(1.f, 1.f, 1.f),TEXT("boss hp up"));

		m_pStatusCom->Incrase_Hp(10);
		CCombatController::GetInstance()->HPMp_Update(this);

	}
	else if (!m_bOnceCreate && m_pModelCom->Get_AnimIndex() == 12)
	{
		CCombatController::GetInstance()->Camera_Shaking();
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 6.f;
		_int iRandom = rand() % 5;
		CDamage_Font_Manager::GetInstance()->Set_Damage_Target1_Font(vPos, _float3(2.f, 2.f, 2.f), iRandom + m_iGetDamageNum);

		Create_Hit_Effect();
		m_bOnceCreate = true;

		m_iMultiHitNum = 0;
	}
	else if (m_bStunUltimate && m_pModelCom->Get_AnimIndex() == 17)
	{
		if (!m_bOnceCreate)
		{
			CCombatController::GetInstance()->Ultimate_Camera_On();
			CCombatController::GetInstance()->Load_CamBG2();
			CCombatController::GetInstance()->Camera_Zoom_In();
			m_bOnceCreate = true;
		}
		m_bStun_UltimateTimer += (_float)CClient_Manager::TimeDelta;
		if (m_bStun_UltimateTimer >= 1.1f && !m_bUltimateFirstEffect)
		{
			Create_Skill_UltimateTwo_FirstEffect();
			m_bUltimateFirstEffect = true;
			m_bMaintainEffectErase = false;
		}
		if (m_bStun_UltimateTimer >= 2.f && !m_bUltiAttackStop)
		{
			m_bMaintainEffectErase = true;
			CCombatController::GetInstance()->Camera_Zoom_Out();
			CCombatController::GetInstance()->Set_Ultimate_End(true);
			m_bUltiAttackStop = true;
		}
		else if (m_bStun_UltimateTimer >= 3.f)
		{
			CCombatController::GetInstance()->Camera_Shaking();
			CCombatController::GetInstance()->CombatScene_Broken();
			Create_Skill_UltimateTwo_Effect();
			m_bStunUltimate = false;
			m_bStun_UltimateTimer = 0.f;
		}
	}
	else if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(21, 62))
	{
		m_bOnceCreate = true;
		Create_BuffEffect();
		CCombatController::GetInstance()->Camera_Zoom_Out();
		CCombatController::GetInstance()->Debuff_Miss();

		//CCombatController::GetInstance()->Wide_Debuff(true,)
	}
	else if (!m_bAttackColliderOn && m_pModelCom->Control_KeyFrame_Create(35, 20))
	{
		m_iHitCount = 1;		// 공격할때
		m_bAttackColliderOn = true;
	}
	else if (!m_bOnceCreate && m_pModelCom->Get_AnimIndex() == 31)
	{
		m_iAnimMondeShaderPass = 3;
		m_bUltiAttackStart = true;
		m_bOnceCreate = true;
	}

	else
		return;

}

void CBoss_Alumon::Combat_DeadTick(_double TimeDelta)
{
}

void CBoss_Alumon::Fsm_UltimateOne_Exit()
{
	_bool	bRenderTrue = true;
	m_Monster_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_pHitTarget = nullptr;
	CCombatController::GetInstance()->Set_MonsterSetTarget(false);
	CCombatController::GetInstance()->Debuff_Stun();
	m_iHitCount = 0;
	m_bUltimateBuffRenderStop = false;
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
		BuffDesc.vPosition = _float4(0.f, 2.f, 0.f, 1.f);
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

	if (CCombatController::GetInstance()->Is_MissDebuff())
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.x -= 2.f;
		vPos.y += 4.f;
		_float4 vPos2;
		XMStoreFloat4(&vPos2, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos2.y += 6.f;
		vPos2.x -= 6.f;
		CExplain_FontMgr::GetInstance()->Set_Debuff_Target0_Font(vPos2, _float3(1.f, 1.f, 1.f), TEXT("miss"));
	
		vPos.x -= 2.f;
		vPos.y += 4.f;
		CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), 1);
		m_pStatusCom->Take_Damage(1);
		CCombatController::GetInstance()->Use_MissDebuff();
		RELEASE_INSTANCE(CGameInstance);
		return;
	}


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
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_AlumonSkill_Buff_Effect_1", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.5f, 1.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(12.f, 12.f, 12.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 3;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_Glow(true, TEXT("Prototype_Component_Texture_AlumonSkill"), 7);
	static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(6);

	m_pEffectParts.push_back(pGameObject);
	RELEASE_INSTANCE(CGameInstance);
}

void CBoss_Alumon::Create_Skill_UltimateTwo_FirstEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(TEXT("Texture_AlumonSkill_Effect_particle"), LEVEL_COMBAT, false);
	BuffDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Alumon_Hand_R");
	XMStoreFloat4x4(&BuffDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.0f,-0.0f,1.4f,1.f);	// -0.9 정위치
	BuffDesc.vScale = _float3(7.f, 7.f, 7.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	BuffDesc.bIsDown = true;

	//static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc, BuffDesc.pSocket, BuffDesc.PivotMatrix);
	static_cast<CBuff_Effect*>(pGameObject)->Set_Glow(true, TEXT("Prototype_Component_Texture_AlumonSkill"),6);

	static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(6);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}



void CBoss_Alumon::Create_Skill_UltimateTwo_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	_uint			iEffectNum = 1;
	CAttack_Effect_Rect::Attack_Effec_Client EffectDesc;
	ZeroMemory(&EffectDesc, sizeof(EffectDesc));
	CGameObject* pGameObject = nullptr;
	pGameObject = pInstance->Load_Effect(TEXT("AlumonCrack_Base_Effect"), LEVEL_COMBAT, false);
	assert(nullptr != pGameObject && "CBoss_Alumon::Create_Skill_UltimateTwo_Effect()");
	EffectDesc.fCoolTime = 5.f;
	EffectDesc.bIsMainTain = true;
	EffectDesc.iFrameCnt = 8;
	EffectDesc.iMaxTextureNum = 24;
	EffectDesc.iShaderPass = 9;

	static_cast<CAttack_Effect_Rect*>(pGameObject)->Set_Client_BuffDesc(EffectDesc, false);
	static_cast<CAttack_Effect_Rect*>(pGameObject)->Set_Glow(true, TEXT("Prototype_Component_Texture_AlumonCrack_Glow"), 0);

	m_pEffectParts.push_back(pGameObject);
	RELEASE_INSTANCE(CGameInstance);
}

void CBoss_Alumon::Create_Skill_Ultimate_Effect()
{
}

void CBoss_Alumon::Create_Test_MeshEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CMesh_Effect::MeshEffcet_Client MeshDesc;
	ZeroMemory(&MeshDesc, sizeof(MeshDesc));
	pGameObject = pInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, true);

	MeshDesc.ParentTransform = m_pTransformCom;
	MeshDesc.vPosition = m_vSkill_Pos;
	MeshDesc.vScale = m_vTestScale;
	MeshDesc.vAngle = 0.f;
	MeshDesc.iFrameCnt = m_iFrameCnt;
	MeshDesc.iMaxNum = m_iMaxCnt;

	/*MeshDesc.bIsMainTain = false;
	MeshDesc.bIsStraight = false;
	MeshDesc.bIsBack = false;
	MeshDesc.bIsUp = false;*/
	static_cast<CMesh_Effect*>(pGameObject)->Set_Client_BuffDesc(MeshDesc);
	static_cast<CMesh_Effect*>(pGameObject)->Set_Glow(true, TEXT("Prototype_Component_Texture_Slime_Ultimate_BubbleOrigin2"));
	RELEASE_INSTANCE(CGameInstance);
}

void CBoss_Alumon::Create_Ultimate_StartFog_CamEffect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	m_pFog = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	m_pFog = pInstance->Load_Effect(TEXT("UltiCam_Sprites_Effect_Cloud"), LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(7.3f, 3.1f, 28.f, 1.f);
	BuffDesc.vScale = _float3(40.f, 30.f, 40.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 8;
	BuffDesc.bIsUp = false;

	static_cast<CBuff_Effect*>(m_pFog)->Set_CamEffect(BuffDesc);
	static_cast<CBuff_Effect*>(m_pFog)->Set_ShaderPass(7);
	RELEASE_INSTANCE(CGameInstance);
}

void CBoss_Alumon::Create_Whip_Trail()
{
	if (nullptr != m_pTrailEffect)
	{
		if (!m_bTraillEffectStartCheck&& m_pModelCom->Control_KeyFrame_Create(3, 8))
		{
			CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
			ZeroMemory(&TrailDesc, sizeof(TrailDesc));
			TrailDesc.pColider = static_cast<CWeapon*>(m_BossParts[1])->Get_Colider();
			TrailDesc.pGameObject = m_BossParts[1];
			TrailDesc.iDevideFixNum = 2;
			TrailDesc.iMaxCenterCount = 10;
			TrailDesc.fSize = 1.f;
			TrailDesc.eType = CTrail_Effect::BOSS_WHIP;
			static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 18);
		}
		else if (!m_bTrailEndCheck&& m_pModelCom->Control_KeyFrame_Create(3, 20))
		{
			m_bTrailEndCheck = true;
			m_bTraillEffectStartCheck = false;
		}
		
		if (!m_bTraillEffectStartCheck&& m_pModelCom->Control_KeyFrame_Create(40, 8))
		{
			CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
			ZeroMemory(&TrailDesc, sizeof(TrailDesc));
			TrailDesc.pColider = static_cast<CWeapon*>(m_BossParts[1])->Get_Colider();
			TrailDesc.pGameObject = m_BossParts[1];
			TrailDesc.iDevideFixNum = 2;
			TrailDesc.iMaxCenterCount = 10;
			TrailDesc.fSize = 0.8f;
			TrailDesc.eType = CTrail_Effect::BOSS_WHIP;
			static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 18);
		}
		else if (!m_bTrailEndCheck&& m_pModelCom->Control_KeyFrame_Create(40, 13))
		{
			m_bTrailEndCheck = true;
			m_bTraillEffectStartCheck = false;
		}

		else if (!m_bTraillEffectStartCheck&& m_pModelCom->Control_KeyFrame_Create(40, 15))
		{
			CTrail_Effect::tag_Trail_Effect_DESC TrailDesc;
			ZeroMemory(&TrailDesc, sizeof(TrailDesc));
			TrailDesc.pColider = static_cast<CWeapon*>(m_BossParts[1])->Get_Colider();
			TrailDesc.pGameObject = m_BossParts[1];
			TrailDesc.iDevideFixNum = 2;
			TrailDesc.iMaxCenterCount = 18;
			TrailDesc.fSize = 0.8f;
			TrailDesc.eType = CTrail_Effect::BOSS_WHIP;
			static_cast<CTrail_Effect*>(m_pTrailEffect)->Set_Desc(TrailDesc, 9, 34);
		}
		else if (!m_bTrailEndCheck&& m_pModelCom->Control_KeyFrame_Create(40, 25))
		{
			m_bTrailEndCheck = true;
			m_bTraillEffectStartCheck = false;
		}
		/*
		else if (!m_bTraillEffectStartCheck&& m_pModelCom->Control_KeyFrame_Create(19, 3))
		{

			CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
			_uint			iEffectNum = 1;
			CBuff_Effect::BuffEffcet_Client BuffDesc;
			ZeroMemory(&BuffDesc, sizeof(BuffDesc));
			CBone* pSocket = m_pModelCom->Get_BonePtr("Weapon_Sword_Classic");

			m_pTrailUVMoveEffect = pInstance->Load_Effect(TEXT("Texture_garrison_burst_0_Effect"), LEVEL_COMBAT, false);
			BuffDesc.ParentTransform = m_pTransformCom;
			BuffDesc.vPosition = _float4(-0.5f, 0.1f, 2.0f, 1.f);
			BuffDesc.vScale = _float3(8.f, 8.f, 8.f);
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
*/
		else
			return;
	}


}

void CBoss_Alumon::Create_Test_Effect()
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
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	static_cast<CBuff_Effect*>(pGameObject)->Set_ShaderPass(5);
	RELEASE_INSTANCE(CGameInstance);
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


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dissoive"), TEXT("Com_Dissolve"),
		(CComponent**)&m_pTexturdissolveCom)))
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
	StatusDesc.iHp = 700;
	StatusDesc.iMp = 600;
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
	
	if (FAILED(m_pTexturdissolveCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 6)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("G_Power", &m_fGlowStrength, sizeof(_float))))
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
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.f, +0.3f, 0.f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(3.f, 2.5f, 0.f, 0.f));
	WeaponDesc.eType = WEAPON_SHILED;
	WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_SHILED;
	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_BossParts.push_back(pPartObject);
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);
	// ~1. 방패

	//// 2. 채찍
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));
	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Alumon_Tail_Whip_10");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.0f, 3.f, 0.f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(0.f, 5.f, 0.f, 0.f));
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
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.5f, 0.4f, -1.9f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(1.f, 2.5f, 1.f, 0.f));
	WeaponDesc.eType = WEAPON_SWORD;
	WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_RIGHT_HAND;
	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;
	m_BossParts.push_back(pPartObject);
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);
	// ~3. 단검


	CHitBoxObject::HitBoxObject  hitBoxDesc;
	lstrcpy(hitBoxDesc.HitBoxOrigin_Desc.m_pTextureTag, TEXT("Prototype_Component_Texture_Trail"));
	hitBoxDesc.Poing_Desc.m_iFrameCnt = 2;
	hitBoxDesc.Poing_Desc.m_iTextureMax_Height_Cnt = 1;
	hitBoxDesc.Poing_Desc.m_iTextureMax_Width_Cnt = 1;
	lstrcpy(hitBoxDesc.m_pTrailObjectName, TEXT("WhipTrail"));

	pGameInstance->Clone_GameObject(LEVEL_COMBAT, LAYER_EFFECT,
		TEXT("Prototype_GameObject_Trail_Effect"), &hitBoxDesc);

	m_pTrailEffect = pGameInstance->
		Get_GameObject(LEVEL_COMBAT, LAYER_EFFECT, TEXT("WhipTrail"));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBoss_Alumon::Calculator_HitDamage()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	vPos.x -= 2.f;
	vPos.y += 4.f;

	if (CCombatController::GetInstance()->Is_MissDebuff())
	{
		_float4 vPos2;
		XMStoreFloat4(&vPos2, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos2.y += 6.f;
		vPos2.x -= 6.f;
		CExplain_FontMgr::GetInstance()->Set_Debuff_Target0_Font(vPos2, _float3(1.f, 1.f, 1.f), TEXT("miss"));
	
		vPos.x -= 2.f;
		vPos.y += 4.f;
		CDamage_Font_Manager::GetInstance()->Set_Damage_Target0_Font(vPos, _float3(2.f, 2.f, 2.f), 1);
		m_pStatusCom->Take_Damage(1);
		CCombatController::GetInstance()->Use_MissDebuff();
		return;
	}


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
	m_CurAnimqeue.push({ 24, 1.f });			// 사운드
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_NormalAttack()	//0
{
	m_iHitCount = 0;
	m_eWeaponType = WEAPON_WHIP;
	m_iStateDamage = rand() % 10 + 25 + m_iStage_Buff_DamgaeUP;
	m_pMeHit_Player = nullptr;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_WHIP;
	m_bRun = false;
	m_bOnceCreate = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStage_Buff_DamgaeUP = 0;
	m_LimitDistance = 10.f;
	m_bAttackColliderOn = false;

	m_CurAnimqeue.push({ 4,  1.f });	// 4 앞으로 가는거
	m_CurAnimqeue.push({ 3,  1.f });	// 짦은 채찍
	m_CurAnimqeue.push({ 10, 1.f });	// 뒤로가기
	m_CurAnimqeue.push({ 11, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_NormalAttack2()//0
{
	m_iHitCount = 0;
	m_eWeaponType = WEAPON_SHILED;
	m_iStateDamage = rand() % 10 + 15 + m_iStage_Buff_DamgaeUP;
	m_pMeHit_Player = nullptr;
	m_iStage_Buff_DamgaeUP = 0;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_SHILED;
	m_bRun = false;
	m_bOnceCreate = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_SpeedRatio = 8.f;
	m_LimitDistance = 10.f;
	m_bAttackColliderOn = false;

	m_CurAnimqeue.push({ 36,  1.f });	//방패들고 앞으로가기
	m_CurAnimqeue.push({ 35,  1.f });	// 방패로 머리찍기
	m_CurAnimqeue.push({ 37, 1.f });	// 방패들고 뒤로가는거
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Skill1_Attack()//0
{
	m_iHitCount = 0;
	m_eWeaponType = WEAPON_SWORD;
	m_iStateDamage = rand() % 10 + 35 + m_iStage_Buff_DamgaeUP;
	m_pMeHit_Player = nullptr;
	m_iStage_Buff_DamgaeUP = 0;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_RIGHT_HAND;
	m_bRun = false;
	m_bOnceCreate = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_SpeedRatio = 8.f;
	m_LimitDistance = 6.f;
	m_pStatusCom->Use_SkillMp(30);
	m_bAttackColliderOn = false;

	m_CurAnimqeue.push({ 36,  1.f });
	m_CurAnimqeue.push({ 9,  1.f });		// 영혼뽑기 , 마나 회복
	m_CurAnimqeue.push({ 37, 1.f });
	m_CurAnimqeue.push({ 1, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Skill2_Attack()//0
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
	m_bAttackColliderOn = false;

	m_CurAnimqeue.push({ 4,  1.f });
	m_CurAnimqeue.push({ 40,  1.f });		// 채찍 두번 휘두르기
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 11, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Uitimate()	
{
	m_fGlowStrength = 1.1f;
	m_bUltimateSoundCheck = false;
	m_bUltimateCam = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStage_Buff_DamgaeUP = 0;
	m_pStatusCom->Use_SkillMp(50);
	m_bRun = false;
	m_bUltimateRun = true;
	m_bOnceCreate = false;
	m_SpeedRatio = 6.f;
	m_LimitDistance = 10.f;
	m_bUltimateOneMoveTimer = 0.f;
	m_iStateDamage = rand() % 10 + 85 + m_iStage_Buff_DamgaeUP;

	m_setTickForSecond = 1.f;
	m_pMeHit_Player = nullptr;
	m_bIsUseUltimate = false;
	m_bUltimateBuffRenderStop = true;
	m_bUltiAttackStart = false;
	m_bUltiAttackStop = false;
	m_fUltimateTimer = 0.f;
	m_eWeaponType = WEAPON_SWORD;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_BOSS_RIGHT_HAND;


	CCombatController::GetInstance()->Ultimate_Camera_OnBoss();
	m_CurAnimqeue.push({ 31,  1.f });	//걸어가고 목적지에서 영혼흡수시작
	//m_CurAnimqeue.push({ 32,  1.f });   //	대기시간
	m_CurAnimqeue.push({ 33, 1.f });	//궁 끝내는 애님
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 11,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CBoss_Alumon::Anim_Uitimate2()	//WideAreaBuff // crack Effect
{
	// 뛰지않음
	m_bUltimateSoundCheck = false;
	m_bUltimateCam = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_iStage_Buff_DamgaeUP = 0;
	m_pStatusCom->Use_SkillMp(50);
	m_bOnceCreate = false;
	m_bStunUltimate = true;

	m_setTickForSecond = 1.f;
	m_pMeHit_Player = nullptr;
	m_bIsUseUltimate = false;
	m_bUltimateBuffRenderStop = true;
	m_bUltiWideAttack = false;
	m_bUltiAttackStart = false;
	m_bUltiAttackStop = false;
	m_fUltimateTimer = 0.f;
	m_eWeaponType = WEAPON_END;
	m_bUltimateFirstEffect = false;
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
	CClient_Manager::Create_BuffImage(m_vecBuffImage,
		_float4(500.f, -245.f, 0.1f, 1.f), _float3(30.f, 30.f, 1.f),
		TEXT("Prototype_GameObject_BuffImage"), 0);
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, true);

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

	Safe_Release(m_pTexturdissolveCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}