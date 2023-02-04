#include "stdafx.h"
#include "..\public\Mesh_Effect.h"
#include "GameInstance.h"
#include "CombatController.h"
#include "Player.h"
CMesh_Effect::CMesh_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice, pContext)
{
}

CMesh_Effect::CMesh_Effect(const CMesh_Effect & rhs)
	: CHitBoxObject(rhs)
{
}

void CMesh_Effect::Set_Client_BuffDesc(MeshEffcet_Client & Desc)
{
	memcpy(&m_Client_MeshEffect_Desc, &Desc, sizeof(MeshEffcet_Client));
	m_bIsFinsishBuffer = false;
	Reset_CurMartirx();


	m_iHitNum = m_Client_MeshEffect_Desc.iHitNum;
}

void CMesh_Effect::Set_Glow(_bool bUseGlow, wstring GlowTag)
{
	m_bUseGlow = bUseGlow;
	m_GlowstrTag = GlowTag;

	if (true == m_bUseGlow)
	{
		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, GlowTag.c_str(), TEXT("Com_GlowTexture"),
			(CComponent**)&m_pGlowTextureCom)))
			assert(!" CBuff_Effect::Set_Glow");

		m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass = 1; //Glow
	}
}


HRESULT CMesh_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh_Effect::Initialize(void * pArg)
{
	//m_ObjectName = L"testMEsh_Effect";

	ZeroMemory(&m_Client_MeshEffect_Desc, sizeof(m_Client_MeshEffect_Desc));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;
#ifdef NOMODLES
	m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass = 0;
#else
	//m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass = 1;
#endif

	m_pTransformCom->Set_TransfromDesc(20.f, 90.f);

	return S_OK;
}

void CMesh_Effect::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


#ifdef NOMODLES

	ImGui::InputInt("iFrameCnt", &m_Client_MeshEffect_Desc.iFrameCnt);
	ImGui::InputInt("iFrameMaxNum", &m_Client_MeshEffect_Desc.iMaxNum);


#else

#endif
	if (ImGui::Button("Test"))
		m_iPlayOnFrameCnt = 0;
	
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());


	m_iPlayOnFrameCnt++;

	if (m_iPlayOnFrameCnt == m_Client_MeshEffect_Desc.iFrameCnt)
	{
		m_iPlayOnFrameCnt = 0;
		++m_iTextureNum;

		if (m_iTextureNum >= m_Client_MeshEffect_Desc.iMaxNum)
		{
			m_iTextureNum = 0;

			m_bIsFinsishBuffer = true;
			m_bGlowEnd = true;
		}
	}

	if (m_Client_MeshEffect_Desc.bSlimeUltimate)
	{
		/*	_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

		vPos.x -= _float(TimeDelta * 10.f);
		vPos.y -= _float(TimeDelta * 5.f); 
		vPos.z += _float(TimeDelta * 10.f);

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));*/
		CGameObject* pGameObject = nullptr;
		_float4 vPos;
		
		if (m_Client_MeshEffect_Desc.bKnolan)
		{
			pGameObject = CCombatController::GetInstance()->Get_Player(TEXT("Hero_Gully"));
			XMStoreFloat4(&vPos, pGameObject->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
			m_pTransformCom->Chase(XMLoadFloat4(&vPos), TimeDelta, 0.5f);
		}
		else if (m_Client_MeshEffect_Desc.bGarrison)
		{
			pGameObject = CCombatController::GetInstance()->Get_Player(TEXT("Hero_Alumon"));
			XMStoreFloat4(&vPos, pGameObject->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
			if (m_pTransformCom->Chase(XMLoadFloat4(&vPos), TimeDelta, 0.1f))
				m_bIsFinsishBuffer = true;
		}

		else if (m_Client_MeshEffect_Desc.bBretto)
		{
			pGameObject = CCombatController::GetInstance()->Get_Player(TEXT("Hero_Calibretto"));
			XMStoreFloat4(&vPos, pGameObject->Get_Transform()->Get_State(CTransform::STATE_TRANSLATION));
			m_pTransformCom->Chase(XMLoadFloat4(&vPos), TimeDelta, 0.5f);
		}
		assert(nullptr != pGameObject &&"CMesh_Effect_Tick Issue");
		CCollider* pColider = dynamic_cast<CCollider*>(pGameObject->Get_Component(TEXT("Com_OBB")));
		assert(nullptr != pColider &&"CMesh_Effect_Tick Issue");

		if (m_pColliderCom->Collision(pColider))
		{
			static_cast<CPlayer*>(pGameObject)->Boss_Ultimate_Anim();
			m_bIsFinsishBuffer = true;
			CGameInstance::GetInstance()->Play_Sound(TEXT("Monster_Slime_Bullet.wav"), 1.f, false, SOUND_MONSTER3);
		}
	}

	if (m_bUseGlow)
	{
		if (m_fGlowStrength >= 1.f)
			m_bIsChange = true;
		else if (m_fGlowStrength <= 0)
			m_bIsChange = false;

		if (m_bIsChange == true)
			m_fGlowStrength += (_float)TimeDelta * -1.f;
		else
			m_fGlowStrength += (_float)TimeDelta;
	}
}

void CMesh_Effect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
	}
}

HRESULT CMesh_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Render(m_pShaderCom, i, m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass);
	}
	return S_OK;
}

HRESULT CMesh_Effect::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_bLast_Initlize = true;
	return S_OK;
}

HRESULT CMesh_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModelUVMove"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_HitBoxDesc.HitBoxOrigin_Desc.m_pModelTag, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_HitBoxDesc.HitBoxOrigin_Desc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vRotation = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.0f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMesh_Effect::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureNum)))
		return E_FAIL;

	if (m_bUseGlow)
	{
		if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_GlowTexture", m_iTextureNum)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("G_Power", &m_fGlowStrength, sizeof(_float))))
			return E_FAIL;

	}
	
	return S_OK;
}

void CMesh_Effect::Reset_CurMartirx()
{
	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	matScale = XMMatrixScaling(m_Client_MeshEffect_Desc.vScale.x, m_Client_MeshEffect_Desc.vScale.y, m_Client_MeshEffect_Desc.vScale.z);
	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(m_Client_MeshEffect_Desc.vAngle));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(m_Client_MeshEffect_Desc.vPosition.x, m_Client_MeshEffect_Desc.vPosition.y, m_Client_MeshEffect_Desc.vPosition.z);
	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;

	m_OriginMatrix *= m_Client_MeshEffect_Desc.ParentTransform->Get_WorldMatrix();

	XMStoreFloat4x4(&m_ConvertCurMatrix, m_OriginMatrix);

	m_pTransformCom->Set_WorldMatrix(m_ConvertCurMatrix);
}

CMesh_Effect * CMesh_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMesh_Effect*		pInstance = new CMesh_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMesh_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMesh_Effect::Clone(void * pArg)
{
	CMesh_Effect*		pInstance = new CMesh_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMesh_Effect::Free()
{
	__super::Free();
	if (m_bUseGlow)
	{
		Safe_Release(m_pGlowTextureCom);
	}
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}