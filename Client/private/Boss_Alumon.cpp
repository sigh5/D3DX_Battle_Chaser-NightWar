#include "stdafx.h"
#include "..\public\Boss_Alumon.h"

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
	, m_pFsmCom{ nullptr }
{
	m_MonsterParts.clear();
}

CGameObject * CBoss_Alumon::Get_Weapon_Or_SkillBody()
{
	return nullptr;
}

_bool CBoss_Alumon::Calculator_HitColl(CGameObject * pWeapon)
{
	return _bool();
}

_bool CBoss_Alumon::IsCollMouse()
{
	return _bool();
}

_bool CBoss_Alumon::Is_Dead()
{
	return _bool();
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
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(33.f, 0.f, -11.3f, 1.f));
	m_pModelCom->Set_AnimIndex(0);


	return S_OK;
}

HRESULT CBoss_Alumon::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

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
	
	m_pModelCom->Play_Animation(TimeDelta, false);
}

void CBoss_Alumon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		//m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
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

void CBoss_Alumon::Initialize_CombatSound()
{
}

void CBoss_Alumon::Fsm_Exit()
{
}

void CBoss_Alumon::UltiHeavyHitExit()
{
}

void CBoss_Alumon::Combat_Tick(_double TimeDelta)
{
}

_int CBoss_Alumon::Is_MovingAnim()
{
	return _int();
}

void CBoss_Alumon::CombatAnim_Move(_double TImeDelta)
{
}

void CBoss_Alumon::MovingAnimControl(_double TimeDelta)
{
}

void CBoss_Alumon::Anim_Frame_Create_Control()
{
}

void CBoss_Alumon::Combat_DeadTick(_double TimeDelta)
{
}

void CBoss_Alumon::Create_Hit_Effect()
{
}

void CBoss_Alumon::Create_Heavy_Hit_Effect()
{
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
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CBoss_Alumon::Ready_Parts()
{
	return S_OK;
}

void CBoss_Alumon::Calculator_HitDamage()
{
}

void CBoss_Alumon::Anim_Idle()
{
}

void CBoss_Alumon::Anim_Intro()
{
}

void CBoss_Alumon::Anim_NormalAttack()
{
}

void CBoss_Alumon::Anim_Skill1_Attack()
{
}

void CBoss_Alumon::Anim_Defence()
{
}

void CBoss_Alumon::Anim_Buff()
{
}

void CBoss_Alumon::Anim_Light_Hit()
{
}

void CBoss_Alumon::Anim_Heavy_Hit()
{
}

void CBoss_Alumon::Anim_Die()
{
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