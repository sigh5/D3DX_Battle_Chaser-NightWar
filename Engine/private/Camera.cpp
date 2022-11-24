#include "..\public\Camera.h"
#include "GameInstance.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
	
{
	Safe_AddRef(m_pPipeLine);

}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void * pArg)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	ZeroMemory(&m_CameraDesc, sizeof(CAMERADESC));

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(m_CameraDesc));

	if (FAILED(__super::Initialize(&m_CameraDesc)))
		return E_FAIL;

	/* ī�޶�(���ӿ�����Ʈ)�� ����ִ� Ʈ�������� ī�޶��� ���¸� ����ȭ�ϳ�. */
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));
	
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CCamera::Last_Initialize()
{
	return S_OK;
}

void CCamera::Tick(_double TimeDelta)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), 1280 / (_float)720, 0.2f, 300.f));



}

void CCamera::Late_Tick(_double TimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}



void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);

}
