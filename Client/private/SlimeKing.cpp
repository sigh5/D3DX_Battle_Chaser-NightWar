#include "stdafx.h"
#include "..\public\SlimeKing.h"

#include "GameInstance.h"
#include "MonsterFsm.h"
#include "CombatController.h"
#include "Weapon.h"
#include "Buff_Effect.h"
#include "Damage_Font_Manager.h"
#include "Explain_FontMgr.h"
#include "Status.h"
#include "HitBoxObject.h"
#include "Buff_Image.h"
#include "Client_Manager.h"


CSlimeKing::CSlimeKing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice,pContext)
{
}

CSlimeKing::CSlimeKing(const CSlimeKing & rhs)
	: CMonster(rhs)
	, m_pShaderCom {nullptr}
	, m_pRendererCom{ nullptr }
    , m_pModelCom{ nullptr }
	, m_pColliderCom{ nullptr }
	, m_pStatusCom{ nullptr }
	, m_pFsmCom { nullptr }
{
	m_MonsterParts.clear();
}

CGameObject * CSlimeKing::Get_Weapon_Or_SkillBody()
{
	for (auto& pParts : m_MonsterParts)
	{
		if (dynamic_cast<CHitBoxObject*>(pParts) != nullptr 
			&& m_eWeaponType == dynamic_cast<CHitBoxObject*>(pParts)->Get_Type())
		{
			static_cast<CHitBoxObject*>(pParts)->Set_WeaponOption(m_iWeaponOption);
			static_cast<CHitBoxObject*>(pParts)->Set_WeaponDamage(m_iStateDamage);
			static_cast<CHitBoxObject*>(pParts)->Set_HitNum(m_iHitCount);
			return pParts;
		}
	}

	return nullptr;
}

_bool CSlimeKing::Calculator_HitColl(CGameObject * pWeapon)
{
	_bool bResult = false;
	CHitBoxObject* pCurActorWepon = static_cast<CHitBoxObject*>(pWeapon);

	assert(pCurActorWepon != nullptr && "CSkeleton_Naked::Calculator_HitColl");

	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{
		m_iGetDamageNum = pCurActorWepon->Get_WeaponDamage();
		m_iHitWeaponOption = static_cast<CHitBoxObject::WEAPON_OPTIONAL>(pCurActorWepon->Get_WeaponOption());

		if (pCurActorWepon->Get_HitNum() == 100)	/* 가리손 궁극기 때문에  */
		{
			m_fHitPerSecond = 2.5f;
			m_bIsHeavyHit = false;
			m_iSign *= -1;
			return true;
		}

		if (m_pStatusCom->Get_CurStatusHpRatio() <= 0.f)
			m_bIsHeavyHit = true;

		if (pCurActorWepon->Get_HitNum() > 1)
		{
			m_bIs_Multi_Hit = true;
			m_bOnceCreate = false;
		}
		CCombatController::GetInstance()->UI_Shaking(true);
		m_fHitPerSecond = 1.f;
		m_iSign = 1;

		Calculator_HitDamage();
		bResult = true;
	}
	return bResult;
}

HRESULT CSlimeKing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSlimeKing::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Monster_SlimeKing");

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
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(29.f, 0.f, -13.f, 1.f));
	m_pModelCom->Set_AnimIndex(0);

	m_bHaveSkill2 = false;
	m_bHaveUltimate = true;
	m_bDefence = false;
	m_bHaveBuff = true;
	m_bIsDefenceTimer = false;

	return S_OK;
}

HRESULT CSlimeKing::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pFsmCom = CMonsterFsm::Create(this, ANIM_CHAR1);
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_bLast_Initlize = true;
	return S_OK;
}

void CSlimeKing::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	m_pFsmCom->Tick(TimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (m_pStatusCom->Get_Dead())
		m_bIsDead = true;
	else
	{
		for (auto &pParts : m_MonsterParts)
			pParts->Tick(TimeDelta);

		for (auto& pBuffImage : m_vecBuffImage)
		{
			if (pBuffImage != nullptr)
				pBuffImage->Tick(TimeDelta);
		}
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

	m_pModelCom->Play_Animation(TimeDelta,true);
}

void CSlimeKing::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	if (m_bModelRender)
	{
		if (false == m_bIsDead)
		{
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
			CClient_Manager::Sort_BuffImage(m_vecBuffImage, false);
			for (auto& pBuffImage : m_vecBuffImage)
			{
				if (pBuffImage != nullptr)
					pBuffImage->Late_Tick(TimeDelta);
			}
			CClient_Manager::Delete_BuffImage(m_vecBuffImage, m_pStatusCom, false);
		}
	}
	if (m_bIsDead && !m_bClearScene)
	{
		for (auto& pParts : m_MonsterParts)
			Safe_Release(pParts);
		m_MonsterParts.clear();

		for (auto& pBuffImage : m_vecBuffImage)
			Safe_Release(pBuffImage);
		m_vecBuffImage.clear();
		m_bClearScene = true;
	}

	if (m_bModelRender	&& nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
	}
}

HRESULT CSlimeKing::Render()
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

void CSlimeKing::Combat_Tick(_double TimeDelta)
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

void CSlimeKing::Combat_DeadTick(_double TimeDelta)
{
	CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
}

void CSlimeKing::Create_Hit_Effect()
{
}

void CSlimeKing::Create_Heacy_Hit_Effect()
{
}

void CSlimeKing::Create_Move_Target_Effect()
{
}

void CSlimeKing::Create_BuffEffect()
{
}

void CSlimeKing::Create_Test_Effect()
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

_int CSlimeKing::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 9)
		m_iMovingDir = ANIM_DIR_STRAIGHT;
	else if (m_pModelCom->Get_AnimIndex() == 4)
		m_iMovingDir = ANIM_DIR_BACK;
	else
		m_iMovingDir = ANIM_EMD;

	return m_iMovingDir;
}

void CSlimeKing::CombatAnim_Move(_double TImeDelta)
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
			CCombatController::GetInstance()->Camera_Zoom_In();
		}
	}
	else
		return;
}

void CSlimeKing::MovingAnimControl(_double TimeDelta)
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

void CSlimeKing::Anim_Frame_Create_Control()
{
}

void CSlimeKing::Fsm_Exit()
{
	m_Monster_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_pHitTarget = nullptr;
	CCombatController::GetInstance()->Set_MonsterSetTarget(false);
	CCombatController::GetInstance()->Camera_Zoom_Out();
}

void CSlimeKing::UltiHeavyHitExit()
{
	if (false == m_bIsDead)
		CCombatController::GetInstance()->Set_MonsterSetTarget(false);
}

_bool CSlimeKing::IsCollMouse()
{
	return m_pColliderCom->Collision_Mouse(g_hWnd);
}

_bool CSlimeKing::Is_Dead()
{
	if (__super::Is_Dead())
		return true;
	return false;
}


HRESULT CSlimeKing::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_COMBAT, TEXT("Prototype_Component_Model_Slime_King"), TEXT("Com_Model"),
	(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.3f, 1.5f, 1.3f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.2f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	ZeroMemory(&StatusDesc, sizeof(CStatus::StatusDesc));
	StatusDesc.iHp = 200;
	StatusDesc.iMp = 105;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusCombat"),
		(CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSlimeKing::SetUp_ShaderResources()
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

HRESULT CSlimeKing::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();
	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Slime_Head");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.f, -0.0f, -0.0f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(1.f, 1.f, 1.f, 0.f));
	WeaponDesc.eType = WEAPON_HEAD;
	WeaponDesc.iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SPIDER_ATTACK;
	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;
	m_MonsterParts.push_back(pPartObject);
	pPartObject->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f));
	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CSlimeKing::Calculator_HitDamage()
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
		CExplain_FontMgr::GetInstance()->
			Set_Debuff_Target2_Font(vPos2, _float3(1.f, 1.f, 1.f), m_DebuffName.c_str());
	}
	else if (m_iGetDamageNum >= 50)
	{
		m_bIsHeavyHit = true;
		CExplain_FontMgr::GetInstance()->
			Set_Explain_Target2_Font1(vPos, _float3(1.f, 1.f, 1.f), TEXT("critical"));
	}

	CDamage_Font_Manager::GetInstance()->
		Set_Damage_Target2_Font(vPos, _float3(2.f, 2.f, 2.f), m_iGetDamageNum, 0.5f, 1.0f);
	m_pStatusCom->Take_Damage(m_iGetDamageNum);
}

void CSlimeKing::Anim_Idle()
{
	m_CurAnimqeue.push({ 0,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Intro()
{
	m_CurAnimqeue.push({ 12,m_IntroTimer });	//2개 넣어야함.. 처음에
	m_CurAnimqeue.push({ 0, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::AnimNormalAttack()
{
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_HEAD;
	m_iStateDamage = rand() % 10 + 25 + m_iStage_Buff_DamgaeUP;
	m_pMeHit_Player = nullptr;
	m_iStage_Buff_DamgaeUP = 0;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SLIME_KING_HEAD;
	m_bRun = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);

	m_LimitDistance = 5.f;
	m_SpeedRatio = 6.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 1.f;

	m_CurAnimqeue.push({ 8,1.f });
	m_CurAnimqeue.push({ 9,1.f });
	m_CurAnimqeue.push({ 10,1.f });
	m_CurAnimqeue.push({ 4,1.f });
	m_CurAnimqeue.push({ 5,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Skill1_Attack()
{
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_HEAD;
	m_iStateDamage = rand() % 10 + 25 + m_iStage_Buff_DamgaeUP;
	m_pStatusCom->Use_SkillMp(40);
	m_pMeHit_Player = nullptr;
	m_iStage_Buff_DamgaeUP = 0;
	m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_SLIME_KING_BREATH;
	m_bRun = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);

	m_LimitDistance = 5.f;
	m_SpeedRatio = 6.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 1.f;

	
	m_CurAnimqeue.push({ 8,1.f });
	m_CurAnimqeue.push({ 9,1.f });
	m_CurAnimqeue.push({ 16,1.f }); // 브레쓰
	m_CurAnimqeue.push({ 4,1.f });
	m_CurAnimqeue.push({ 5,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Uitimate()
{
	m_bUltimateBuffRenderStop = true;
	m_bUltimateCam = false;
	m_pStatusCom->Set_DebuffOption(CStatus::BUFF_DAMAGE, false);
	m_pStatusCom->Use_SkillMp(50);

	m_SpeedRatio = 6.f;
	m_LimitDistance = 15.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 1.f;
	m_pMeHit_Player = nullptr;

	m_CurAnimqeue.push({ 8,1.f });
	m_CurAnimqeue.push({ 9,1.f });
	m_CurAnimqeue.push({ 14,1.f });  // 몸커지기 
	m_CurAnimqeue.push({ 15,1.f }); 
	m_CurAnimqeue.push({ 4,1.f });
	m_CurAnimqeue.push({ 5,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Buff()
{
	m_iStage_Buff_DamgaeUP = 10;
	m_bOnceCreate = false;
	m_useBuff = true;
	m_pStatusCom->Use_SkillMp(40);

	m_CurAnimqeue.push({ 3, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Light_Hit()
{
	++m_iHitNum;
	if (m_bIsHeavyHit)
	{
		m_CurAnimqeue.push({ 6,1.f });
		m_bIsHeavyHit = false;
	}
	else if (true == m_bIs_Multi_Hit)
	{
		m_CurAnimqeue.push({ 1,1.f });
		m_CurAnimqeue.push({ 7,1.f });

		m_bIs_Multi_Hit = false;
		m_bOnceCreate = false;
	}
	else
	{
		m_CurAnimqeue.push({ 1,m_fHitPerSecond });
	}

	Set_CombatAnim_Index(m_pModelCom);
	m_pMeHit_Player = nullptr;
}

void CSlimeKing::Anim_Heavy_Hit()
{
	++m_iHitNum;
	m_bIsHeavyHit = false;

	m_CurAnimqeue.push({ 6,1.f });

	Set_CombatAnim_Index(m_pModelCom);	
	Create_Heacy_Hit_Effect();
}

void CSlimeKing::Anim_Die()
{
	m_iTurnCanvasOption = 1;
	m_Monster_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_CurAnimqeue.push({ 2,1.f });
	m_CurAnimqeue.push({ 17,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Viroty()
{
	m_CurAnimqeue.push({ 12,1.f });
	Set_CombatAnim_Index(m_pModelCom);

}

CSlimeKing * CSlimeKing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSlimeKing*		pInstance = new CSlimeKing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSlimeKing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSlimeKing::Clone(void * pArg)
{
	CSlimeKing*		pInstance = new CSlimeKing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSlimeKing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSlimeKing::Free()
{
	__super::Free();

	for (auto& pPart : m_MonsterParts)
		Safe_Release(pPart);
	m_MonsterParts.clear();	
	
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}