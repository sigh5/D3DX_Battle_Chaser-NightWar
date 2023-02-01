#include "stdafx.h"
#include "..\public\Trail_Effect.h"
#include "GameInstance.h"
#include "Model.h"

CTrail_Effect::CTrail_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice, pContext)
{
}

CTrail_Effect::CTrail_Effect(const CTrail_Effect & rhs)
	: CHitBoxObject(rhs)
{
}

void CTrail_Effect::Set_Desc(tag_Trail_Effect_DESC & Desc, _int iTextureNum)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	memcpy(&m_Desc, &Desc, sizeof(m_Desc));
	m_iTextureNum = iTextureNum;
	
	m_bIsTrailStartCheck = true;
	m_iShaderPass = 0;
	m_iCenterCount = 0;
	m_iCUrCOunt = 0;

	//m_pTransformCom->Set_Scaled(_float3(5.f, 5.f, 5.f));
	m_pVIBufferCom->Set_Point_Instancing_Scale(_float3(3.f, 3.f, 3.f));

	m_vFirstPos = XMVectorSetW(XMLoadFloat3(&m_Desc.pColider->Get_Center()), 1.f);
}

void CTrail_Effect::End_Anim()
{
	_float3  Temp = m_Desc.pColider->Get_Center();
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(XMLoadFloat3(&Temp), 1.f));
	_float3  Sour = m_Desc.pColider->Get_Center();
	_float	fRadius = m_Desc.pColider->Get_Radius();

	Temp.y += fRadius;
	Temp.x += fRadius;

	Sour.y -= fRadius;
	Sour.x -= fRadius;
	

	m_Trail_Pos[1] = XMVectorSetW(XMLoadFloat3(&Temp), 1.f);
	m_Trail_Pos[2] = XMVectorSetW(XMLoadFloat3(&Sour), 1.f);

	
}

void CTrail_Effect::First_Edition()
{
	_float3  Temp, Sour, dest, Temp2;
	_float	fRadius;
	Temp = m_Desc.pColider->Get_Center();
	Sour = m_Desc.pColider->Get_Center();
	fRadius = m_Desc.pColider->Get_Radius();
	dest = m_Desc.pColider->Get_Center();
	Temp2 = m_Desc.pColider->Get_Center();
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(XMLoadFloat3(&Temp), 1.f));


	Temp.x -= fRadius;
	Temp.y += fRadius;
	Temp.z -= fRadius;

	Sour.x -= fRadius;
	Sour.y -= fRadius;
	Sour.z -= fRadius;

	dest.x += fRadius;;
	dest.y += fRadius;
	dest.z += fRadius;


	Temp2.x += fRadius;
	Temp2.y -= fRadius;
	Temp2.z += fRadius;

	m_Trail_Pos[0] = m_Trail_Pos[1];
	m_Trail_Pos[3] = m_Trail_Pos[2];

	m_Trail_Pos[1] = XMVectorSetW(XMLoadFloat3(&dest), 1.f);
	m_Trail_Pos[2] = XMVectorSetW(XMLoadFloat3(&Temp2), 1.f);

	
}

HRESULT CTrail_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail_Effect::Initialize(void * pArg)
{
	m_ObjectName = L"TestTrail";

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pVIBufferCom->Set_Point_Instancing_MainTain();
	m_pVIBufferCom->Set_Point_Instancing_Scale(_float3(1.f, 1.f, 1.f));
	m_pVIBufferCom->Set_Point_Instancing_Num(1);

	
	return S_OK;
}

void CTrail_Effect::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	if(m_bIsTrailStartCheck)
	{

		/*	if(m_bTrail_Maintain)
			{
				m_fTrailTimer += TimeDelta;
				if (m_fTrailTimer > 2.f)
				{
					m_bIsTrailStartCheck = false;
					m_fTrailTimer = 0.f;
					m_iCUrCOunt = 0;
					m_bTrail_Maintain = false;
				}
			}*/

		if (m_pVIBufferCom->UV_Move_Tick(TimeDelta))
		{
			//m_bIsTrailStartCheck = false;
		}

		if (m_iCUrCOunt >= 8 )//&& !m_bTrail_Maintain)
		{
			m_bIsTrailStartCheck = false;
			m_fTrailTimer = 0.f;
			m_iCUrCOunt = 0;
			m_bTrail_Maintain = false;
		}
		/*else if (m_iCUrCOunt >= 8 && m_bTrail_Maintain)
		{
			m_bIsTrailStartCheck = false;
			m_fTrailTimer = 0.f;
			m_iCUrCOunt = 0;
			m_bTrail_Maintain = false;
			return;
		}*/

		if (m_iCenterCount < 8)
		{
			m_Collider_pos[m_iCenterCount] = XMVectorSetW(XMLoadFloat3(&m_Desc.pColider->Get_Center()), 1.f);
		}
		else if (m_iCenterCount >= 8)
		{
			_float3 vTestPos;
			if (m_iCUrCOunt != 0 )
			{
				m_Collider_pos[m_iCUrCOunt] = XMVectorCatmullRom(m_Collider_pos[m_iCUrCOunt - 1],
					m_Collider_pos[m_iCUrCOunt],
					m_Collider_pos[m_iCUrCOunt + 1], m_Collider_pos[m_iCUrCOunt + 1], m_Desc.pModel->Get_Ratio());
			}
			
			XMStoreFloat3(&vTestPos, m_Collider_pos[m_iCUrCOunt]);
			_float3  Temp, Sour, dest, Temp2;
			_float	fRadius;
			Temp = vTestPos;
			Sour = vTestPos;
			fRadius = m_Desc.pColider->Get_Radius();
			dest = vTestPos;
			Temp2 = vTestPos;
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(XMLoadFloat3(&Temp), 1.f));

			Temp.x -= fRadius;
			Temp.y += fRadius *3;
			Temp.z -= fRadius;

			Sour.x -= fRadius;
			Sour.y -= fRadius *3;
			Sour.z -= fRadius;

			dest.x += fRadius;
			dest.y += fRadius *3;
			dest.z += fRadius;


			Temp2.x += fRadius;
			Temp2.y -= fRadius *3;
			Temp2.z += fRadius;

			m_Trail_Pos[0] = m_Trail_Pos[1];
			m_Trail_Pos[3] = m_Trail_Pos[2];

			m_Trail_Pos[1] = XMVectorSetW(XMLoadFloat3(&dest), 1.f);
			m_Trail_Pos[2] = XMVectorSetW(XMLoadFloat3(&Temp2), 1.f);
			++m_iCUrCOunt;
			
			//else
			//{
			//	m_fTrailTimer += TimeDelta;
			//	if (m_fTrailTimer > 0.05f)
			//	{
			//		m_bIsTrailStartCheck = false;
			//		m_fTrailTimer = 0.f;
			//		m_iCUrCOunt = 0;
			//	}
			//}
		}
		
		++m_iCenterCount;


		
	}
}

void CTrail_Effect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CTrail_Effect::Render()
{
	if(m_bIsTrailStartCheck== true)
	{ 
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(SetUp_ShaderResources()))
			return E_FAIL;

		m_pShaderCom->Begin(m_iShaderPass);
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CTrail_Effect::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;


	m_bLast_Initlize = true;
	return S_OK;
}

HRESULT CTrail_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxTrail"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CVIBuffer_Point_Instancing::VIBUffer_Point_TextureDesc PointDesc;
	ZeroMemory(&PointDesc, sizeof(PointDesc));

	memcpy(&PointDesc, &m_HitBoxDesc.Poing_Desc, sizeof(PointDesc));

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Single"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom, &PointDesc)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_HitBoxDesc.HitBoxOrigin_Desc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrail_Effect::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositon(), sizeof(_float4))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pVIBufferCom->Set_UV_RawValue(m_pShaderCom)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureNum)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldTrailPos", &m_Trail_Pos, sizeof(_vector)* 4)))
		return E_FAIL;

	_float fLife = 12.f;
	_float fCurLife = (_float)m_iCenterCount;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Life", &fLife, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_CurLife", &fCurLife, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_FirstPos", &m_vFirstPos, sizeof(_vector))))
		return E_FAIL;

	return S_OK;
}

CTrail_Effect * CTrail_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrail_Effect*		pInstance = new CTrail_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCam_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTrail_Effect::Clone(void * pArg)
{
	CTrail_Effect*		pInstance = new CTrail_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCam_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrail_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
