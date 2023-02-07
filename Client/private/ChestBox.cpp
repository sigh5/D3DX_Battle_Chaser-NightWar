#include "stdafx.h"
#include "..\public\ChestBox.h"
#include "GameInstance.h"
#include "PlayerController.h"

CChestBox::CChestBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnvironment_Object(pDevice, pContext)
{
}

CChestBox::CChestBox(const CChestBox & rhs)
	: CEnvironment_Object(rhs)
{
}

HRESULT CChestBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}
HRESULT CChestBox::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION,XMVectorSet(0.22f,0.f, 1.f, 1.f));


	return S_OK;
}

HRESULT CChestBox::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pModelCom->Set_AnimIndex(0);
	m_pTransformCom->Set_Scaled(_float3(0.1f, 0.1f, 0.1f));
	m_bLast_Initlize = true;
	return S_OK;
}

void CChestBox::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

#ifdef NOMODLES

#else
	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CCollider* pCaptinColl = static_cast<CCollider*>(pPlayerController->Get_Captin()->Get_Component(L"Com_OBB"));
	
	if (!m_bOnceGet && m_pColliderCom->Collision(pCaptinColl) && pGameInstance->Key_Down(DIK_F))
	{
		m_pModelCom->Set_AnimIndex(2);
		Get_Captin_Player_Item();
		pGameInstance->Play_Sound(TEXT("Common_0037.wav"), 1.f, false, SOUND_ULTIBGM);
		m_bOnceGet = true;
	}

	RELEASE_INSTANCE(CPlayerController);
	RELEASE_INSTANCE(CGameInstance);
#endif
	m_pModelCom->Play_Animation_2(TimeDelta);
}

void CChestBox::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CChestBox::Render()
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

void CChestBox::Get_Captin_Player_Item()
{
	CPlayer* pCaptinPlayer = 		CPlayerController::GetInstance()->Get_Captin();

	CStatus* pCaptinStatus = static_cast<CStatus*>(pCaptinPlayer->Get_Component(TEXT("Com_StatusDungeon")));

	assert(nullptr != pCaptinStatus && "CChestBox::Get_Captin_Player_Item");

	pCaptinPlayer->Get_CaptinPlayer_Item(pCaptinStatus);
}

HRESULT CChestBox::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_EnviromentDesc.m_pModelTag, TEXT("Com_Model"),
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

HRESULT CChestBox::SetUp_ShaderResources()
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

CChestBox * CChestBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CChestBox*		pInstance = new CChestBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CChestBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CChestBox::Clone(void * pArg)
{
	CChestBox*		pInstance = new CChestBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkeleton_Naked");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChestBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}