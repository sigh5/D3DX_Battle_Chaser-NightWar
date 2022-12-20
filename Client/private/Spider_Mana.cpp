#include "stdafx.h"
#include "..\public\Spider_Mana.h"

#include "GameInstance.h"
#include "Client_Manager.h"
#include "MonsterFsm.h"
#include "CombatController.h"

CSpider_Mana::CSpider_Mana(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CSpider_Mana::CSpider_Mana(const CSpider_Mana & rhs)
	: CMonster(rhs)
{
}

HRESULT CSpider_Mana::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSpider_Mana::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Spider_Mana");

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
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(38.f, 0.f, -9.f, 1.f));

	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

HRESULT CSpider_Mana::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pFsmCom = CMonsterFsm::Create(this, ANIM_CHAR3);

	m_bLast_Initlize = true;
	return S_OK;
}

void CSpider_Mana::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


	m_pFsmCom->Tick(TimeDelta);
	
	m_pModelCom->Play_Animation(TimeDelta, true);
}

void CSpider_Mana::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	
	CurAnimQueue_Play_LateTick(m_pModelCom);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSpider_Mana::Render()
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

void CSpider_Mana::Combat_Tick(_double TimeDelta)
{
	CMonster::CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	Is_MovingAnim();
	CombatAnim_Move(TimeDelta);
}

_int CSpider_Mana::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 9 )
	{
		m_iMovingDir = ANIM_DIR_STRAIGHT;
		m_pModelCom->Set_Duration(9, 2);
	}
	else if  (m_pModelCom->Get_AnimIndex() == 13)
	{
		m_iMovingDir = ANIM_DIR_STRAIGHT;
		m_pModelCom->Set_Duration(13, 2);
	}
	else if (m_pModelCom->Get_AnimIndex() == 4)
	{
		m_iMovingDir = ANIM_DIR_BACK;
		m_pModelCom->Set_Duration(4, 2);
	}
	else if (m_pModelCom->Get_AnimIndex() == 12)
	{
		m_iMovingDir = ANIM_DIR_BACK;
		m_pModelCom->Set_Duration(12, 2);
	}
	else
		m_iMovingDir = ANIM_EMD;

	return m_iMovingDir;
}

void CSpider_Mana::CombatAnim_Move(_double TImeDelta)
{
	if (m_iMovingDir == ANIM_DIR_STRAIGHT)
		m_pTransformCom->Go_Straight(TImeDelta);		//chase로 바꾸기

	else if (m_iMovingDir == ANIM_DIR_BACK)
		m_pTransformCom->Go_Backward(TImeDelta);
}


HRESULT CSpider_Mana::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_COMBAT, TEXT("Prototype_Component_Model_Spider_Mana_Baby"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpider_Mana::SetUp_ShaderResources()
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

void CSpider_Mana::Anim_Idle()
{
	m_CurAnimqeue.push({ 0, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_Intro()
{
	m_CurAnimqeue.push({ 11, m_IntroTimer });
	m_CurAnimqeue.push({ 0, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_NormalAttack()
{
	m_CurAnimqeue.push({ 8, 1.f });
	m_CurAnimqeue.push({ 9, 1.f });
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 4, 1.f });
	m_CurAnimqeue.push({ 5, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_Skill1_Attack()
{
	m_CurAnimqeue.push({ 13, 1.f });	//깨물기
	m_CurAnimqeue.push({ 14, 1.f });
	m_CurAnimqeue.push({ 12, 1.f });
	m_CurAnimqeue.push({ 5, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_Skill2_Attack()
{
	m_CurAnimqeue.push({ 15, 0.8f });	// 거미줄 발사
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_Buff()
{
	m_CurAnimqeue.push({ 3, 0.7f });	
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_Light_Hit()
{
	m_CurAnimqeue.push({ 1, 1.f });	
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_Heavy_Hit()
{
	m_CurAnimqeue.push({ 6,1.f });	
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_Die()
{
	m_CurAnimqeue.push({ 2,1.f });	
	m_CurAnimqeue.push({ 19,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSpider_Mana::Anim_Viroty()
{
	m_CurAnimqeue.push({ 17,1.f });
	m_CurAnimqeue.push({ 16,1.f });
	m_CurAnimqeue.push({ 18,1.f });
	m_CurAnimqeue.push({ 0,1.f });
	Set_CombatAnim_Index(m_pModelCom);

}

CSpider_Mana * CSpider_Mana::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpider_Mana*		pInstance = new CSpider_Mana(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpider_Mana");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSpider_Mana::Clone(void * pArg)
{
	CSpider_Mana*		pInstance = new CSpider_Mana(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpider_Mana");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSpider_Mana::Free()
{
	__super::Free();

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
