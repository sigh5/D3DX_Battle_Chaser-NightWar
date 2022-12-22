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

CHero_Garrison::CHero_Garrison(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice,pContext)
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
		if (dynamic_cast<CWeapon*>(pParts) != nullptr)
			return pParts;
	}

	return nullptr;
}

_bool CHero_Garrison::Calculator_HitColl(CGameObject * pWeapon)
{
	CWeapon* pCurActorWepon = static_cast<CWeapon*>(pWeapon);

	if (nullptr == pCurActorWepon)		//나중에 아래것으로
		return false;
	//	assert(pCurActorWepon != nullptr && "CSkeleton_Naked::Calculator_HitColl");

	return pCurActorWepon->Get_Colider()->Collision(m_pColliderCom);
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

	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
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
	
	m_bLast_Initlize = true;
	return S_OK;
}

void CHero_Garrison::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	if (m_bIsCombatScene == false)
		Dungeon_Tick(TimeDelta);
	else
	{
		Combat_Init();

		m_pAnimFsm->Tick(TimeDelta);
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	}

	m_pModelCom->Play_Animation(TimeDelta, m_bIsCombatScene);
}


void CHero_Garrison::Late_Tick(_double TimeDelta)
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
		m_pModelCom->Render(m_pShaderCom, i, 0,"g_BoneMatrices","DN_FR_FishingRod");
	}
#ifdef _DEBUG
	CClient_Manager::Navigation_Render(this, m_pNavigationCom);
	CClient_Manager::Collider_Render(this, m_pColliderCom);
	if (m_bIsCombatScene)
		m_pColliderCom->Render();
#endif
	return S_OK;
}


void CHero_Garrison::Change_Level_Data(_uint iLevleIdx)
{
	CGameInstance *pGameInstance = GET_INSTANCE(CGameInstance);

	Safe_Release(m_pModelCom);
	Remove_component(TEXT("Com_Model"));
	if (LEVEL_GAMEPLAY == iLevleIdx)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GarrisonDungeon"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("CHero_Garrison Change_Level_Data : LEVEL_COMBAT ");

		m_bIsCombatScene = false;

	}
	else if (LEVEL_COMBAT == iLevleIdx)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GarrisonCombat"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("CHero_Garrison Change_Level_Data : LEVEL_COMBAT ");

		m_bIsCombatScene = true;
		m_pTransformCom->Set_Scaled(_float3(2.0f, 2.0f, 2.0f));

		
	}
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
	if (IsCaptin() && !CPlayer::KeyInput(TimeDelta,m_pNavigationCom))
		m_iAnimIndex = 0;
	AnimMove();
	CClient_Manager::CaptinPlayer_ColiderUpdate(this, m_pColliderCom, m_pTransformCom);

}

void CHero_Garrison::Combat_Tick(_double TimeDelta)
{
	if (bResult == ANIM_DIR_STRAIGHT || bResult == ANIM_DIR_BACK)
	{
		MovingAnimControl(TimeDelta);
	}
	else
		CurAnimQueue_Play_Tick(TimeDelta,m_pModelCom);
	
	for (_uint i = 0; i < m_PlayerParts.size(); ++i)
	{
		m_PlayerParts[i]->Tick(TimeDelta);
	}
	Is_MovingAnim();
	CombatAnim_Move(TimeDelta);

}

void CHero_Garrison::Combat_Ultimate(_double TimeDelta)
{
	if (bResult == ANIM_DIR_STRAIGHT || bResult == ANIM_DIR_BACK)
	{
		MovingAnimControl(TimeDelta);
	}
	else
		CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	Is_MovingAnim();
	CombatAnim_Move_Ultimate(TimeDelta);
	
}

void CHero_Garrison::Combat_BlendAnimTick(_double TimeDelta)
{
	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	Is_Skill1MovingAnim();
	CombatAnim_Move(TimeDelta);
}

void CHero_Garrison::Fsm_Exit()
{
	_bool	bRenderTrue = true;
	m_Hero_CombatStateCanvasDelegeter.broadcast(bRenderTrue);
	m_Hero_CombatTurnDelegeter.broadcast(TEst, iTestNum);
	m_pHitTarget = nullptr;
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

	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;


	/* For.Lights */
	/*const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;*/
	//
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

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
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(4.f, 0.f, 16.f, 1.f));


	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//m_CurAnimqeue.push({ 0,1.f });
	//Set_CombatAnim_Index(m_pModelCom);

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_bCombatInit = true;
	return S_OK;
}

HRESULT CHero_Garrison::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Weapon_Sword_Classic");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.f, 0.f, -2.f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(0.5f, 2.f, 0.5f,0.f));

	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

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

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pAnimFsm);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

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
	if (m_pHitTarget == nullptr || bResult == ANIM_EMD)
		return;

	_float4 Target;
	XMStoreFloat4(&Target, m_pHitTarget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));

	if (bResult == ANIM_DIR_STRAIGHT)
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(XMLoadFloat4(&Target), TImeDelta, m_LimitDistance, m_SpeedRatio);

	else if (bResult == ANIM_DIR_BACK)
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(m_vOriginPos, TImeDelta, m_ReturnDistance, m_SpeedRatio);
		
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
	m_SpeedRatio = 7.f;
	m_LimitDistance = 10.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 0.9f;

	m_CurAnimqeue.push({ 4,  m_setTickForSecond });
	m_CurAnimqeue.push({ 3,  1.f });
	m_CurAnimqeue.push({ 11, m_setTickForSecond });
	m_CurAnimqeue.push({ 12, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Skill1_Attack()
{
	m_SpeedRatio = 7.f;
	m_LimitDistance = 10.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 0.9f;

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
	m_LimitDistance = 12.f;
	m_SpeedRatio = 6.f;
	m_ReturnDistance = 0.5f;
	m_setTickForSecond = 0.9f;

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
	m_LimitDistance = 12.f;
	m_SpeedRatio = 6.f;
	m_ReturnDistance = 0.5f;
	m_setTickForSecond = 0.9f;

	m_CurAnimqeue.push({ 30,  1.f });	// Key프레임 (뛰는 것)하나 찾기 83~94 는 움직여야함 
	m_CurAnimqeue.push({ 11,  m_setTickForSecond });
	m_CurAnimqeue.push({ 12,  1.f });
	m_CurAnimqeue.push({ 1,   1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Buff()
{
	m_CurAnimqeue.push({ 8,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}



void CHero_Garrison::Anim_Use_Item()
{
	m_CurAnimqeue.push({ 23,  1.f });
	m_CurAnimqeue.push({ 1,   1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Defence()
{
	m_CurAnimqeue.push({ 32,   1.f });
	m_CurAnimqeue.push({ 1,    1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Light_Hit()
{
	m_CurAnimqeue.push({ 26, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Heavy_Hit()
{
	_uint iHitRand = rand() % 4 + 13;
	m_CurAnimqeue.push({ iHitRand ,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Flee()
{
	m_CurAnimqeue.push({ 48,  1.f });
	m_CurAnimqeue.push({ 47,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Die()
{
	m_CurAnimqeue.push({ 7,  1.f });
	m_CurAnimqeue.push({ 33,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Viroty()
{
	Is_Dead();
	Set_CombatAnim_Index(m_pModelCom);
}

_bool CHero_Garrison::Is_Dead()
{
	
	//ToDo .. 다른 애님 추가
	/* if 플레이어 죽음 아니면 살음*/
	m_CurAnimqeue.push({ 22 ,1.f });
	m_CurAnimqeue.push({ 43,1.f });
	

	return true;
}
