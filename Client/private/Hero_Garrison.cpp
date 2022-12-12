#include "stdafx.h"
#include "..\public\Hero_Garrison.h"
#include "GameInstance.h"
#include "Client_Manager.h"
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
	return S_OK;
}

void CHero_Garrison::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bIsCombatScene == false)
	{
		if (IsCaptin() && !CPlayer::KeyInput(TimeDelta))
			m_iAnimIndex = 0;
		AnimMove();
	}
	if (m_bTestImguiButton == 1)
	{
		if (!m_CurAnimqeue.empty() && m_pModelCom->Get_Finished(m_pModelCom->Get_AnimIndex()))
		{
			m_bIsTest = false;
			m_iOldAnim = m_pModelCom->Get_AnimIndex();
			_uint i = m_CurAnimqeue.front().first;
			m_pModelCom->Set_AnimIndex(i);
			m_CurAnimqeue.pop();
			m_bFinishOption = ANIM_CONTROL_NEXT;

			if (m_CurAnimqeue.empty())
			{
				m_bIsTest = true;
			}
		
			if (18 == i)
				m_pModelCom->Set_Lerp(i, m_CurAnimqeue.front().first);
			
		}
	}
	m_pModelCom->Play_Animation(TimeDelta,m_bIsCombatScene);
}


void CHero_Garrison::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bFinishOption == ANIM_CONTROL_NEXT)
	{
		m_pModelCom->Set_Finished(m_iOldAnim, false);
		m_bFinishOption = ANIM_CONTROL_SEQUNCE;

	}
	if (m_bTestImguiButton == 1)
	{
		if (m_bIsTest && m_CurAnimqeue.empty() && m_pModelCom->Get_Finished(m_pModelCom->Get_AnimIndex()))
		{
			m_pModelCom->Set_PlayTime(m_pModelCom->Get_AnimIndex());
		}
	}

	ImGui::RadioButton("QeueeNoneUse", &m_bTestImguiButton, 0);
	ImGui::RadioButton("QeueeUes", &m_bTestImguiButton, 1);
	
	queue<pair<_uint, _double>> Temp;
	CClient_Manager::Make_Anim_Queue(Temp, ANIM_CHAR1);

	if (m_bTestImguiButton == 1)
	{
		if (CGameInstance::GetInstance()->Key_Down(DIK_Y))
		{
			m_CurAnimqeue = Temp;

			m_pModelCom->Set_PlayTime(m_pModelCom->Get_AnimIndex());
			m_pModelCom->InitChannel();
			m_pModelCom->Set_AnimIndex(m_CurAnimqeue.front().first);
			m_CurAnimqeue.pop();
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
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, 0,"g_BoneMatrices","DN_FR_FishingRod");
	}

	return S_OK;
}




_bool CHero_Garrison::Piciking_GameObject()
{
	return false;
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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}


