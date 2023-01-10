#include "stdafx.h"
#include "..\public\Skill_TextureObj.h"
#include "GameInstance.h"
#include "Bone.h"



CSkill_TextureObj::CSkill_TextureObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice, pContext)
{
}

CSkill_TextureObj::CSkill_TextureObj(const CSkill_TextureObj & rhs)
	: CHitBoxObject(rhs)
{
}


HRESULT CSkill_TextureObj::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_TextureObj::Initialize(void * pArg)
{
	ZeroMemory(&m_SkillDesc, sizeof(m_SkillDesc));

	SKILL_TEXTURE_DESC  Desc;
	ZeroMemory(&Desc, sizeof(Desc));

	HitBoxObject OriginDesc;
	ZeroMemory(&OriginDesc, sizeof(OriginDesc));
	
	if (pArg != nullptr)
	{
		memcpy(&Desc, pArg, sizeof(SKILL_TEXTURE_DESC));
		OriginDesc.TransformDesc.fSpeedPerSec = 35.f;			//60.f;
		OriginDesc.TransformDesc.fRotationPerSec = 90.f;
		memcpy(&OriginDesc.HitBoxOrigin_Desc ,&Desc.HitBoxOrigin_Desc, sizeof(OriginDesc.HitBoxOrigin_Desc));
		memcpy(&OriginDesc.Poing_Desc, &Desc.Poing_Desc, sizeof(OriginDesc.Poing_Desc));
	}

	if (FAILED(__super::Initialize(&OriginDesc)))
		return E_FAIL;
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CSkill_TextureObj::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);

	m_pVIBufferCom->UV_Move_Tick(TimeDelta);
#ifdef NOMODLES
	m_HitBoxDesc.Poing_Desc = m_pVIBufferCom->Get_Point_TextureDesc();
#else
	Skill_DIR eDir = m_SkillDesc.eDir;

	switch (eDir)
	{
	case Skill_DIR_straight:
		Skill_Charging(TimeDelta);
		break;
	case Skill_DIR_LISING:
		Skill_Golf_Shot(TimeDelta);
		break;
	case Skill_DIR_DOWN:
		Set_ChaserBone();
		break;
	case Skill_DIR_ScaleUP_DOWN:
		Skill_Meteo(TimeDelta);
		break;
	case SKILL_TEST:
		Skill_Golf_Shot(TimeDelta);
	case SKILL_CREATE_HITER_POS:
		Skill_Create_Hiter(TimeDelta);

	case Skill_DIR_END:
		break;
	default:
		break;
	}
#endif	
}

void CSkill_TextureObj::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkill_TextureObj::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	//m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass;
	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif 
	return S_OK;
}

HRESULT CSkill_TextureObj::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	
	m_bLast_Initlize = true;
	return S_OK;
}

HRESULT CSkill_TextureObj::SetUp_Components()
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

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(0.4f, 0.4f, 0.4f);
	ColliderDesc.vRotation = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vCenter = _float3(0.0f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_TextureObj::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	_matrix		matTest = XMMatrixIdentity();
	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f,1.f,0.f,0.f), XMConvertToDegrees(-45.f));

	_vector		vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	_vector		vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector		vTranslation = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float3		vScale = m_pTransformCom->Get_Scaled();

	vRight =	 XMVector4Transform(vRight, RotationMatrix) *vScale.x ;
	vUp = XMVector4Transform(vUp, RotationMatrix) *vScale.y;
	vLook = XMVector4Transform(vLook, RotationMatrix) *vScale.z;
	vTranslation = XMVector4Transform(vTranslation, RotationMatrix);

	memcpy(&matTest.r[0], &vRight,sizeof(_vector));
	memcpy(&matTest.r[1], &vUp, sizeof(_vector));
	memcpy(&matTest.r[2], &vLook, sizeof(_vector));
	memcpy(&matTest.r[3], &vTranslation, sizeof(_vector));

	_float4x4	 Temp;
	XMStoreFloat4x4(&Temp, matTest);

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrixRotation",&Temp)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositon(), sizeof(_float4))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pVIBufferCom->Set_UV_RawValue(m_pShaderCom)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture",m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;

	return S_OK;
}

void CSkill_TextureObj::Skill_Charging(_double TimeDelta)
{
	if (!m_bShoot)
	{
		m_vScale.x -= (5.f ) *(_float)TimeDelta;
		m_vScale.y -= (5.f ) *(_float)TimeDelta;
		m_vScale.x -= (5.f) *(_float)TimeDelta;

		if (m_vScale.x <= 5.f)
		{
			m_vScale.x = 5.f;
			m_vScale.y = 5.f;
			m_vScale.z = 5.f;
		}
		m_pVIBufferCom->Set_Point_Instancing_Scale(m_vScale);	// 텍스쳐의 크기를 키우는것
		m_pTransformCom->Set_Scaled(m_vScale);

		/*	_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
			vPos.z += _float(1.5f*TimeDelta *iChange_Sign);
			vPos.y += _float(10.f*TimeDelta);
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));*/
	}
	else
	{
		m_vScale = _float3(5.f, 5.f, 5.f);

		m_pVIBufferCom->Set_Point_Instancing_Scale(m_vScale);	// 텍스쳐의 크기를 키우는것
		m_pTransformCom->Set_Scaled(m_vScale);

		m_pTransformCom->Chase_Rising(m_SkillDesc.pTraget->Get_State(CTransform::STATE_TRANSLATION), TimeDelta,true);
		

	}

}

void CSkill_TextureObj::Set_Cur_Pos()
{
#ifdef NOMODLES

#else
	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(m_SkillDesc.vAngle));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(m_SkillDesc.vPosition.x, m_SkillDesc.vPosition.y, m_SkillDesc.vPosition.z);
	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;

	_matrix			SocketMatrix = m_SkillDesc.pSocket->Get_OffsetMatrix() * m_SkillDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_SkillDesc.PivotMatrix);
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	SocketMatrix = SocketMatrix *	m_OriginMatrix* m_SkillDesc.ParentTransform->Get_WorldMatrix();
	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	m_pVIBufferCom->Set_Point_Instancing_Scale(m_SkillDesc.vScale);	// 텍스쳐의 크기를 키우는것
	m_pTransformCom->Set_Scaled(m_SkillDesc.vScale);				// 콜라이더의 크기를 키우는것임
	m_vScale = m_SkillDesc.vScale;
	m_bShoot = false;
#endif
}

void CSkill_TextureObj::Set_Cur_Pos_On_Hiter()
{
	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(m_SkillDesc.vAngle));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(m_SkillDesc.vPosition.x, m_SkillDesc.vPosition.y, m_SkillDesc.vPosition.z);
	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;

	_matrix			HiterMatrix = m_SkillDesc.pTraget->Get_WorldMatrix();
	HiterMatrix = m_OriginMatrix*HiterMatrix;

	XMStoreFloat4x4(&m_SocketMatrix, HiterMatrix);
	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	m_pVIBufferCom->Set_Point_Instancing_Scale(m_SkillDesc.vScale);	// 텍스쳐의 크기를 키우는것
	m_pTransformCom->Set_Scaled(m_SkillDesc.vScale);				// 콜라이더의 크기를 키우는것임
	m_vScale = m_SkillDesc.vScale;
	m_bShoot = false;

}

void CSkill_TextureObj::Set_Skill_Texture_Client(Skill_Texture_Client & Desc)
{
	m_fGOlfTimer = 0.f;
	m_fFixTimer = 0.25f;
	m_fIncrease = 6.f;
	m_fDecreate = 7.f;
	iChange_Sign = 1;

	memcpy(&m_SkillDesc, &Desc, sizeof(Skill_Texture_Client));
	Set_Cur_Pos();
}

void CSkill_TextureObj::Set_SKill_Texture_Client_Make_Hiter(Skill_Texture_Client & Desc)
{
	m_fGOlfTimer = 0.f;
	m_fFixTimer = 0.25f;
	m_fIncrease = 6.f;
	m_fDecreate = 7.f;
	iChange_Sign = 1;
	memcpy(&m_SkillDesc, &Desc, sizeof(Skill_Texture_Client));
	Set_Cur_Pos_On_Hiter();
}

void CSkill_TextureObj::Set_ChaserBone()
{
	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(m_SkillDesc.vAngle));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans = XMMatrixTranslation(m_SkillDesc.vPosition.x, m_SkillDesc.vPosition.y, m_SkillDesc.vPosition.z);
	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;
	_matrix			SocketMatrix = m_SkillDesc.pSocket->Get_OffsetMatrix() * m_SkillDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_SkillDesc.PivotMatrix);
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	SocketMatrix = SocketMatrix *	m_OriginMatrix* m_SkillDesc.ParentTransform->Get_WorldMatrix();
	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);
	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	m_pVIBufferCom->Set_Point_Instancing_Scale(m_SkillDesc.vScale);	// 텍스쳐의 크기를 키우는것
	m_pTransformCom->Set_Scaled(m_SkillDesc.vScale);
}

_bool CSkill_TextureObj::Hit_CountIncrease()
{	
	return m_bIncreateHit;
}

_bool CSkill_TextureObj::Is_HitReady()
{
	return m_bIncreateHit;
}

void CSkill_TextureObj::Skill_Golf_Shot(_double TimeDelta)
{
	/*계속 뼈를 따라 움직이는 코드*/
	//_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
	//matScale = XMMatrixScaling(1.f, 1.f, 1.f);
	//matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	//matRotY = XMMatrixRotationY(XMConvertToRadians(m_SkillDesc.vAngle));	//스트레이트는 90 , 골프 -180
	//matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	//matTrans = XMMatrixTranslation(m_SkillDesc.vPosition.x, m_SkillDesc.vPosition.y, m_SkillDesc.vPosition.z);
	//m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;
	//_matrix			SocketMatrix = m_SkillDesc.pSocket->Get_OffsetMatrix() * m_SkillDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_SkillDesc.PivotMatrix);
	//SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	//SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	//SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	//SocketMatrix = SocketMatrix *	m_OriginMatrix* m_SkillDesc.ParentTransform->Get_WorldMatrix();
	//XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);
	//m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);
	//m_pVIBufferCom->Set_Point_Instancing_Scale(m_SkillDesc.vScale);	// 텍스쳐의 크기를 키우는것
	//m_pTransformCom->Set_Scaled(m_SkillDesc.vScale);

	if (!m_bShoot && !m_bChaserBoneStop)
	{
		m_fGOlfTimer += (_float)TimeDelta;
		
		_float4 vPos;
		iChange_Sign *= -1;

		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		
		if (m_fGOlfTimer >= m_fFixTimer)
		{
			m_bSignChanger = !m_bSignChanger;
			m_fGOlfTimer = 0.f;
			m_fFixTimer -= 0.01f;
			m_fFixTimer = fmax(m_fFixTimer, 0.01f);
			m_fDecreate -= 0.5f;
			m_fIncrease -= 0.5f;
			
		}
		if (m_fDecreate >= 1.5f)
		{
			if (m_bSignChanger)
				vPos.y += _float(m_fIncrease * TimeDelta);
			else
				vPos.y -= _float(m_fDecreate * TimeDelta);
		}

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
	
	
	}
	else if (m_bShoot == true)
	{
		m_pTransformCom->Chase_Rising(m_SkillDesc.pTraget->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
		m_bChaserBoneStop = true;
	}
	else
		return;


}

void CSkill_TextureObj::Skill_Meteo(_double TimeDelta)
{
	if (!m_bShoot)
	{
		_float4 vPos;

		iChange_Sign *= -1;

		m_vScale.x += (m_SkillDesc.vScale.x *4.f) *(_float)TimeDelta;
		m_vScale.y += m_SkillDesc.vScale.y *4.f *(_float)TimeDelta;
		m_vScale.x += m_SkillDesc.vScale.z *4.f *(_float)TimeDelta;

		m_pVIBufferCom->Set_Point_Instancing_Scale(m_vScale);	// 텍스쳐의 크기를 키우는것
		m_pTransformCom->Set_Scaled(m_vScale);

		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
		vPos.z += _float(1.5f*TimeDelta *iChange_Sign);
		vPos.y += _float(10.f*TimeDelta);
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
	}
	else
	{
		m_pTransformCom->Chase(m_SkillDesc.pTraget->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
		m_bChaserBoneStop = true;
	}
}

void CSkill_TextureObj::Skill_Create_Hiter(_double TimeDelta)
{

}

CSkill_TextureObj * CSkill_TextureObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkill_TextureObj*		pInstance = new CSkill_TextureObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkill_TextureObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkill_TextureObj::Clone(void * pArg)
{
	CSkill_TextureObj*		pInstance = new CSkill_TextureObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkill_TextureObj");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_TextureObj::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}