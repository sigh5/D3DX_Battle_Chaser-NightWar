#include "stdafx.h"
#include "..\public\SlimeKing.h"

#include "GameInstance.h"
#include "MonsterFsm.h"
#include "CombatController.h"
#include "Weapon.h"


CSlimeKing::CSlimeKing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CMonster(pDevice,pContext)
{
}

CSlimeKing::CSlimeKing(const CSlimeKing & rhs)
	:CMonster(rhs)
{
}

CGameObject * CSlimeKing::Get_Weapon_Or_SkillBody()
{
	for (auto& pParts : m_MonsterParts)
	{
		if (dynamic_cast<CWeapon*>(pParts) != nullptr && m_eWeaponType == dynamic_cast<CWeapon*>(pParts)->Get_Type())
			return pParts;
	}

	return nullptr;
}

_bool CSlimeKing::Calculator_HitColl(CGameObject * pWeapon)
{
	CWeapon* pCurActorWepon = static_cast<CWeapon*>(pWeapon);

	if (nullptr == pCurActorWepon)		//나중에 아래것으로
		return false;
	//	assert(pCurActorWepon != nullptr && "CSkeleton_Naked::Calculator_HitColl");

	return pCurActorWepon->Get_Colider()->Collision(m_pColliderCom);
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
	
	m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-30.f));
	m_pTransformCom->Set_Scaled(_float3(4.f, 4.f, 4.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(25.f, 0.f, -10.f, 1.f));
	m_pModelCom->Set_AnimIndex(0);

	if (FAILED(Ready_Parts()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CSlimeKing::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pFsmCom = CMonsterFsm::Create(this, ANIM_CHAR1);
	m_vOriginPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	m_bLast_Initlize = true;
	return S_OK;
}

void CSlimeKing::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	m_pFsmCom->Tick(TimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	/*ImGui::Text("SlimeKing");
	ImGui::InputFloat("SlimeKing_SpeedRatio", &m_SpeedRatio);
	ImGui::InputFloat("SlimeKing_LimitDistance", &m_LimitDistance);
	ImGui::InputFloat("SlimeKing_ReturnDistance", &m_ReturnDistance);
	ImGui::InputFloat("SlimeKing_setTickForSecond", &m_setTickForSecond);
	ImGui::NewLine();
*/

	m_pModelCom->Play_Animation(TimeDelta,true);	// 몬스터들은 다 컴뱃씬에만있으니까
}

void CSlimeKing::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CurAnimQueue_Play_LateTick(m_pModelCom);

	for (_uint i = 0; i < m_MonsterParts.size(); ++i)
	{
		m_MonsterParts[i]->Late_Tick(TimeDelta);
	}

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

#ifdef _DEBUG
	m_pColliderCom->Render();

#endif // !_DEBUG

	return S_OK;
}

void CSlimeKing::Combat_Tick(_double TimeDelta)
{
	Is_MovingAnim();
	CombatAnim_Move(TimeDelta);
	
	if (m_iMovingDir == ANIM_DIR_STRAIGHT || m_iMovingDir == ANIM_DIR_BACK)
	{
		MovingAnimControl(TimeDelta);
	}
	else
		CurAnimQueue_Play_Tick(TimeDelta, m_pModelCom);

	for (_uint i = 0; i < m_MonsterParts.size(); ++i)
	{
		m_MonsterParts[i]->Tick(TimeDelta);
	}
}

_int CSlimeKing::Is_MovingAnim()
{
	if (m_pModelCom->Get_AnimIndex() == 9)
		m_iMovingDir = ANIM_DIR_STRAIGHT;
	else if (m_pModelCom->Get_AnimIndex() == 4)
		m_iMovingDir = ANIM_DIR_BACK;
	else
		m_iMovingDir = ANIM_EMD;

	return m_iMovingDir;
}

void CSlimeKing::CombatAnim_Move(_double TImeDelta)
{
	if (m_pHitTarget == nullptr || m_iMovingDir == ANIM_EMD)
		return;

	if (m_iMovingDir == ANIM_DIR_BACK)
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(m_vOriginPos, TImeDelta, m_ReturnDistance, m_SpeedRatio);

	else if (m_iMovingDir == ANIM_DIR_STRAIGHT)
	{
		_float4 Target;
		XMStoreFloat4(&Target, m_pHitTarget->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
		m_bCombatChaseTarget = m_pTransformCom->CombatChaseTarget(XMLoadFloat4(&Target), TImeDelta, m_LimitDistance, m_SpeedRatio);
	}
}

void CSlimeKing::MovingAnimControl(_double TimeDelta)
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
			m_bIsCombatAndAnimSequnce = true;
	}
}

void CSlimeKing::Fsm_Exit()
{
	_uint iTestNum = 0;
	_double TEst = 0.0;

	m_Monster_CombatTurnDelegeter.broadcast(TEst, iTestNum);
	m_pHitTarget = nullptr;
}

_bool CSlimeKing::IsCollMouse()
{
	return m_pColliderCom->Collision_Mouse(g_hWnd);
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


	CCollider::COLLIDERDESC			ColliderDesc;

	/* For.Com_AABB */
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vSize = _float3(1.3f, 1.5f, 1.3f);
	ColliderDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vCenter = _float3(0.2f, ColliderDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_AABB"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
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

HRESULT CSlimeKing::Ready_Parts()
{
	CGameObject*		pPartObject = nullptr;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CWeapon::WEAPONDESC			WeaponDesc;
	ZeroMemory(&WeaponDesc, sizeof(CWeapon::WEAPONDESC));

	WeaponDesc.PivotMatrix = m_pModelCom->Get_PivotFloat4x4();

	WeaponDesc.pSocket = m_pModelCom->Get_BonePtr("Bone_Slime_Head");
	WeaponDesc.pTargetTransform = m_pTransformCom;
	XMStoreFloat4(&WeaponDesc.vPosition, XMVectorSet(0.f, -0.0f, -0.0f, 1.f));
	XMStoreFloat3(&WeaponDesc.vScale, XMVectorSet(1.f, 1.f, 1.f, 0.f));
	WeaponDesc.eType = WEAPON_HEAD;
	// 이놈은 아마 2개써야될듯?

	Safe_AddRef(WeaponDesc.pSocket);
	Safe_AddRef(m_pTransformCom);

	pPartObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Weapon"), &WeaponDesc);
	if (nullptr == pPartObject)
		return E_FAIL;
	m_MonsterParts.push_back(pPartObject);

	pPartObject->Get_Transform()->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.0f));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CSlimeKing::Anim_Idle()
{
	m_CurAnimqeue.push({ 0,  1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Intro()
{
	m_CurAnimqeue.push({ 12,m_IntroTimer });	//2개 넣어야함.. 처음에
	m_CurAnimqeue.push({ 0, 1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::AnimNormalAttack()
{
	m_CurAnimqeue.push({ 8,1.f });
	m_CurAnimqeue.push({ 9,1.f });
	m_CurAnimqeue.push({ 10,1.f });
	m_CurAnimqeue.push({ 4,1.f });
	m_CurAnimqeue.push({ 5,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Skill1_Attack()
{
	m_CurAnimqeue.push({ 16,1.f }); // 브레쓰
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Uitimate()
{
	m_CurAnimqeue.push({ 14,1.f });  // 몸커지기 
	m_CurAnimqeue.push({ 15,1.f }); 
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Buff()
{
	m_CurAnimqeue.push({ 3,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Light_Hit()
{
	m_CurAnimqeue.push({ 7,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Heavy_Hit()
{
	m_CurAnimqeue.push({ 6,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Die()
{
	m_CurAnimqeue.push({ 2,1.f });
	m_CurAnimqeue.push({ 17,1.f });
	Set_CombatAnim_Index(m_pModelCom);
}

void CSlimeKing::Anim_Viroty()
{
	m_CurAnimqeue.push({ 12,1.f });
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

	for (auto& pPart : m_MonsterParts)
		Safe_Release(pPart);
	m_MonsterParts.clear();

	Safe_Release(m_pFsmCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}