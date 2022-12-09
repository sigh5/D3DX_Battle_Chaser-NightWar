#include "stdafx.h"
#include "..\public\Hero_Gully.h"

#include "GameInstance.h"

CHero_Gully::CHero_Gully(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CPlayer(pDevice,pContext)
{
}

CHero_Gully::CHero_Gully(const CHero_Gully & rhs)
	: CPlayer(rhs)
{
}

HRESULT CHero_Gully::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHero_Gully::Initialize(void * pArg)
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

HRESULT CHero_Gully::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bControlKeyInput = true;

	m_pModelCom->Set_NoRenderMeshName("Gully_Gloves_Stone");

	m_pModelCom->m_iNoRenderIndex = 12;
	m_bLast_Initlize = true;

	return S_OK;
}

void CHero_Gully::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	if (IsCaptin() && !CPlayer::KeyInput(TimeDelta))
		m_iAnimIndex = 0;

	AnimMove();
	
	ObserverTest(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);

}

void CHero_Gully::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CHero_Gully::Render()
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

		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices", "FishingRod");
	}

	return S_OK;
	
}

_vector CHero_Gully::Get_CameraBoneVector()
{
	return m_pModelCom->GetModelCameraBone();
}



_bool CHero_Gully::Piciking_GameObject()
{
	return _bool();
}

_uint CHero_Gully::Get_AnimationIndex()
{
	return m_pModelCom->Get_AnimIndex();
}

void CHero_Gully::AnimMove()
{
	m_pModelCom->Set_AnimIndex(m_iAnimIndex);
}

void CHero_Gully::HighLightChar()
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

void CHero_Gully::NormalLightCharUI()
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


void CHero_Gully::ObserverTest(_double TimeDelta)
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

HRESULT CHero_Gully::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GullyDungeon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GullyCombat"), TEXT("Com_Model2"),
	//	(CComponent**)&m_pModelCom[COMBAT_PLAYER])))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CHero_Gully::SetUp_ShaderResources()
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

CHero_Gully * CHero_Gully::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHero_Gully*		pInstance = new CHero_Gully(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHero_Gully");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CHero_Gully::Clone(void * pArg)
{
	CHero_Gully*		pInstance = new CHero_Gully(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHero_Gully");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHero_Gully::Free()
{
	__super::Free();

	//for (auto& pModel : m_pModelCom)
		Safe_Release(m_pModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}