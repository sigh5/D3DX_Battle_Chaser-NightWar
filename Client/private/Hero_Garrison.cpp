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

	if (nullptr == pCurActorWepon)		//나중에 아래것으로
		return false;
	//	assert(pCurActorWepon != nullptr && "CSkeleton_Naked::Calculator_HitColl");

	if (pCurActorWepon->Get_Colider()->Collision(m_pColliderCom))
	{
		if (m_bUseDefence == true)
		{
			m_pStatusCom[COMBAT_PLAYER]->Take_Damage(_int(pCurActorWepon->Get_WeaponDamage()*0.5));
			Create_Hit_Effect();
		}
		else
			m_pStatusCom[COMBAT_PLAYER]->Take_Damage(pCurActorWepon->Get_WeaponDamage());
	
		
		if (m_pStatusCom[COMBAT_PLAYER]->Get_CurStatusHpRatio() <= 0.f)
			m_bIsHeavyHit = true;

		m_iHitWeaponOption = pCurActorWepon->Get_WeaponOption();

		if (pCurActorWepon->Get_HitNum() > 1)
		{
			m_bIs_Multi_Hit = true;
			m_bOnceCreate = false;
		}
		return true;
	}
	return false;
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

	for (auto iter = m_pEffectParts.begin(); iter != m_pEffectParts.end();)
	{
		if ((*iter) != nullptr)
			(*iter)->Late_Tick(TimeDelta);

		if (true == static_cast<CBuff_Effect*>(*iter)->Get_IsFinish())
		{
			Safe_Release(*iter);
			iter = m_pEffectParts.erase(iter);
		}
		else if (true == static_cast<CBuff_Effect*>(*iter)->Get_MainTain() && m_bUseDefence ==false)
		{
			Safe_Release(*iter);
			iter = m_pEffectParts.erase(iter);
		}
		else
			++iter;
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
	if (IsCaptin() && !CPlayer::KeyInput(TimeDelta,m_pNavigationCom))
		m_iAnimIndex = 0;
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
		CurAnimQueue_Play_Tick(TimeDelta,m_pModelCom);
	
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
	
	for (_uint i = 0; i < m_PlayerParts.size(); ++i)
	{
		m_PlayerParts[i]->Tick(TimeDelta);
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
	WEAPON_OPTIONAL WeaponOption = static_cast<WEAPON_OPTIONAL>(m_iHitWeaponOption);
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
		case Client::WEAPON_OPTIONAL_NONE:
			break;
		case Client::WEAPON_OPTIONAL_BLUE:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_8", LEVEL_COMBAT, false);
			break;
		case Client::WEAPON_OPTIONAL_PULPLE:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_10", LEVEL_COMBAT, false);
			iEffectNum = 1;
			BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
			BuffDesc.vScale = _float3(5.f, 5.f, 5.f);
			break;
		case Client::WEAPON_OPTIONAL_GREEN:
			pGameObject = pInstance->Load_Effect(L"Texture_Common_Hit_Effect_9", LEVEL_COMBAT, false);
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
	pGameObject = pInstance->Load_Effect(L"Texture_Common_Aura_8", LEVEL_COMBAT, false);

	BuffDesc.ParentTransform = m_pTransformCom;
	BuffDesc.vPosition = _float4(0.f, 1.f, 0.f, 1.f);
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = -90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = false;
	BuffDesc.iFrameCnt = 4;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);
	m_bOnceCreate = false;
	m_bCreateDefenceTimer = true;
	RELEASE_INSTANCE(CGameInstance);

	
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
	BuffDesc.vPosition = _float4(1.5f, 1.f, -1.5f, 1.f);
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


}

void CHero_Garrison::Create_BuffEffect()
{
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
	BuffDesc.vScale = _float3(10.f, 10.f, 10.f);
	BuffDesc.vAngle = -90.f;
	BuffDesc.fCoolTime = 5.f;
	BuffDesc.bIsMainTain = true;
	BuffDesc.iFrameCnt = 6;
	BuffDesc.bIsUp = false;
	static_cast<CBuff_Effect*>(pGameObject)->Set_Client_BuffDesc(BuffDesc);
	m_pEffectParts.push_back(pGameObject);

	RELEASE_INSTANCE(CGameInstance);
}


void CHero_Garrison::Anim_Frame_Create_Control()
{
	if (m_pModelCom->Control_KeyFrame_Create(16, 1) && !m_bOnceCreate)
	{
		Create_Hit_Effect();
		m_bOnceCreate = true;
	}
	else if (m_pModelCom->Control_KeyFrame_Create(4, 1) && !m_bRun)
	{
		Create_Move_Target_Effect();
		m_bRun = true;
	}
	else if (m_pModelCom->Control_KeyFrame_Create(17, 14) && !m_bRun)
	{
		Create_Move_Target_Effect();
		m_bRun = true;
	}
	else if (m_pModelCom->Control_KeyFrame_Create(3, 9) && !m_bOnceCreate)
	{
		Create_Normal_Attack_Effect();
		m_bOnceCreate = true;
	}
	else if (m_pModelCom->Control_KeyFrame_Create(10, 30) && !m_bOnceCreate)
	{
		Create_Skill1_Attack_Effect();
		m_bOnceCreate = true;
	}
	else if (m_pModelCom->Control_KeyFrame_Create(19, 20) && !m_bOnceCreate)
	{
		Create_Skill2_Attack_Effect();
		m_bOnceCreate = true;
	}//   Texture_Bleeding_Effect_1
	else if (m_bUseDefence == true &&m_pModelCom->Control_KeyFrame_Create(8, 20) && !m_bOnceCreate)
	{
		Create_Defence_Area();
		m_bOnceCreate = true;
	}
	else if (m_bUseDefence == true && !m_bOnceCreate &&m_pModelCom->Control_KeyFrame_Create(32, 43))
	{
		Create_Hit_Effect();
		m_bOnceCreate = true;
		m_bIs_Multi_Hit = false;
	}
	else
		return;


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
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(9.f, 0.f, 16.f, 1.f));
	m_pTransformCom->Set_TransfromDesc(7.f, 90.f);
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (FAILED(Ready_CombatParts()))
		return E_FAIL;

	_float4 vPos;
	XMStoreFloat4(&vPos, XMVectorSet(9.f, 0.f, 16.f, 1.f));
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
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(0.5f, 2.f, 0.5f,0.f));
	WeaponDesc.eType = WEAPON_SWORD;
	WeaponDesc.iWeaponOption = WEAPON_OPTIONAL_BLUE;

	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;

	m_PlayerParts.push_back(pPartObject);

	/* For.Prototype_Component_Status */
	CStatus::StatusDesc			StatusDesc;
	StatusDesc.iHp = 150;
	StatusDesc.iMp = 300;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Status"), TEXT("Com_StatusCombat"),
		(CComponent**)&m_pStatusCom[COMBAT_PLAYER], &StatusDesc)))
		return E_FAIL;


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
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(m_vOriginPos, TImeDelta, m_ReturnDistance, m_SpeedRatio);	
	
	if (m_pHitTarget == nullptr || bResult == ANIM_EMD)
		return;

	if (bResult == ANIM_DIR_STRAIGHT)
	{
		_float4 Target;
		XMStoreFloat4(&Target, m_pHitTarget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(XMLoadFloat4(&Target), TImeDelta, m_LimitDistance, m_SpeedRatio);
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
	m_iStateDamage = 20;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SWORD;
	m_iWeaponOption = WEAPON_OPTIONAL_NONE;
	m_bOnceCreate = false;
	m_bRun = false;

	m_SpeedRatio = 7.f;
	m_LimitDistance = 10.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 0.9f;
	m_CurAnimqeue.push({ 4,  m_setTickForSecond });
	m_CurAnimqeue.push({ 3,  1.f });		// 3에서		// 3 22에 이펙트 생성
	m_CurAnimqeue.push({ 11, m_setTickForSecond });
	m_CurAnimqeue.push({ 12, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Skill1_Attack()
{
	m_bOnceCreate = false;
	m_bRun = false;
	m_iHitCount = 1;
	m_eWeaponType = WEAPON_SWORD;
	m_iStateDamage = 40;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(30);
	m_SpeedRatio = 7.f;
	m_LimitDistance = 10.f;
	m_ReturnDistance = 0.1f;
	m_setTickForSecond = 0.9f;
	m_iWeaponOption = WEAPON_OPTIONAL_NONE;
	
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
	m_bOnceCreate = false;
	m_bRun = false;
	m_eWeaponType = WEAPON_SWORD;
	m_iStateDamage = 50;
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_iHitCount = 1;
	m_LimitDistance = 12.f;
	m_SpeedRatio = 6.f;
	m_ReturnDistance = 0.5f;
	m_setTickForSecond = 0.9f;
	m_iWeaponOption = WEAPON_OPTIONAL_NONE;

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
	m_bOnceCreate = false;
	m_bRun = false;
	m_eWeaponType = WEAPON_SWORD;
	m_iStateDamage = 20;			//20*6
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);

	m_LimitDistance = 12.f;
	m_SpeedRatio = 6.f;
	m_ReturnDistance = 0.5f;
	m_setTickForSecond = 0.9f;
	m_iWeaponOption = WEAPON_OPTIONAL_NONE;

	m_CurAnimqeue.push({ 30,  1.f });	// Key프레임 (뛰는 것)하나 찾기 83~94 는 움직여야함  //Texture_Garrison_Ultimate_Effect
	m_CurAnimqeue.push({ 11,  m_setTickForSecond });
	m_CurAnimqeue.push({ 12,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Buff()
{
	m_pStatusCom[COMBAT_PLAYER]->Use_SkillMp(40);
	m_CurAnimqeue.push({ 8,  1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
	Create_BuffEffect();
}

void CHero_Garrison::Anim_Use_Item()
{
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
		_uint iHitRand = 13;			// 13 ,14 ,15
		m_CurAnimqeue.push({ iHitRand ,  1.f });
	}
	else if (true == m_bIs_Multi_Hit)
	{
		m_CurAnimqeue.push({ 6,  1.f });	
		m_CurAnimqeue.push({ 16,  1.f });	
		m_CurAnimqeue.push({ 1,  1.f });
		m_bOnceCreate = false;
		m_bIs_Multi_Hit = false;
	}
	else
	{
		m_CurAnimqeue.push({ 26, 1.f });
		m_CurAnimqeue.push({ 1,  1.f });
	}
	
	Set_CombatAnim_Index(m_pModelCom);
	Create_Hit_Effect();
}

void CHero_Garrison::Anim_Heavy_Hit()
{
	_uint iHitRand = rand() % 4 + 13;
	m_CurAnimqeue.push({ iHitRand ,  1.f });
	
	Create_Hit_Effect();
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
	m_iTurnCanvasOption = 1;
	m_Hero_CombatTurnDelegeter.broadcast(m_Represnt, m_iTurnCanvasOption);
	m_CurAnimqeue.push({ 7,  1.f });
	m_CurAnimqeue.push({ 33,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Anim_Viroty()
{
	//Is_Dead();
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Garrison::Create_Test_Effect()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = nullptr;
	_uint			iEffectNum = 1;
	CBuff_Effect::BuffEffcet_Client BuffDesc;
	ZeroMemory(&BuffDesc, sizeof(BuffDesc));
	pGameObject =	pInstance->Load_Effect(m_TextureTag.c_str(), LEVEL_COMBAT, true);

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



_bool CHero_Garrison::Is_Dead()
{
	
	if (__super::Is_Dead())
	{
		//Anim_Die();
		return true;
	}

	return false;
}
