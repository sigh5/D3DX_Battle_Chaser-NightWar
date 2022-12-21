#include "stdafx.h"
#include "..\public\Skeleton_Naked.h"

#include "GameInstance.h"
#include "MonsterFsm.h"

#include "CombatController.h"

CSkeleton_Naked::CSkeleton_Naked(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice, pContext)
{
}

CSkeleton_Naked::CSkeleton_Naked(const CSkeleton_Naked & rhs)
	: CMonster(rhs)
{
}

HRESULT CSkeleton_Naked::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CSkeleton_Naked::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Skeleton_Naked");

	CGameObject::GAMEOBJECTDESC			GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof GameObjectDesc);
	GameObjectDesc.TransformDesc.fSpeedPerSec = 7.0f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-30.f));
	m_pTransformCom->Set_Scaled(_float3(4.f, 4.f, 4.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(27.f, 0.f, -4.f, 1.f));

	m_pModelCom->Set_AnimIndex(0);

	return S_OK;
}
HRESULT CSkeleton_Naked::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pFsmCom = CMonsterFsm::Create(this, ANIM_CHAR2);

	m_bLast_Initlize = true;
	return S_OK;
}

void CSkeleton_Naked::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	m_pFsmCom->Tick(TimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Play_Animation(TimeDelta, true);
}

void CSkeleton_Naked::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkeleton_Naked::Render()
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
	m_pColliderCom->Render();

#endif // !_DEBUG
	return S_OK;
}

void CSkeleton_Naked::Combat_Tick(_double TimeDelta)
{
	CMonster::CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);
	Is_MovingAnim();
	CombatAnim_Move(TimeDelta);
}

_int CSkeleton_Naked::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 9)
	{
		m_iMovingDir = ANIM_DIR_STRAIGHT;
		m_pModelCom->Set_Duration(9, 2);
	}
	else if (m_pModelCom->Get_AnimIndex() == 15)
	{
		m_iMovingDir = ANIM_DIR_STRAIGHT;
		m_pModelCom->Set_Duration(15, 2);
	}
	else if (m_pModelCom->Get_AnimIndex() == 4)
	{
		m_iMovingDir = ANIM_DIR_BACK;
		m_pModelCom->Set_Duration(4, 2);
	}
	else
		m_iMovingDir = ANIM_EMD;

	return m_iMovingDir;
}

void CSkeleton_Naked::CombatAnim_Move(_double TImeDelta)
{
	if (m_iMovingDir == ANIM_DIR_STRAIGHT)
		m_pTransformCom->Go_Straight(TImeDelta);

	else if (m_iMovingDir == ANIM_DIR_BACK)
		m_pTransformCom->Go_Backward(TImeDelta);
}

void CSkeleton_Naked::Fsm_Exit()
{
	_uint iTestNum = 0;
	_double TEst = 0.0;

	m_Monster_CombatTurnDelegeter.broadcast(TEst, iTestNum);
}

_bool CSkeleton_Naked::IsCollMouse()
{
	return m_pColliderCom->Collision_Mouse(g_hWnd, m_pTransformCom);
}

HRESULT CSkeleton_Naked::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_COMBAT, TEXT("Prototype_Component_Model_Skeleton_Naked"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC			ColliderDesc;
	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.2f, 1.7f, 1.2f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.2f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkeleton_Naked::SetUp_ShaderResources()
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

void CSkeleton_Naked::Anim_Idle()
{
	m_CurAnimqeue.push({ 0, 1.f });		
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Intro()
{
	m_CurAnimqeue.push({ 13, 1.f });		//이때 안보이게 뭐를 설정해야함
	m_CurAnimqeue.push({ 12, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_NormalAttack()
{
	m_CurAnimqeue.push({ 8, 1.f });	// 한대툭
	m_CurAnimqeue.push({ 9, 1.f });
	m_CurAnimqeue.push({ 10, 1.f });
	m_CurAnimqeue.push({ 4, 1.f });
	m_CurAnimqeue.push({ 5, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Skill1_Attack()
{
	m_CurAnimqeue.push({ 8, 1.f });	// 2대 툭
	m_CurAnimqeue.push({ 15, 1.f });
	m_CurAnimqeue.push({ 16, 0.5f });	// 선형보관 필수?
	m_CurAnimqeue.push({ 4, 0.5f });
	m_CurAnimqeue.push({ 5, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Defence()
{
	m_CurAnimqeue.push({ 14, 1.f });	
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Buff()
{
	m_CurAnimqeue.push({ 3, 1.f }); 
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Light_Hit()
{
	m_CurAnimqeue.push({ 7, 1.f });	 
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Heavy_Hit()
{
	m_CurAnimqeue.push({ 6, 1.f });	 
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Die()
{
	m_CurAnimqeue.push({ 2, 1.f });	 
	m_CurAnimqeue.push({ 18, 1.f });	
	Set_CombatAnim_Index(m_pModelCom);
}

void CSkeleton_Naked::Anim_Viroty()
{
	/*죽었으면 가만히 있기*/
	m_CurAnimqeue.push({ 0, 1.f });	
	Set_CombatAnim_Index(m_pModelCom);
	
}

CSkeleton_Naked * CSkeleton_Naked::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkeleton_Naked*		pInstance = new CSkeleton_Naked(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkeleton_Naked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkeleton_Naked::Clone(void * pArg)
{
	CSkeleton_Naked*		pInstance = new CSkeleton_Naked(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkeleton_Naked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkeleton_Naked::Free()
{
	__super::Free();

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}