#include "stdafx.h"
#include "..\public\CollObject.h"
#include "GameInstance.h"
#include "PlayerController.h"
#include "Client_Manager.h"
#include "NpcCanvas.h"

CCollObject::CCollObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnvironment_Object(pDevice, pContext)
{
}

CCollObject::CCollObject(const CCollObject & rhs)
	: CEnvironment_Object(rhs)
{
}

HRESULT CCollObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CCollObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Scaled(_float3(0.2f, 0.2f, 0.2f));
	return S_OK;
}

HRESULT CCollObject::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;


	m_bLast_Initlize = true;
	return S_OK;
}

void CCollObject::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

#ifdef NOMODLES
#else
	Coll_CaptinPlayer();
#endif
}

void CCollObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
	}
}

HRESULT CCollObject::Render()
{

	if (FAILED(__super::Render()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);		// UI 1번 알파블랜딩
	m_pVIBufferCom->Render();

	return S_OK;
}

void CCollObject::Coll_CaptinPlayer()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);
	CCollider* pCaptinColl = static_cast<CCollider*>(pPlayerController->Get_Captin()->Get_Component(L"Com_OBB"));

	if (m_pColliderCom->Collision(pCaptinColl) && pGameInstance->Key_Down(DIK_F))
	{
		m_bActive = !m_bActive;
		CGameObject* pGameObject = nullptr;
		pGameObject = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, LAYER_UI, L"NpcCanvas_BG");
		if (true == m_bActive)
		{
			
			if (!lstrcmp(m_ObjectName, TEXT("CollObject_0")))
			{
				static_cast<CNpcCanvas*>(pGameObject)->Set_RenderActive(true);
				static_cast<CNpcCanvas*>(pGameObject)->Set_DialogOption(0);
			}
			else if (!lstrcmp(m_ObjectName, TEXT("CollObject_1")))
			{
				static_cast<CNpcCanvas*>(pGameObject)->Set_RenderActive(true);
				static_cast<CNpcCanvas*>(pGameObject)->Set_DialogOption(1);
			}
			else if (!lstrcmp(m_ObjectName, TEXT("CollObject_2")))
			{
				static_cast<CNpcCanvas*>(pGameObject)->Set_RenderActive(true);
				static_cast<CNpcCanvas*>(pGameObject)->Set_DialogOption(2);
			}
		}
		else
		{
			if (!lstrcmp(m_ObjectName, TEXT("CollObject_0")))
			{
				static_cast<CNpcCanvas*>(pGameObject)->Set_RenderActive(false);
			}
			else if (!lstrcmp(m_ObjectName, TEXT("CollObject_1")))
			{
				static_cast<CNpcCanvas*>(pGameObject)->Set_RenderActive(false);
			}
			else if (!lstrcmp(m_ObjectName, TEXT("CollObject_2")))
			{
				static_cast<CNpcCanvas*>(pGameObject)->Set_RenderActive(false);
			}
		}
	
	}


	RELEASE_INSTANCE(CPlayerController);
	RELEASE_INSTANCE(CGameInstance);
}



HRESULT CCollObject::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_NPC"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
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

HRESULT CCollObject::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 1)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CCollObject * CCollObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCollObject*		pInstance = new CCollObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCollObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCollObject::Clone(void * pArg)
{
	CCollObject*		pInstance = new CCollObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCollObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollObject::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
