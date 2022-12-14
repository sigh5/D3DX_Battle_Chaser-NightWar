#include "stdafx.h"
#include "..\public\Hero_Knolan.h"

#include "GameInstance.h"
#include "Client_Manager.h"

#include "PlayerController.h"
#include "AnimFsm.h"

CHero_Knolan::CHero_Knolan(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice,pContext)
{
}

CHero_Knolan::CHero_Knolan(const CHero_Knolan & rhs)
	: CPlayer(rhs)
{
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
	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
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
	

	m_bLast_Initlize = true;
	return S_OK;
}

void CHero_Knolan::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


	if (m_bIsCombatScene == false)
		Dungeon_Tick(TimeDelta);
	else
	{
		Combat_Initialize();
		
		if(CPlayerController::GetInstance()->Get_CurActor() == this)
			m_pFsmCom->Tick(TimeDelta);
		//Combat_Tick(TimeDelta);
	}
		
	m_pModelCom->Play_Animation(TimeDelta, m_bIsCombatScene);
}

void CHero_Knolan::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	
	CurAnimQueue_Play_LateTick(m_pModelCom);

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
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_KnolanDungeon"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("Change_Level_Data : LEVEL_COMBAT ");

		m_bIsCombatScene = false;
	}
	else if (LEVEL_COMBAT == iLevleIdx)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_KnolanCombat"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			assert("Change_Level_Data : LEVEL_COMBAT ");
		m_bIsCombatScene = true;
		m_pTransformCom->Set_Scaled(_float3(2.0f, 2.0f, 2.0f));
	}
	// maybe 다른씬?
	RELEASE_INSTANCE(CGameInstance);
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
	if (IsCaptin() && !CPlayer::KeyInput(TimeDelta))
		m_iAnimIndex = 0;
	AnimMove();
	CClient_Manager::CaptinPlayer_ColiderUpdate(this, m_pColliderCom, m_pTransformCom);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CHero_Knolan::Combat_Initialize()
{
	if (m_bCombat_LastInit)
		return S_OK;

	m_pFsmCom = CAnimFsm::Create(this, ANIM_CHAR1);
	m_bCombat_LastInit = true;
	
	return S_OK;
}

void CHero_Knolan::Combat_Tick(_double TimeDelta)
{
	CPlayer::CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
}

void CHero_Knolan::ObserverTest(_double TimeDelta)
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	if (pInstance->Key_Down(DIK_SPACE))
	{
		_uint iMoveSpeed = 100;
		m_Hero_GullyHPDelegater.broadcast(TimeDelta, iMoveSpeed);
	}
	if (pInstance->Key_Down(DIK_O))
	{
		_uint iShakingTime = 3;
		m_Hero_GullyTestShakingDelegater.broadcast(iShakingTime);
	}


	RELEASE_INSTANCE(CGameInstance);
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

		/*if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Spider_Mana_Baby"), TEXT("Com_Model"),
			(CComponent**)&m_pModelCom)))
			return E_FAIL;*/

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GullyCombat"), TEXT("Com_Model2"),
	//	(CComponent**)&m_pModelCom[COMBAT_PLAYER])))
	//	return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_OBB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(0.8f, 1.7f, 0.7f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.2f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"), TEXT("Com_OBB"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
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

void CHero_Knolan::Anim_Idle()
{
	m_CurAnimqeue.push({ 2,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Intro()
{
	m_CurAnimqeue.push({ 12, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::AnimNormalAttack()
{
	//_uint iHitRand = rand() % 4 + 13;
	m_CurAnimqeue.push({ 7, 1.f }); //7 21(제자리),24(골프샷),25(뒤로점프샷)
	m_CurAnimqeue.push({ 1, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Skill1_Attack()
{
	m_CurAnimqeue.push({ 15, 1.f });
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Skill2_Attack()
{
	m_CurAnimqeue.push({ 19, 1.f }); // fireStorm
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Uitimate()
{
	m_CurAnimqeue.push({ 28, 1.f }); // 28 전체공격력 업
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Buff()
{
	m_CurAnimqeue.push({ 26, 1.f });	// 자 버프 
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_WideAreaBuff()
{
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
	m_CurAnimqeue.push({ 4,  1.f });	// 3 or 4
	m_CurAnimqeue.push({ 1,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}
		
void CHero_Knolan::Anim_Heavy_Hit()
{
	m_CurAnimqeue.push({ 8,  1.f });	// 8,9,10
	m_CurAnimqeue.push({ 1,  1.f });
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
	m_CurAnimqeue.push({ 5,  1.f });
	m_CurAnimqeue.push({ 23, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CHero_Knolan::Anim_Viroty()
{
	Is_PlayerDead();
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

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}

_bool CHero_Knolan::Is_PlayerDead()
{
	/* 플레이가 죽었을때랑 살았을때 구분해서 쓰기*/
	/*살았을때*/
	m_CurAnimqeue.push({ 13 ,	1.f });
	m_CurAnimqeue.push({ 36,	1.f });

	/* 죽었을때*/
	//m_CurAnimqeue.push({ 33 ,0.1f });
	//m_CurAnimqeue.push({ 13,0.1f });
	//m_CurAnimqeue.push({ 36,0.1f });
	return _bool();
}


