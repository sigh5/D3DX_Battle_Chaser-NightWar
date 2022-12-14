#include "stdafx.h"
#include "..\public\SlimeKing.h"

#include "GameInstance.h"
#include "AnimFsm.h"

CSlimeKing::CSlimeKing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice,pContext)
{
}

CSlimeKing::CSlimeKing(const CSlimeKing & rhs)
	:CMonster(rhs)
{
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

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 10.f, 1.f));


	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}

HRESULT CSlimeKing::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pFsmCom = CAnimFsm::Create(this, ANIM_CHAR4);

	m_bLast_Initlize = true;
	return S_OK;
}

void CSlimeKing::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


	m_pFsmCom->Tick(TimeDelta);
	//Combat_Tick(TimeDelta);

	m_pModelCom->Play_Animation(TimeDelta);
}

void CSlimeKing::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
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
	CSlimeKing::CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
}

_bool CSlimeKing::DemoTest()
{
	if (GetKeyState('Z') & 0x8000)
	{
		return true;
	}

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

	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CSlimeKing::Anim_Idle()
{
	m_CurAnimqeue.push({ 2,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Intro()
{
	m_CurAnimqeue.push({ 1,1.f });
	m_CurAnimqeue.push({ 3,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::AnimNormalAttack()
{
	m_CurAnimqeue.push({ 4,1.f });
	m_CurAnimqeue.push({ 5,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Skill1_Attack()
{
	m_CurAnimqeue.push({ 6,1.f });
	m_CurAnimqeue.push({ 7,1.f });
	m_CurAnimqeue.push({ 8,1.f });
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

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}