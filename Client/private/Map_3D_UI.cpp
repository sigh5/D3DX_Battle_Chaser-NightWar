#include "stdafx.h"
#include "..\public\Map_3D_UI.h"
#include "GameInstance.h"
#include "PlayerController.h"
#include "UI.h"
CMap_3D_UI::CMap_3D_UI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnvironment_Object(pDevice, pContext)
{
}

CMap_3D_UI::CMap_3D_UI(const CMap_3D_UI & rhs)
	: CEnvironment_Object(rhs)
{
}

HRESULT CMap_3D_UI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CMap_3D_UI::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Scaled(_float3(0.2f, 0.2f, 0.2f));
	return S_OK;
}

HRESULT CMap_3D_UI::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_pTransformCom->Set_TransfromDesc(0.5f, 90.f);

	m_bLast_Initlize = true;
	return S_OK;
}

void CMap_3D_UI::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	Coll_CaptinPlayer();

	m_fMoveTimer += _float(TimeDelta);

	if (m_fMoveTimer >= 1.f)
	{
		m_iLeft_Right *= -1;
		m_fMoveTimer = 0;
		m_pTransformCom->Set_TransfromDesc(0.1f*m_iLeft_Right, 90.f);
	}
	
	m_pTransformCom->Go_Left(TimeDelta);
	if (m_bRenderFont)
	{
		ImGui::InputFloat("PosX", &m_FontPosX);
		ImGui::InputFloat("PosY", &m_FontPosY);
		ImGui::InputFloat("SizeX", &m_FontSizeX);
		ImGui::InputFloat("SizeY", &m_FontSizeY);
	}

}

void CMap_3D_UI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMap_3D_UI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);		// UI 1�� ���ĺ���
	m_pVIBufferCom->Render();

#ifdef _DEBUG
	m_pColliderCom->Render();

#endif // !_DEBUG

	if (m_bRenderFont)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Render_Font(TEXT("Font_Comic"), TEXT("���� ����!"), _float2(560.f, 90.f), 0.f, _float2(m_FontSizeX, m_FontSizeY), XMVectorSet(1.f, 0.f, 0.f, 1.f));
		RELEASE_INSTANCE(CGameInstance);

	}
	return S_OK;
}

void CMap_3D_UI::Coll_CaptinPlayer()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CPlayerController* pPlayerController = GET_INSTANCE(CPlayerController);
	CCollider* pCaptinColl = static_cast<CCollider*>(pPlayerController->Get_Captin()->Get_Component(L"Com_OBB"));

	if (m_pColliderCom->Collision(pCaptinColl))
	{
		if (!lstrcmp(m_ObjectName, TEXT("Map_3D_UI0")) && !m_bOnce)
		{
			pGameInstance->Load_Object(TEXT("OneMonsterStart"), LEVEL_GAMEPLAY);
			m_bOnce = true;
			m_bRenderFont = true;

			CUI*pCanvas  =static_cast<CUI*>(pGameInstance->Get_GameObject(pGameInstance->GetCurLevelIdx(), LAYER_UI, TEXT("DungeonCanvas")));
			pCanvas->Set_RenderActive(false);

		}
		else if (!lstrcmp(m_ObjectName, TEXT("Map_3D_UI1")) && !m_bOnce)
		{
			pGameInstance->Load_Object(TEXT("twoMonsterStart"), LEVEL_GAMEPLAY);
			m_bOnce = true;
			m_bRenderFont = true;
			CUI*pCanvas = static_cast<CUI*>(pGameInstance->Get_GameObject(pGameInstance->GetCurLevelIdx(), LAYER_UI, TEXT("DungeonCanvas")));
			pCanvas->Set_RenderActive(false);
		}
		else if (!lstrcmp(m_ObjectName, TEXT("Map_3D_UI2")) && !m_bOnce)
		{
			pGameInstance->Load_Object(TEXT("twoMonsterStart"), LEVEL_GAMEPLAY);
			m_bOnce = true;
			m_bRenderFont = true;
			CUI*pCanvas = static_cast<CUI*>(pGameInstance->Get_GameObject(pGameInstance->GetCurLevelIdx(), LAYER_UI, TEXT("DungeonCanvas")));
			pCanvas->Set_RenderActive(false);
		}
		else if (!lstrcmp(m_ObjectName, TEXT("Map_3D_UI3")) && !m_bOnce)
		{
			pGameInstance->Load_Object(TEXT("ThirdMonsterStart"), LEVEL_GAMEPLAY);
			m_bOnce = true;
			m_bRenderFont = true;
			CUI*pCanvas = static_cast<CUI*>(pGameInstance->Get_GameObject(pGameInstance->GetCurLevelIdx(), LAYER_UI, TEXT("DungeonCanvas")));
			pCanvas->Set_RenderActive(false);
		}
	}

	if (ImGui::IsMouseClicked(1))
	{
		CUI*pCanvas = static_cast<CUI*>(pGameInstance->Get_GameObject(pGameInstance->GetCurLevelIdx(), LAYER_UI, TEXT("DungeonCanvas")));
		pCanvas->Set_RenderActive(true);
	}


	RELEASE_INSTANCE(CPlayerController);
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CMap_3D_UI::SetUp_Components()
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
	//m_EnviromentDesc.m_pTextureTag
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_EnviromentDesc.m_pTextureTag, TEXT("Com_Texture"),
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

HRESULT CMap_3D_UI::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

CMap_3D_UI * CMap_3D_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMap_3D_UI*		pInstance = new CMap_3D_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMap_3D_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMap_3D_UI::Clone(void * pArg)
{
	CMap_3D_UI*		pInstance = new CMap_3D_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMap_3D_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMap_3D_UI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}