#include "stdafx.h"
#include "..\public\Hero_Calibretto.h"

#include "GameInstance.h"
#include "Client_Manager.h"

#include "AnimFsm.h"
#include "PlayerController.h"
#include "CombatController.h"


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

	if (nullptr == pCurActorWepon)		//³ªÁß¿¡ ¾Æ·¡°ÍÀ¸·Î
		return false;
	//	assert(pCurActorWepon != nullptr && "CSkeleton_Naked::Calculator_HitColl");

	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{
		m_pStatusCom[COMBAT_PLAYER]->Take_Damage(pCurActorWepon->Get_WeaponDamage());
		if (m_pStatusCom[COMBAT_PLAYER]->Get_CurStatusHpRatio() <= 0.f)
			m_bIsHeavyHit = true;

		return true;
	}
	return false;
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
	
		if (m_pStatusCom[COMBAT_PLAYER]->Get_Dead() && !m_bIsDead)
		{
			m_bIsDead = true;
		}
	}

	m_pModelCom->Play_Animation(TimeDelta, m_bIsCombatScene);
}

void CHero_Calibretto::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	if (m_bIsCombatScene)
	{
		for (_uint i = 0; i < m_PlayerParts.size(); ++i)
		{
			m_PlayerParts[i]->Late_Tick(TimeDelta);
		}
	}

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CHero_Calibretto::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_bIsCombatScene)
		m_pModelCom->Set_NoRenderMeshIndex(10);	//  º£ÀÌ½º¹«±â  ±×¸®Áö¸»±â

	else
		m_pModelCom->Set_NoRenderMeshIndex(2);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (_uint i = m_iNonRenderMeshIndex; i < iNumMeshes; ++i)
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


void CHero_Calibretto::Change_Level_Data(_uint iLevleIdx)
{
	CGameInstance *pGameInstance = GET_INSTANCE(CGameInstance);

	Safe_Release(m_pModelCom);
	Remove_component(TEXT("Com_Model"));
	if (LEVEL_GAMEPLAY == iLevleIdx)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CalibrettoDungeon"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("CHero_Calibretto Change_Level_Data : LEVEL_COMBAT ");

		m_bIsCombatScene = false;

	}
	else if (LEVEL_COMBAT == iLevleIdx)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CalibrettoCombat"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("CHero_Calibretto Change_Level_Data : LEVEL_COMBAT ");
		m_bIsCombatScene = true;
		m_pTransformCom->Set_Scaled(_float3(2.0f, 2.0f, 2.0f));
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
	AnimMove();
	CClient_Manager::CaptinPlayer_ColiderUpdate(this, m_pColliderCom, m_pTransformCom);

}

HRESULT CHero_Calibretto::Combat_Initialize()
{
	if (m_bCombat_LastInit)
		return S_OK;

	m_pFsmCom = CAnimFsm::Create(this, ANIM_CHAR3);

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(135.f));
	m_pTransformCom->Set_Scaled(_float3(4.f, 4.f, 4.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(4.f, 0.f, 32.f, 1.f));
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_pTransformCom->Set_TransfromDesc(7.f, 90.f);


	if (FAILED(Ready_Parts_Combat()))
		return E_FAIL;


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

	for (_uint i = 0; i < m_PlayerParts.size(); ++i)
	{
		m_PlayerParts[i]->Tick(TimeDelta);
	}
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



	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
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
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(0.5f, 2.f, 0.5f, 0.f));
	WeaponDesc.eType = WEAPON_HAND;

	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);


	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	StatusDesc.iHp = 100;
	StatusDesc.iMp = 125;
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
	m_SpeedRatio = 8.f;
	m_LimitDistance = 12.f;
	m_ReturnDistance = 0.4f;
	m_setTickForSecond = 0.9f;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_HAND;
	m_iStateDamage = 350;
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
	m_iStateDamage = 20;			// ÆÝÄ¡ÇÏ°í ÃÑ°¥±â±â
	m_SpeedRatio = 6.f;
	m_LimitDistance = 8.f;
	m_ReturnDistance = 0.4f;
	m_setTickForSecond = 0.9f;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_HAND;
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
	m_SpeedRatio = 8.f;
	m_LimitDistance = 15.f;
	m_ReturnDistance = 0.4f;
	m_setTickForSecond = 0.9f;
	m_iStateDamage = 20;			// 20*2 
	
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_CurAnimqeue.push({ 15, 1.f });
	m_CurAnimqeue.push({ 3,	 1.f });
	m_CurAnimqeue.push({ 20, 1.f });		// Âð °ø°Ý±Ã Brust??
	m_CurAnimqeue.push({ 9,  1.f });
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Uitimate()
{
	m_iStateDamage = 50;			// 20*2 
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(100);
	m_CurAnimqeue.push({ 46, 1.f }); // À§¿¡¼­ ·ÎÄÏ½î±â
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Buff()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_CurAnimqeue.push({ 7,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_WideAreaBuff()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(50);
	m_CurAnimqeue.push({ 40, 1.f });		// ÆÀ¿ø ´ëÆøÈú			
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Use_Item()
{
	m_CurAnimqeue.push({ 19, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Light_Hit()
{
	if (m_bIsHeavyHit)
	{
		m_CurAnimqeue.push({ 11, 1.f }); //,10
	}
	else 
	{
		m_CurAnimqeue.push({ 4 , 1.f });		// 4 or 5
		m_CurAnimqeue.push({ 1,  1.f });
	}

	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Heavy_Hit()
{
	m_CurAnimqeue.push({ 11, 1.f });		// 11,12,13,14
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Flee()
{
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
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Calibretto::Anim_Viroty()
{
	//Is_Dead();
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

	for (_uint i = 0; i < MAPTYPE_END; ++i)
		Safe_Release(m_pStatusCom[i]);


	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}


_bool CHero_Calibretto::Is_Dead()
{
	if (__super::Is_Dead())
	{
		//Anim_Die();
		return true;
	}

	return false;
}

_int CHero_Calibretto::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 3)
		bResult = ANIM_DIR_STRAIGHT;
	else if (m_pModelCom->Get_AnimIndex() == 9)
		bResult = ANIM_DIR_BACK;
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

}

void CHero_Calibretto::Intro_Exit()
{
	m_iNonRenderMeshIndex = 8;
}
