#include "stdafx.h"
#include "..\public\Light_Pos.h"
#include "GameObject.h"
#include "GameInstance.h"


CLight_Pos::CLight_Pos(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CLight_Pos::CLight_Pos(const CLight_Pos & rhs)
	:CGameObject(rhs)
{
}

HRESULT CLight_Pos::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLight_Pos::Initialize(void * pArg)
{	
	m_ObjectName = TEXT("Light_Pos");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_float4x4   WorldMatrix, m_LightProjMatrix,m_WorldMat;
	_matrix		m_matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

	matScale = XMMatrixScaling(1.f, 1.f, 1.f);

	matRotX = XMMatrixRotationX(XMConvertToRadians(-100.f));
	matRotY = XMMatrixRotationY(XMConvertToRadians(0.f));
	matRotZ = XMMatrixRotationZ(XMConvertToRadians(-110.f));
	matTrans = XMMatrixTranslation(-144.f, 130.f, 76.f);

	m_matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;
	XMStoreFloat4x4(&m_WorldMat, m_matWorld);
	m_pTransformCom->Set_WorldMatrix(m_WorldMat);


	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix_Inverse());

	_matrix		LightProjMatrix;
	LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), _float(1280) / _float(720), 0.2f, 300.f);
	XMStoreFloat4x4(&m_LightProjMatrix, LightProjMatrix);


	CGameInstance::GetInstance()->Set_LightView_Matrirx(TEXT("Test"), WorldMatrix);
	CGameInstance::GetInstance()->Set_LightProj_Matrirx(TEXT("Test"), m_LightProjMatrix);


	return S_OK;
}

void CLight_Pos::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	//m_pTransformCom->LookAt(XMVectorSet(0.f, -1.f, 0.f, 0.f));

	/*For Tool*/
	//_float4x4 WorldMatrix, m_LightProjMatrix;
	//XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix_Inverse());

	//_matrix		LightProjMatrix;
	//LightProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), _float(1280) / _float(720), 0.2f, 300.f);
	//XMStoreFloat4x4(&m_LightProjMatrix, LightProjMatrix);


	//CGameInstance::GetInstance()->Set_LightView_Matrirx(TEXT("Test"), WorldMatrix);
	//CGameInstance::GetInstance()->Set_LightProj_Matrirx(TEXT("Test"), m_LightProjMatrix);

	/*~ For Tool*/

}

void CLight_Pos::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_INVENTORY, this);
}

HRESULT CLight_Pos::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLight_Pos::Last_Initialize()
{
	return S_OK;
}

HRESULT CLight_Pos::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCubeTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLight_Pos::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLight_Pos * CLight_Pos::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLight_Pos*		pInstance = new CLight_Pos(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLight_Pos::Clone(void * pArg)
{
	CLight_Pos*		pInstance = new CLight_Pos(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSky");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight_Pos::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}

