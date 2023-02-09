#include "stdafx.h"
#include "..\public\Buff_Effect.h"
#include "GameInstance.h"
#include "Bone.h"

CBuff_Effect::CBuff_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice, pContext)
{
}

CBuff_Effect::CBuff_Effect(const CBuff_Effect & rhs)
	: CHitBoxObject(rhs)
{
}

void CBuff_Effect::Set_Client_BuffDesc(BuffEffcet_Client & Desc)
{
	memcpy(&m_Client_BuffEffect_Desc, &Desc, sizeof(BuffEffcet_Client)); 
	m_bIsFinsishBuffer = false;
	Reset_CurMartirx();
}

void CBuff_Effect::Set_Client_BuffDesc(BuffEffcet_Client & Desc, CBone * pSocket,_float4x4 PivotMatrix)
{
	memcpy(&m_Client_BuffEffect_Desc, &Desc, sizeof(BuffEffcet_Client));

	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(m_Client_BuffEffect_Desc.vAngle));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(m_Client_BuffEffect_Desc.vPosition.x, m_Client_BuffEffect_Desc.vPosition.y, m_Client_BuffEffect_Desc.vPosition.z);
	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;

	_matrix			SocketMatrix = pSocket->Get_OffsetMatrix() * pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&PivotMatrix);
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	SocketMatrix = SocketMatrix *	m_OriginMatrix* m_Client_BuffEffect_Desc.ParentTransform->Get_WorldMatrix();
	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	//m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	//m_pVIBufferCom->Set_Point_Instancing_Scale(m_Client_BuffEffect_Desc.vScale);	// 텍스쳐의 크기를 키우는것
	//m_pTransformCom->Set_Scaled(m_Client_BuffEffect_Desc.vScale);				// 콜라이더의 크기를 키우는것임


	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	m_pVIBufferCom->Set_Point_Instancing_MainTain();
	m_pVIBufferCom->Set_Point_Instancing_Scale(m_Client_BuffEffect_Desc.vScale);	// 텍스쳐의 크기를 키우는것
	m_pTransformCom->Set_Scaled(m_Client_BuffEffect_Desc.vScale);				// 콜라이더의 크기를 키우는것임
	m_vScale = m_Client_BuffEffect_Desc.vScale;
	m_pVIBufferCom->Set_FrameCnt(m_Client_BuffEffect_Desc.iFrameCnt);

	/*if (m_Client_BuffEffect_Desc.bIsDown)
	{
		__super::Add_Component(LEVEL_GAMEPLAY,TEXT("Prototype_Component_Texture_AlumonSkill"), TEXT("Com_GlowTexture"),
			(CComponent**)&m_pGlowTextureCom);

		m_iDownGlowTextureNum = 6;


	}*/
}



void CBuff_Effect::Set_Glow(_bool bUseGlow, wstring GlowTag,_int iGlowTextureNumber)
{
	m_bUseGlow = bUseGlow;
	m_GlowstrTag = GlowTag;
	m_iGlowTextureNum = iGlowTextureNumber;

	if(true ==m_bUseGlow )
	{
	/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, GlowTag.c_str(), TEXT("Com_GlowTexture"),
			(CComponent**)&m_pGlowTextureCom)))
			assert(!" CBuff_Effect::Set_Glow");

		m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass = 1; //Glow
	}

}

void CBuff_Effect::Set_Dissove(_bool bUseGlow, wstring GlowTag, _int iGlowTextureNumber)
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, GlowTag.c_str(), TEXT("Com_DeTexture"),
		(CComponent**)&m_pDeTextureCom)))
		assert(!" CBuff_Effect::Set_Glow");

	m_iDeSolveNum = iGlowTextureNumber;

}

void CBuff_Effect::Set_ShaderPass(_uint iShaderPass)
{
	m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass = iShaderPass;
}

void CBuff_Effect::Set_CamEffect(BuffEffcet_Client & Desc)
{
	memcpy(&m_Client_BuffEffect_Desc, &Desc, sizeof(BuffEffcet_Client));
	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(m_Client_BuffEffect_Desc.vAngle));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(m_Client_BuffEffect_Desc.vPosition.x, m_Client_BuffEffect_Desc.vPosition.y, m_Client_BuffEffect_Desc.vPosition.z);
	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;
	

	XMStoreFloat4x4(&m_SocketMatrix, m_OriginMatrix);
	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	m_pVIBufferCom->Set_Point_Instancing_MainTain();
	m_pVIBufferCom->Set_Point_Instancing_Scale(m_Client_BuffEffect_Desc.vScale);	// 텍스쳐의 크기를 키우는것
	m_pTransformCom->Set_Scaled(m_Client_BuffEffect_Desc.vScale);				// 콜라이더의 크기를 키우는것임
	m_vScale = m_Client_BuffEffect_Desc.vScale;
	m_pVIBufferCom->Set_FrameCnt(m_Client_BuffEffect_Desc.iFrameCnt);
}

void CBuff_Effect::Is_Particle_Effect(_int iInstanceNum)
{
	m_pVIBufferCom->Set_Point_Instancing_Num(iInstanceNum);
	m_bIsUpdown = true;
}

HRESULT CBuff_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuff_Effect::Initialize(void * pArg)
{
	ZeroMemory(&m_Client_BuffEffect_Desc, sizeof(m_Client_BuffEffect_Desc));

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	return S_OK;
}

void CBuff_Effect::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


	if (m_pVIBufferCom->UV_Move_Tick(TimeDelta) && m_Client_BuffEffect_Desc.bIsMainTain ==false)
	{
		m_bIsFinsishBuffer = true;
	}
	else
	{
		if (m_Client_BuffEffect_Desc.bIsUp)
		{
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

			vPos.y += (_float)(5 * TimeDelta);
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		}

		if (m_Client_BuffEffect_Desc.bIsStraight)
		{
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			vPos.x += (_float)(35.f * TimeDelta);
			vPos.y += (_float)(0.5f * TimeDelta);
			vPos.z -= (_float)(25.f * TimeDelta);
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		}

		if (m_Client_BuffEffect_Desc.bIsBack)
		{
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			vPos.x -= (_float)(10.f * TimeDelta);
			vPos.y += (_float)(0.5f * TimeDelta);
			vPos.z += (_float)(10.f * TimeDelta);
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
		}
		if (m_Client_BuffEffect_Desc.bIsDown)
		{
			m_fMaxYDown += (_float)(7.f * TimeDelta);;
			if (m_fMaxYDown <= 2.5f)
			{
				_float4 vPos;
				XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
				//vPos.x -= (_float)(10.f * TimeDelta);
				vPos.y -= (_float)(7.f * TimeDelta);
				

				//vPos.z += (_float)(10.f * TimeDelta);
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
			}
		}


	}

	if (m_bIsUpdown)
	{
		m_pVIBufferCom->Tick_UpDown(TimeDelta);
	}
	
	
	if (m_bUseGlow && m_Client_BuffEffect_Desc.bIsDown==false)
	{
		m_fIncraseX +=  2.f*(_float)TimeDelta ;

		if (m_fIncraseX >= 1.f)
			m_fIncraseX = 1.f;

		if (m_fGlowStrength >= 1.f)
		{
			//m_bIsChange = true;
			return;
		}
		else if (m_fGlowStrength <= 0)
			m_bIsChange = false;

		if (m_bIsChange == true)
			m_fGlowStrength += (_float)TimeDelta * -1.f ;
		else
			m_fGlowStrength += (_float)TimeDelta;


	}

	if (m_Client_BuffEffect_Desc.bIsDown)
	{
		m_fIncraseX += 2.f*(_float)TimeDelta;

		if (m_fIncraseX >= 1.f)
			m_fIncraseX = 1.f;

		if (m_fGlowStrength >= 0.5f)
		{
			m_bIsChange = true;
			//return;
		}
		else if (m_fGlowStrength <= 0)
			m_bIsChange = false;

		if (m_bIsChange == true)
			m_fGlowStrength += (_float)TimeDelta * -1.f *5.f;
		else
			m_fGlowStrength += (_float)TimeDelta * 5.f;
	}




#ifdef NOMODLES
	m_HitBoxDesc.Poing_Desc = m_pVIBufferCom->Get_Point_TextureDesc();
#else
	
#endif	

}

void CBuff_Effect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBuff_Effect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass);
	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBuff_Effect::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;


	//m_pVIBufferCom->Set_FrameCnt(10);

	m_bLast_Initlize = true;
	return S_OK;
}

HRESULT CBuff_Effect::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Shader_VtxTexIncCount"), TEXT("Com_Shader"),
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

HRESULT CBuff_Effect::SetUp_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;
	

	if (m_bUseGlow == true)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("G_Power", &m_fGlowStrength ,sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_GlowTexture", m_iGlowTextureNum)))
			return E_FAIL;

		if (nullptr != m_pDeTextureCom)
		{
			if (FAILED(m_pDeTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", m_iDeSolveNum)))
				return E_FAIL;
		}
	
	}

	/*if (m_Client_BuffEffect_Desc.bIsDown)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("G_Power", &m_fGlowStrength, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pGlowTextureCom->Bind_ShaderResource(m_pShaderCom, "g_GlowTexture", 6)))
			return E_FAIL;
	}
*/

	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVX_InCrease", &m_fIncraseX, sizeof(_float))))
		return E_FAIL;

	
	return S_OK;
}

void CBuff_Effect::Reset_CurMartirx()
{
	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(m_Client_BuffEffect_Desc.vAngle));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(m_Client_BuffEffect_Desc.vPosition.x, m_Client_BuffEffect_Desc.vPosition.y, m_Client_BuffEffect_Desc.vPosition.z);
	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;
	_matrix			SocketMatrix;
	if (m_Client_BuffEffect_Desc.pSocket != nullptr)
	{
		SocketMatrix = m_Client_BuffEffect_Desc.pSocket->Get_OffsetMatrix() * m_Client_BuffEffect_Desc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_Client_BuffEffect_Desc.PivotMatrix);
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
		SocketMatrix = SocketMatrix *	m_OriginMatrix* m_Client_BuffEffect_Desc.ParentTransform->Get_WorldMatrix();
	}
	else
		SocketMatrix = m_OriginMatrix* m_Client_BuffEffect_Desc.ParentTransform->Get_WorldMatrix();

	//if (m_Client_BuffEffect_Desc.bIsMainTain)
	//{
	//	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);
	//	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	//	m_pVIBufferCom->Set_Point_Instancing_MainTain();
	//	m_pVIBufferCom->Set_Point_Instancing_Scale(m_Client_BuffEffect_Desc.vScale);	// 텍스쳐의 크기를 키우는것
	//	m_pTransformCom->Set_Scaled(m_Client_BuffEffect_Desc.vScale);				// 콜라이더의 크기를 키우는것임
	//	m_vScale = m_Client_BuffEffect_Desc.vScale;
	//	m_pVIBufferCom->Set_FrameCnt(m_Client_BuffEffect_Desc.iFrameCnt);
	//}
	//else
	
	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);
	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	m_pVIBufferCom->Set_Point_Instancing_MainTain();
	m_pVIBufferCom->Set_Point_Instancing_Scale(m_Client_BuffEffect_Desc.vScale);	// 텍스쳐의 크기를 키우는것
	m_pTransformCom->Set_Scaled(m_Client_BuffEffect_Desc.vScale);				// 콜라이더의 크기를 키우는것임
	m_vScale = m_Client_BuffEffect_Desc.vScale;
	m_pVIBufferCom->Set_FrameCnt(m_Client_BuffEffect_Desc.iFrameCnt);
}


CBuff_Effect * CBuff_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBuff_Effect*		pInstance = new CBuff_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBuff_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBuff_Effect::Clone(void * pArg)
{
	CBuff_Effect*		pInstance = new CBuff_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBuff_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuff_Effect::Free()
{
	__super::Free();
	
	if (m_bUseGlow)
	{
		Safe_Release(m_pGlowTextureCom);
	}

	Safe_Release(m_pDeTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}