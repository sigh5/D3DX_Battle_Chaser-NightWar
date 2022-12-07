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
	return S_OK;
}

void CHero_Gully::Tick(_double TimeDelta)
{
	//ImGui::InputInt("AnimIndex", &AnimIndex);
	//ImGui::InputInt("AnimType", &m_iPlayerType);	// 0 or 1

	//if (AnimIndex >= m_pModelCom->m_iNumAnimations)
	//	AnimIndex = 0;

	__super::Tick(TimeDelta);

	KeyInput(TimeDelta);

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
		/* �� ���� �׸������� ���̴��� ���׸��� �ؽ��ĸ� �����ϳ�. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices");
	}

	return S_OK;
	
}

void CHero_Gully::KeyInput(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4 vTargetPos;
	XMStoreFloat4(&vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));


	if (pGameInstance->Key_Pressing(DIK_DOWN))
	{
		vTargetPos.z -= 1.f;
		if (pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			vTargetPos.z += 0.5f;
			vTargetPos.x += 0.5f;
		}
		else if (pGameInstance->Key_Pressing(DIK_LEFT))
		{
			vTargetPos.z += 0.5f;
			vTargetPos.x -= 0.5f;
		}
		m_bKeyInput = true;
		
	}
	else if (pGameInstance->Key_Pressing(DIK_UP))
	{
		vTargetPos.z += 1.f;
		if (pGameInstance->Key_Pressing(DIK_RIGHT))
		{
			vTargetPos.z -= 0.5f;
			vTargetPos.x += 0.5f;
		}
		else if (pGameInstance->Key_Pressing(DIK_LEFT))
		{
			vTargetPos.z -= 0.5f;
			vTargetPos.x -= 1.f;
		}
		m_bKeyInput = true;
		
	}
	else if (pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_bKeyInput = true;
		vTargetPos.x += 1.f;
	}
	else if (pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_bKeyInput = true;
		vTargetPos.x -= 1.f;
	}
	else
		m_bKeyInput = false;

	if (m_bKeyInput)
	{
		m_fWalkTime += TimeDelta* 1.0;
		m_pTransformCom->LookAt(XMLoadFloat4(&vTargetPos));
		
		if (m_fWalkTime >= 1.f)
		{
			m_pModelCom->Set_AnimIndex(2);
			m_fMoveSpeedRatio = 1.f;
		}
		else
		{
			m_pModelCom->Set_AnimIndex(1);
			m_fMoveSpeedRatio = 0.5f;
		}
		m_pTransformCom->Go_Straight(TimeDelta, m_fMoveSpeedRatio);
	}
	else
	{
		m_pModelCom->Set_AnimIndex(0);
		m_fWalkTime = 0.0;
	}



	RELEASE_INSTANCE(CGameInstance);
}

_bool CHero_Gully::Piciking_GameObject()
{
	return _bool();
}

void CHero_Gully::ObserverTest()
{
	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);
	/*if (pInstance->Key_Down(DIK_SPACE))
	{
	_uint iMoveSpeed = 100;
	m_Hero_GullyHPDelegater.broadcast(TimeDelta, iMoveSpeed);

	}

	if (pInstance->Key_Down(DIK_O))
	{
	_uint iShakingTime = 3;
	m_Hero_GullyTestShakingDelegater.broadcast(iShakingTime);
	}*/
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
	/*CModel::MODELDESC Desc;
	Desc.szProtoName = TEXT("Prototype_Component_Model_GullyDungeon");
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_GullyDungeon"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom,&Desc)))
		return E_FAIL;*/

	//Desc.szProtoName = TEXT("Prototype_Component_Model_GullyDungeon");
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