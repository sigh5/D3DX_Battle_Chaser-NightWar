#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
{
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject || 
		eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{

	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_LightAcc()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (nullptr != m_pTarget_Manager)
	{
		m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"));
	}
#endif

	return S_OK;
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

//	D3D11_VIEWPORT			ViewportDesc;
//	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);
//
//	_uint			iNumViewports = 1;
//
//	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
//	/* 렌터타겟들을 생성. */
//
//	/* For.Target_Diffuse */
//	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(1.f, 0.0f, 0.0f, 1.f))))
//		return E_FAIL;
//
//	/* For.Target_Normal */
//	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
//		return E_FAIL;
//
//	/* For.Target_Shade */
//	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
//		return E_FAIL;
//
//	/* For.MRT_Deferred */ /* 디퍼드 렌더링(빛)을 수행하기위해 필요한 데이터들을 저장한 렌더타겟들. */
//	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
//		return E_FAIL;
//	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
//		return E_FAIL;
//
//	/* For.MRT_LightAcc */ /* 빛 연산의 결과를 저장할 렌더타겟들.  */
//	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
//		return E_FAIL;
//
//#ifdef _DEBUG
//	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.0f, 100.f, 200.f, 200.f)))
//		return E_FAIL;
//	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 100.0f, 300.f, 200.f, 200.f)))
//		return E_FAIL;
//	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 300.0f, 100.f, 200.f, 200.f)))
//		return E_FAIL;
//#endif

	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	//if (nullptr == m_pTarget_Manager)
	//	return E_FAIL;

	///* Diffuse + Normal */
	//if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"))))
	//	return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONALPHABLEND].clear();

	//if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Deferred"))))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CRenderer * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}
void CRenderer::Free()
{
	__super::Free();

	Safe_Release(m_pTarget_Manager);

}
