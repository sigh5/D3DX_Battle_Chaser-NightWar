#include "stdafx.h"
#include "..\public\Hero_Garrison.h"
#include "GameInstance.h"
#include "Client_Manager.h"

#include "PlayerController.h"

CHero_Garrison::CHero_Garrison(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice,pContext)
{
}

CHero_Garrison::CHero_Garrison(const CHero_Garrison& rhs)
	: CPlayer(rhs)
{
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
		Combat_Tick(TimeDelta);
	}
	m_pModelCom->Play_Animation(TimeDelta,m_bIsCombatScene);
}


void CHero_Garrison::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);
	
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
	CClient_Manager::Collider_Render(this, m_pColliderCom);
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
	if (IsCaptin() && !CPlayer::KeyInput(TimeDelta))
		m_iAnimIndex = 0;
	AnimMove();
	CClient_Manager::CaptinPlayer_ColiderUpdate(this, m_pColliderCom, m_pTransformCom);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CHero_Garrison::Combat_Tick(_double TimeDelta)
{
	CPlayer::CurAnimQueue_Play_Tick(TimeDelta,m_pModelCom);
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

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}

void CHero_Garrison::Set_Current_AnimQueue(CurrentState eType, _bool IsEvent)
{
	if (!IsEvent)
		return;
	
	_uint iHitRand = rand() % 4 + 13; 
	m_eType = eType;
	
	switch (eType)
	{
	case STATE_INTRO:
		m_CurAnimqeue.push({ 21, 1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_CurAnimqeue.push({ 2,  1.f });
		break;
	case STATE_NORMAL_ATTACK:
		m_CurAnimqeue.push({ 4,  1.f });
		m_CurAnimqeue.push({ 3,  1.f });
		m_CurAnimqeue.push({ 11, 1.f });
		m_CurAnimqeue.push({ 12, 1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_CurAnimqeue.push({ 2,  1.f });
		break;
	case STATE_SKILL_ATTACK1:
		m_CurAnimqeue.push({ 4,  1.f });
		m_CurAnimqeue.push({ 10, 1.f });
		m_CurAnimqeue.push({ 11, 1.f });
		m_CurAnimqeue.push({ 12, 1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_CurAnimqeue.push({ 2,  1.f });
		break;

	case STATE_SKILL_ATTACK2:
		m_CurAnimqeue.push({ 17, 1.f });
		m_CurAnimqeue.push({ 18, 1.f });
		m_CurAnimqeue.push({ 19, 1.f });
		m_CurAnimqeue.push({ 11, 1.f });
		m_CurAnimqeue.push({ 12, 1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_CurAnimqeue.push({ 2,  1.f });
		break;

	case STATE_UlTIMATE:
		m_CurAnimqeue.push({ 30,  1.f });
		m_CurAnimqeue.push({ 11,  1.f });
		m_CurAnimqeue.push({ 12,  1.f });
		m_CurAnimqeue.push({ 1,   1.f });
		m_CurAnimqeue.push({ 2,   1.f });
		break;

	case STATE_BUFF:
		m_CurAnimqeue.push({ 8,  1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_CurAnimqeue.push({ 2,  1.f });
		break;

	case STATE_WIDEAREA_BUFF:
		m_CurAnimqeue.push({ 2,  1.f });		//없음
		break;
		

	case STATE_USE_ITEM:
		m_CurAnimqeue.push({ 23,  1.f });
		m_CurAnimqeue.push({ 1,   1.f });
		m_CurAnimqeue.push({ 2,   1.f });
		break;
	case STATE_DEFENCE:
		m_CurAnimqeue.push({ 32,   1.f });
		m_CurAnimqeue.push({ 1,    1.f });
		m_CurAnimqeue.push({ 2,    1.f });
		break;
	case STATE_LIGHT_HIT:
		m_CurAnimqeue.push({ 26, 1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_CurAnimqeue.push({ 2,  1.f });
		break;
	case STATE_HEAVY_HIT:
		m_CurAnimqeue.push({ iHitRand ,  1.f });
		m_CurAnimqeue.push({ 1,  1.f });
		m_CurAnimqeue.push({ 2,  1.f });
		break;
	case STATE_FLEE:
		m_CurAnimqeue.push({ 48,  1.f });
		m_CurAnimqeue.push({ 47,  1.f });
		break;
	case STATE_DIE:
		m_CurAnimqeue.push({ 7,  1.f });
		m_CurAnimqeue.push({ 33,  1.f });
		break;
	case STATE_VITORY:
		Is_PlayerDead();
		break;
	case STATE_END:
		break;
	default:
		break;
	}
	Set_CombatAnim_Index(m_pModelCom);

}

_bool CHero_Garrison::Is_PlayerDead()
{
	if (m_eType == STATE_VITORY)
	{
		//ToDo .. 다른 애님 추가
		/* if 플레이어 죽음 아니면 살음*/
		m_CurAnimqeue.push({ 22 ,0.1f });
		m_CurAnimqeue.push({ 43,0.1f });
	}


	return true;
}
