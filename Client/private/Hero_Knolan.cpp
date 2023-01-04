#include "stdafx.h"
#include "..\public\Hero_Knolan.h"

#include "GameInstance.h"
#include "Client_Manager.h"

#include "PlayerController.h"
#include "CombatController.h"
#include "AnimFsm.h"
#include "Weapon.h"

#include "Skill_Object.h"
#include "Bone.h"
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
		if (dynamic_cast<CHitBoxObject*>(pParts) != nullptr && m_eWeaponType == dynamic_cast<CHitBoxObject*>(pParts)->Get_Type())
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

	if (nullptr == pCurActorWepon)		//나중에 아래것으로
		return false;
	//	assert(pCurActorWepon != nullptr && "CSkeleton_Naked::Calculator_HitColl");

	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{
		if (m_bUseDefence == true)
			m_pStatusCom[COMBAT_PLAYER]->Take_Damage(_int(pCurActorWepon->Get_WeaponDamage()*0.5));
		else
			m_pStatusCom[COMBAT_PLAYER]->Take_Damage(pCurActorWepon->Get_WeaponDamage());

		if (m_pStatusCom[COMBAT_PLAYER]->Get_CurStatusHpRatio() <= 0.f)
		{
			m_bIsHeavyHit = true;
		}

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


	//Temp[3] = ( 0.f,0.f,0.f );
	//Sour[3] = ( m_vTestScale.x,m_vTestScale.y,m_vTestScale.z );

	//ImGui::InputFloat3("vPos", Temp);
	//ImGui::InputFloat3("vScale", Sour);

	//if (ImGui::Button("Skill"))
	//{
	//	Create_Skill_Stop_Fire();
	//}

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

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
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
#ifdef _DEBUG
	CClient_Manager::Collider_Render(this, m_pColliderCom);
	CClient_Manager::Navigation_Render(this, m_pNavigationCom);
	if (m_bIsCombatScene)
		m_pColliderCom->Render();

#endif
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
			assert("Change_Level_Data : LEVEL_COMBAT ");

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
			vPos = m_pStatusCom[COMBAT_PLAYER]->Get_CombatPos();
			vScale = m_pStatusCom[COMBAT_PLAYER]->Get_CombatScale();
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
			m_pTransformCom->Set_Scaled(vScale);
		}

		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_KnolanCombat"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("Change_Level_Data : LEVEL_COMBAT ");
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
		Fsm_Exit();
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
	m_pTransformCom->Set_Scaled(_float3(4.f, 4.f, 4.f));

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(-2.f, 0.f, 26.f, 1.f));

	if (FAILED(Ready_Parts_Combat()))
		return E_FAIL;

	m_bDefence = true;
	m_isWideBuff = true;

	_float4 vPos;
	XMStoreFloat4(&vPos, XMVectorSet(-2.f, 0.f, 26.f, 1.f));
	_float3 vScale = _float3(4.f, 4.f, 4.f);
	m_pStatusCom[COMBAT_PLAYER]->Set_Combat_PosScale(vPos, vScale);


	m_bCombat_LastInit = true;
	return S_OK;
}

void CHero_Knolan::Combat_Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	Anim_Frame_Create_Control();

	size_t PartSize = m_PlayerParts.size();
	for (size_t i = 0; i < PartSize; ++i)
	{
		m_PlayerParts[i]->Tick(TimeDelta);

	}
	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Combat_DeadTick(_double TimeDelta)
{
	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
}

void CHero_Knolan::Create_SkillFire()
{
	if (m_pHitTarget == nullptr)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pSkillParts = nullptr;

	CSkill_Object::SKILL_OBJ_DESC			SkillDesc;
	ZeroMemory(&SkillDesc, sizeof(CSkill_Object::SKILL_OBJ_DESC));

	XMStoreFloat4x4(&SkillDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	SkillDesc.ParentTransform = m_pTransformCom;
	SkillDesc.pSocket = m_pModelCom->Get_BonePtr("Weapon_Staff_Classic");
	SkillDesc.eType = WEAPON_SKILL;
	lstrcpy(SkillDesc.pModelTag, TEXT("Prototype_Component_FireBall"));
	SkillDesc.pTraget = m_pHitTarget->Get_Transform();
	SkillDesc.eDir = m_SkillDir;
	SkillDesc.vPosition = m_vSkill_Pos;
	SkillDesc.vAngle = 90.f;

	pSkillParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Obj"), &SkillDesc);
	assert(pSkillParts != nullptr && "Create_SkillFire");
	m_PlayerParts.push_back(pSkillParts);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_Skill_Stop_Fire()
{
	if (m_pHitTarget == nullptr)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pSkillParts = nullptr;

	CSkill_Object::SKILL_OBJ_DESC			SkillDesc;
	ZeroMemory(&SkillDesc, sizeof(CSkill_Object::SKILL_OBJ_DESC));

	XMStoreFloat4x4(&SkillDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	SkillDesc.ParentTransform = m_pTransformCom;
	SkillDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Knolan_Hand_L");
	SkillDesc.eType = WEAPON_SKILL;
	lstrcpy(SkillDesc.pModelTag, TEXT("Prototype_Component_FireBall"));
	SkillDesc.pTraget = m_pHitTarget->Get_Transform();
	SkillDesc.vAngle = -180.f;

	SkillDesc.vPosition = m_vSkill_Pos;
	SkillDesc.eDir = m_SkillDir;
	pSkillParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Obj"), &SkillDesc);
	assert(pSkillParts != nullptr && "Create_SkillFire");
	m_PlayerParts.push_back(pSkillParts);

	RELEASE_INSTANCE(CGameInstance);
}

void CHero_Knolan::Create_Skill_Meteo()
{
	if (m_pHitTarget == nullptr)
		return;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pSkillParts = nullptr;

	CSkill_Object::SKILL_OBJ_DESC			SkillDesc;
	ZeroMemory(&SkillDesc, sizeof(CSkill_Object::SKILL_OBJ_DESC));

	XMStoreFloat4x4(&SkillDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	SkillDesc.ParentTransform = m_pTransformCom;
	SkillDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Knolan_Hand_L");
	SkillDesc.eType = WEAPON_SKILL;
	lstrcpy(SkillDesc.pModelTag, TEXT("Prototype_Component_Meteo0"));
	SkillDesc.pTraget = m_pHitTarget->Get_Transform();
	SkillDesc.vAngle = -180.f;

	SkillDesc.vPosition = m_vSkill_Pos;
	SkillDesc.eDir = m_SkillDir;
	pSkillParts = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Skill_Obj"), &SkillDesc);
	assert(pSkillParts != nullptr && "Create_SkillFire");
	m_PlayerParts.push_back(pSkillParts);

	RELEASE_INSTANCE(CGameInstance);


}

void CHero_Knolan::Anim_Frame_Create_Control()
{
	if (m_pModelCom->Control_KeyFrame_Create(7, 64) && !m_bOnceCreate)
	{
		Create_SkillFire();
		m_bOnceCreate = true;
	}
	else if (m_pModelCom->Control_KeyFrame_Create(24, 71) && !m_bOnceCreate)
	{
		m_bOnceCreate = true;

		size_t PartSize = m_PlayerParts.size();
		for (size_t i = 0; i < PartSize; ++i)
		{
			if (dynamic_cast<CSkill_Object*>(m_PlayerParts[i]) != nullptr)
			{
				dynamic_cast<CSkill_Object*>(m_PlayerParts[i])->Set_Shoot(true);
			}
		}
	}
	else if (m_pModelCom->Control_KeyFrame_Create(19, 50) && !m_bOnceCreate)
	{
		size_t PartSize = m_PlayerParts.size();
		for (size_t i = 0; i < PartSize; ++i)
		{
			if (dynamic_cast<CSkill_Object*>(m_PlayerParts[i]) != nullptr)
			{
				dynamic_cast<CSkill_Object*>(m_PlayerParts[i])->Set_Shoot(true);
			}
		}
		m_bOnceCreate = true;

	}
	else
		return;
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
	StatusDesc.iHp = 150;
	StatusDesc.iMp = 250;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusDungeon"),
		(CComponent**)&m_pStatusCom[DUNGEON_PLAYER], &StatusDesc)))
		return E_FAIL;

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

	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;




	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CHero_Knolan::Ready_Parts_Combat()
{
	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	StatusDesc.iHp = 150;
	StatusDesc.iMp = 250;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusCombat"),
		(CComponent**)&m_pStatusCom[COMBAT_PLAYER], &StatusDesc)))
		return E_FAIL;


	return S_OK;
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
	m_iStateDamage = 30;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SKILL;
	m_vSkill_Pos = _float4(0, 0, -1.5f, 1.f);
	m_SkillDir = CSkill_Object::Skill_DIR_straight;
	m_bOnceCreate = false;

	//_uint iHitRand = rand() % 4 + 13;
	m_CurAnimqeue.push({ 7, 1.f }); //7 21(제자리),,25(뒤로점프샷)
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Skill1_Attack()
{
	m_iStateDamage = 40;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_vSkill_Pos = _float4(0.7f, 0.6f, 2.f, 1.f);
	m_SkillDir = CSkill_Object::Skill_DIR_LISING;
	m_iHitCount = 1;
	m_bOnceCreate = false;
	Create_Skill_Stop_Fire();


	m_CurAnimqeue.push({ 24, 1.f }); //24(골프샷)
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Skill2_Attack()
{
	m_iStateDamage = 40;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(50);
	m_vSkill_Pos = _float4(0.7f, 0.6f, 2.f, 1.f);
	m_SkillDir = CSkill_Object::Skill_DIR_ScaleUP_DOWN;
	m_iHitCount = 1;
	m_bOnceCreate = false;
	Create_Skill_Meteo();



	m_CurAnimqeue.push({ 19, 0.5f }); // fireStorm
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Uitimate()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(50);
	m_CurAnimqeue.push({ 28, 1.f }); // 28 전체공격력 업
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Buff()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(30);
	m_CurAnimqeue.push({ 26, 1.f });	// 자 버프 
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_WideAreaBuff()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(60);
	m_CurAnimqeue.push({ 27, 1.f });	// 마나 채워주기
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Use_Item()
{
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
	}
	else
	{
		m_CurAnimqeue.push({ 4,  1.f });	// 3 or 4
		m_CurAnimqeue.push({ 1,  1.f });
	}
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Heavy_Hit()
{
	m_CurAnimqeue.push({ 8,  1.f });	// 8,9,10
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Flee()
{
	m_CurAnimqeue.push({ 32, 1.f });
	m_CurAnimqeue.push({ 31, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Die()
{
	m_iTurnCanvasOption = 1;
	m_Hero_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_CurAnimqeue.push({ 5,  1.f });
	m_CurAnimqeue.push({ 23, 1.f });
	Set_CombatAnim_Index(m_pModelCom);

}

void CHero_Knolan::Anim_Viroty()
{
	//Is_Dead();
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


/* 플레이가 죽었을때랑 살았을때 구분해서 쓰기*/
///*살았을때*/
//m_CurAnimqeue.push({ 13 ,	1.f });
//m_CurAnimqeue.push({ 36,	1.f });

///* 죽었을때*/
////m_CurAnimqeue.push({ 33 ,0.1f });
////m_CurAnimqeue.push({ 13,0.1f });
////m_CurAnimqeue.push({ 36,0.1f });