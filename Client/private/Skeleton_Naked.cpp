#include "stdafx.h"
#include "..\public\Skeleton_Naked.h"

#include "GameInstance.h"
#include "MonsterFsm.h"

#include "CombatController.h"
#include "Weapon.h"
#include "Buff_Effect.h"
#include "Damage_Font_Manager.h"
CSkeleton_Naked::CSkeleton_Naked(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CSkeleton_Naked::CSkeleton_Naked(const CSkeleton_Naked & rhs)
	: CMonster(rhs)
	, m_pShaderCom{ nullptr }
	, m_pRendererCom{ nullptr }
	, m_pModelCom{ nullptr }
	, m_pColliderCom{ nullptr }
	, m_pStatusCom{ nullptr }
	, m_pFsmCom{ nullptr }
{
	m_MonsterParts.clear();
}

CGameObject * CSkeleton_Naked::Get_Weapon_Or_SkillBody()
{
	for (auto& pParts : m_MonsterParts)
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

_bool CSkeleton_Naked::Calculator_HitColl(CGameObject * pWeapon)
{

	_bool bResult = false;
	CHitBoxObject* pCurActorWepon = static_cast<CHitBoxObject*>(pWeapon);

	if (nullptr == pCurActorWepon)		//나중에 아래것으로
		return false;
	//	assert(pCurActorWepon != nullptr && "CSkeleton_Naked::Calculator_HitColl");
	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{


		m_iGetDamageNum = pCurActorWepon->Get_WeaponDamage();
		m_pStatusCom->Take_Damage(m_iGetDamageNum);

		if (m_pStatusCom->Get_CurStatusHpRatio() <= 0.f)
			m_bIsHeavyHit = true;

		m_iHitWeaponOption = pCurActorWepon->Get_WeaponOption();

		if (pCurActorWepon->Get_HitNum() > 1)
		{
			m_bIs_Multi_Hit = true;
			m_bOnceCreate = false;
		}

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 4.f;
		CDamage_Font_Manager::GetInstance()->Set_DamageFont(vPos, _float3(2.f, 2.f, 2.f), m_iGetDamageNum);
		CCombatController::GetInstance()->UI_Shaking(true);
		bResult = true;
	}


	return bResult;
}

HRESULT CSkeleton_Naked::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSkeleton_Naked::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Skeleton_Naked");

	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);
	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-30.f));
	m_pTransformCom->Set_Scaled(_float3(3.f, 3.f, 3.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(25.84f, 0.f, -4.28f, 1.f));

	m_pModelCom->Set_AnimIndex(0);


	m_bHaveSkill2 = false;
	m_bHaveUltimate = false;
	m_bDefence = true;
	m_isBuff = true;
	return S_OK;
}
HRESULT CSkeleton_Naked::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pFsmCom = CMonsterFsm::Create(this, ANIM_CHAR2);
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_bLast_Initlize = true;
	return S_OK;
}

void CSkeleton_Naked::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	m_pFsmCom->Tick(TimeDelta);
	for (auto &pParts : m_MonsterParts)
		pParts->Tick(TimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_pStatusCom->Get_Dead() && !m_bIsDead)
	{
		m_bIsDead = true;
	}

	//static float ffPos[3] = {};
	//static float ffScale[3] = {};
	//static char  szName[MAX_PATH] = "";
	//ImGui::InputFloat3("SkillPos", ffPos);
	//ImGui::InputFloat3("SkillScale", ffScale);
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//ImGui::InputText("TextureName", szName, MAX_PATH);
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


	m_pModelCom->Play_Animation(TimeDelta, true);
}

void CSkeleton_Naked::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	for (auto iter = m_MonsterParts.begin(); iter != m_MonsterParts.end();)
	{
		if ((*iter) != nullptr)
			(*iter)->Late_Tick(TimeDelta);

		if (dynamic_cast<CBuff_Effect*>((*iter)) != nullptr)
		{
			if (true == static_cast<CBuff_Effect*>(*iter)->Get_IsFinish())
			{
				Safe_Release(*iter);
				iter = m_MonsterParts.erase(iter);
			}
			else
				++iter;
		}
		else
			++iter;
	}


	if (m_bModelRender	&& nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
	}
}

HRESULT CSkeleton_Naked::Render()
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

void CSkeleton_Naked::Combat_Tick(_double TimeDelta)
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

_int CSkeleton_Naked::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 9)
		m_iMovingDir = ANIM_DIR_STRAIGHT;
	else if (m_pModelCom->Get_AnimIndex() == 15)
		m_iMovingDir = ANIM_DIR_STRAIGHT;
	else if (m_pModelCom->Get_AnimIndex() == 4)
		m_iMovingDir = ANIM_DIR_BACK;
	else
		m_iMovingDir = ANIM_EMD;

	return m_iMovingDir;
}

void CSkeleton_Naked::CombatAnim_Move(_double TImeDelta)
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

void CSkeleton_Naked::MovingAnimControl(_double TimeDelta)
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

void CSkeleton_Naked::Create_Hit_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	WEAPON_OPTIONAL WeaponOption = static_cast<WEAPON_OPTIONAL>(m_iHitWeaponOption);
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));

	switch (WeaponOption)
	{
	case Client::WEAPON_OPTIONAL_NONE:
		RELEASE_INSTANCE(CGameInstance);
		return;
	case Client::WEAPON_OPTIONAL_BLUE:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_8", LEVEL_COMBAT, false);
		BuffDesc.vPosition = _float4(0.f, 1.0f, 0.f, 1.f);
		BuffDesc.vScale = _float3(8.f, 8.f, 8.f);
		iEffectNum = 1;
		break;
	case Client::WEAPON_OPTIONAL_RED_KNOLAN_SKILL2:
		//pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
		iEffectNum = 5;
		break;
	case Client::WEAPON_OPTIONAL_RED_KNOLAN_SKILL1:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
		BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
		BuffDesc.vScale = _float3(4.f, 4.f, 4.f);
		iEffectNum = 1;
		break;
	case Client::WEAPON_OPTIONAL_RED_KNOLAN_NORMAL:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_11", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
		BuffDesc.vScale = _float3(5.f, 5.f, 5.f);
		break;
	case Client::WEAPON_OPTIONAL_PULPLE:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_10", LEVEL_COMBAT, false);
		break;
	case Client::WEAPON_OPTIONAL_GREEN:
		pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_9", LEVEL_COMBAT, false);
		break;
	case Client::WEAPON_OPTIONAL_PUNCH_HIT:
		pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_Impact_Mirror_0", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(1.5f, 1.f, -3.f, 1.f);
		BuffDesc.vScale = _float3(22.f, 22.f, 22.f);
		break;
	case Client::WEAPON_OPTIONAL_PUNCH_GUN:
		pGameObject = pInstance->Load_Effect(L"Texture_Monster_Bite_Impact_Mirror_0", LEVEL_COMBAT, false);
		iEffectNum = 1;
		BuffDesc.vPosition = _float4(1.5f, 1.f, -3.f, 1.f);
		BuffDesc.vScale = _float3(22.f, 22.f, 22.f);
		break;
	case Client::WEAPON_OPTIONAL_END:
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
		m_MonsterParts.push_back(pGameObject);
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
			m_MonsterParts.push_back(pGameObject);
			iSignNum *= -1;
		}
	}


	RELEASE_INSTANCE(CGameInstance);

}

void CSkeleton_Naked::Create_Heacy_Hit_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;

	WEAPON_OPTIONAL WeaponOption = static_cast<WEAPON_OPTIONAL>(m_iHitWeaponOption);
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
	m_MonsterParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);

}

void CSkeleton_Naked::Anim_Frame_Create_Control()
{
	if (!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(6, 1))
	{
		Create_Hit_Effect();
		CCombatController::GetInstance()->Camera_Shaking();
		_int iRandom = rand() % 5 + 10;
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 4.f;
		CDamage_Font_Manager::GetInstance()->Set_DamageFont(vPos, _float3(2.f, 2.f, 2.f), iRandom + m_iGetDamageNum);
		m_pStatusCom->Take_Damage(iRandom + m_iGetDamageNum);
		m_bOnceCreate = true;
	}
	else if (!m_bRun && m_pModelCom->Control_KeyFrame_Create(8, 1) )
	{
		Create_Move_Target_Effect();
		m_bRun = true;
	}
	else if(!m_bOnceCreate && m_pModelCom->Control_KeyFrame_Create(17, 68))
	{
		_int iRandom = rand() % 10 + 50;
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.y += 4.f;
		CDamage_Font_Manager::GetInstance()->Set_DamageFont(vPos, _float3(2.f, 2.f, 2.f), iRandom);
		m_pStatusCom->Take_Damage(iRandom);
		CCombatController::GetInstance()->HPMp_Update(this);
		
		m_bOnceCreate = true;
	}

	else
	{
		return;
	}
}

void CSkeleton_Naked::Multi_Hit_Effect(CGameInstance* pInstance)
{
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(CBuff_Effect::BuffEffcet_Client));
	_int iSignNum = 1;
	CGameObject* pGameObject = nullptr;
	for (_uint i = 0; i < 5; ++i)
	{
		_int iRandomNum = rand() % 5;
		pGameObject = pInstance->Load_Effect(L"Texture_bretto_Real_Bullet_Effect_0", LEVEL_COMBAT, false);
		BuffDesc.ParentTransform = m_pTransformCom;
		BuffDesc.vPosition = _float4(_float(iRandomNum*0.2f * iSignNum), 0.3f, 1.2f, 1.f);
		BuffDesc.vScale = _float3(4.f, 4.f, 4.f);
		BuffDesc.vAngle = 90.f;
		BuffDesc.fCoolTime = 2.f;
		BuffDesc.bIsMainTain = false;
		BuffDesc.iFrameCnt = rand() % 4 + 3;
		BuffDesc.bIsUp = false;
		static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
		m_MonsterParts.push_back(pGameObject);
		iSignNum *= -1;
	}
}

void CSkeleton_Naked::Create_Move_Target_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject = pInstance->Load_Effect(L"Texture_Jump_Right_To_Left_3", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 0.5f, -1.f, 1.f);
	BuffDesc.vScale = _float3(4.f, 4.f, 4.f);
	BuffDesc.vAngle = 90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 1;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_MonsterParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);

}


void CSkeleton_Naked::Create_Test_Effect()
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
	BuffDesc.vAngle = -90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 5;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	RELEASE_INSTANCE(CGameInstance);
}

void CSkeleton_Naked::Fsm_Exit()
{
	m_Monster_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_pHitTarget = nullptr;
	CCombatController::GetInstance()->Set_MonsterSetTarget(false);
	CCombatController::GetInstance()->Camera_Zoom_Out();
}

_bool CSkeleton_Naked::IsCollMouse()
{
	return m_pColliderCom->Collision_Mouse(g_hWnd);
}

HRESULT CSkeleton_Naked::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_COMBAT, TEXT("Prototype_Component_Model_Skeleton_Naked"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.2f, 1.7f, 1.2f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.2f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	StatusDesc.iHp = 250;
	StatusDesc.iMp = 125;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusCombat"),
		(CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSkeleton_Naked::SetUp_ShaderResources()
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

HRESULT CSkeleton_Naked::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();

	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Weapon_Club");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.f, 0.f, -1.f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(0.5f, 1.5f, 0.5f, 0.f));
	WeaponDesc.eType = WEAPON_SWORD;
	WeaponDesc.iWeaponOption = WEAPON_OPTIONAL_PULPLE;
	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_MonsterParts.push_back(pPartObject);
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CSkeleton_Naked::Anim_Idle()
{
	m_CurAnimqeue.push({ 0, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Intro()
{
	m_CurAnimqeue.push({ 13, 1.f });		//이때 안보이게 뭐를 설정해야함
	m_CurAnimqeue.push({ 12, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_NormalAttack()
{
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SWORD;
	m_iStateDamage = rand() % 10 + 20;
	m_pMeHit_Player = nullptr;
	m_iWeaponOption = WEAPON_OPTIONAL_RED_KNOLAN_SKILL2;
	m_bRun = false;

	m_CurAnimqeue.push({ 8, m_setTickForSecond });	// 한대툭
	m_CurAnimqeue.push({ 9, 1.f });
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 4, m_setTickForSecond });
	m_CurAnimqeue.push({ 5, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Skill1_Attack()
{
	m_iHitCount = 2;
	m_iStateDamage = rand() % 10 + 20;
	m_pStatusCom->Use_SkillMp(30);
	m_pMeHit_Player = nullptr;
	m_bRun = false;
	m_iWeaponOption = WEAPON_OPTIONAL_RED_KNOLAN_SKILL2;

	m_CurAnimqeue.push({ 8, m_setTickForSecond });	// 2대 툭
	m_CurAnimqeue.push({ 15, 1.f });
	m_CurAnimqeue.push({ 16, 0.5f });
	m_CurAnimqeue.push({ 4, m_setTickForSecond });
	m_CurAnimqeue.push({ 5, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Defence()
{
	m_CurAnimqeue.push({ 14, 1.f });
	m_pStatusCom->Use_SkillMp(40);
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Buff()
{
	m_pStatusCom->Use_SkillMp(10);
	m_CurAnimqeue.push({ 3, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Light_Hit()
{
	++m_iHitNum;
	if (m_bIsHeavyHit)
	{
		m_CurAnimqeue.push({ 6, 1.f }); //,10
	}
	else if (true == m_bIs_Multi_Hit)
	{
		m_CurAnimqeue.push({ 7,  1.f });
		m_CurAnimqeue.push({ 6,  1.f });

		m_bOnceCreate = false;
		m_bIs_Multi_Hit = false;
	}
	else
	{
		m_CurAnimqeue.push({ 7 , 1.f });
	}

	Set_CombatAnim_Index(m_pModelCom);
	Create_Hit_Effect();
}

void CSkeleton_Naked::Anim_Heavy_Hit()
{
	++m_iHitNum;
	m_bOnceCreate = false;
	
	m_CurAnimqeue.push({ 17, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
	
	Create_Heacy_Hit_Effect();
	
	
	
	
}

void CSkeleton_Naked::Anim_Die()
{
	m_iTurnCanvasOption = 1;
	m_Monster_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_CurAnimqeue.push({ 2, 1.f });
	m_CurAnimqeue.push({ 18, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Viroty()
{
	/*죽었으면 가만히 있기*/
	m_CurAnimqeue.push({ 0, 1.f });
	Set_CombatAnim_Index(m_pModelCom);

}

CSkeleton_Naked * CSkeleton_Naked::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkeleton_Naked*		pInstance = new CSkeleton_Naked(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkeleton_Naked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkeleton_Naked::Clone(void * pArg)
{
	CSkeleton_Naked*		pInstance = new CSkeleton_Naked(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkeleton_Naked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkeleton_Naked::Free()
{
	__super::Free();

	//for (auto& pPart : m_MonsterParts)
	//	Safe_Release(pPart);
	//m_MonsterParts.clear();

	for (auto iter = m_MonsterParts.begin(); iter != m_MonsterParts.end();)
	{
		Safe_Release(*iter);
		iter = m_MonsterParts.erase(iter);
	}


	Safe_Release(m_pStatusCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}