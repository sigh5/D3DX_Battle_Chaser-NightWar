#include "stdafx.h"
#include "..\public\Skill_Object.h"
#include "GameInstance.h"
#include "Bone.h"


CSkill_Object::CSkill_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CHitBoxObject(pDevice,pContext)
{
}

CSkill_Object::CSkill_Object(const CSkill_Object & rhs)
	: CHitBoxObject(rhs)
{
}

HRESULT CSkill_Object::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill_Object::Initialize(void * pArg)
{
	m_ObjectName = TEXT("FireBall");	//Test


	if (nullptr != pArg)
		memcpy(&m_SkillDesc, pArg, sizeof(m_SkillDesc));

	tagHitBoxObject Desc;
	ZeroMemory(&Desc, sizeof(Desc));

	Desc.TransformDesc.fSpeedPerSec = 70.f;
	Desc.TransformDesc.fRotationPerSec = 90.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

	matScale = XMMatrixScaling(1.f, 1.f, 1.f);

	matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(m_SkillDesc.vAngle));	//스트레이트는 90 , 골프 -180
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
	matTrans =	XMMatrixTranslation(m_SkillDesc.vPosition.x, m_SkillDesc.vPosition.y, m_SkillDesc.vPosition.z);

	m_OriginMatrix = matScale * matRotX * matRotY * matRotZ * matTrans;
	

	_matrix			SocketMatrix = m_SkillDesc.pSocket->Get_OffsetMatrix() * m_SkillDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_SkillDesc.PivotMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix = SocketMatrix *	m_OriginMatrix* m_SkillDesc.ParentTransform->Get_WorldMatrix();


	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);



	return S_OK;
}

void CSkill_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	Skill_DIR eDir = m_SkillDesc.eDir;

	switch (eDir)
	{
	case Skill_DIR_straight:
		m_pTransformCom->Chase(m_SkillDesc.pTraget->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
		break;
	case Skill_DIR_LISING:
		Rising_Move_Start(TimeDelta);
		break;
	case Skill_DIR_DOWN:
		
		break;
	case Skill_DIR_ScaleUP_DOWN:
		ScaleUP_Down_Move_Start(TimeDelta);
		break;
	case Skill_DIR_END:
		break;
	default:
		break;
	}
	
}

void CSkill_Object::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
	}
}

HRESULT CSkill_Object::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	 
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Render(m_pShaderCom, i, 0);
	}



	return S_OK;
}

HRESULT CSkill_Object::Last_Initialize()
{
	return S_OK;
}

void CSkill_Object::Rising_Move_Start(_double TimeDelta)
{
	if (!m_bShoot)
	{
		_matrix			SocketMatrix = m_SkillDesc.pSocket->Get_OffsetMatrix() * m_SkillDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_SkillDesc.PivotMatrix);

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		SocketMatrix = SocketMatrix *	m_OriginMatrix* m_SkillDesc.ParentTransform->Get_WorldMatrix();

		XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

		m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);

	
	}
	else
	{
		m_pTransformCom->Chase_Rising(m_SkillDesc.pTraget->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
	}
}

void CSkill_Object::ScaleUP_Down_Move_Start(_double TimeDelta)
{
	if (!m_bShoot)
	{
		_matrix			SocketMatrix = m_SkillDesc.pSocket->Get_OffsetMatrix() * m_SkillDesc.pSocket->Get_CombindMatrix() * XMLoadFloat4x4(&m_SkillDesc.PivotMatrix);

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);


		/*_matrix			ParentMatrix = ;

		ParentMatrix.r[0] = XMVector3Normalize(ParentMatrix.r[0]);
		ParentMatrix.r[1] = XMVector3Normalize(ParentMatrix.r[1]);
		ParentMatrix.r[2] = XMVector3Normalize(ParentMatrix.r[2]);*/
		// 나중에 수정하자
		static _float Dest =20.f;
		Dest += 20.f + 200.f *(_float)TimeDelta;

		static _float yPos = 5.f;
		yPos += 5.f*(_float)TimeDelta;

		_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;
		matScale = XMMatrixScaling(Dest, Dest, Dest);

		matRotX = XMMatrixRotationX(XMConvertToRadians(0.f));
		matRotY = XMMatrixRotationY(XMConvertToRadians(m_SkillDesc.vAngle));	//스트레이트는 90 , 골프 -180
		matRotZ = XMMatrixRotationZ(XMConvertToRadians(0.f));
		matTrans = XMMatrixTranslation(0.f, yPos, 0.f);



		SocketMatrix = SocketMatrix *	m_OriginMatrix* m_SkillDesc.ParentTransform->Get_WorldMatrix();

		XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

		m_pTransformCom->Set_WorldMatrix(m_SocketMatrix);


	}
	else
	{
		m_pTransformCom->Chase_Rising(m_SkillDesc.pTraget->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
	}
}

HRESULT CSkill_Object::SetUp_Components()
{
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_SkillDesc.pModelTag, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
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

	//TEXT("Prototype_Component_FireBall")

	return S_OK;
}

HRESULT CSkill_Object::SetUp_ShaderResources()
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

CSkill_Object * CSkill_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkill_Object*		pInstance = new CSkill_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkill_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkill_Object::Clone(void * pArg)
{
	CSkill_Object*		pInstance = new CSkill_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkill_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkill_Object::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}