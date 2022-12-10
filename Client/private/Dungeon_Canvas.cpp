#include "stdafx.h"
#include "..\public\Dungeon_Canvas.h"

#include "GameInstance.h"
#include "Hero_Knolan.h"
#include "Hero_Calibretto.h"
#include "Hero_Garrison.h"

CDungeon_Canvas::CDungeon_Canvas(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCanvas(pDevice, pContext)
{
}

CDungeon_Canvas::CDungeon_Canvas(const CDungeon_Canvas & rhs)
	: CCanvas(rhs)
{
}

HRESULT CDungeon_Canvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CDungeon_Canvas::Initialize(void * pArg)
{
	CCanvas::CANVASDESC		CanvasDesc;
	ZeroMemory(&CanvasDesc, sizeof(CanvasDesc));

	if (nullptr != pArg)
		memcpy(&CanvasDesc, pArg, sizeof(CanvasDesc));

	if (FAILED(__super::Initialize(&CanvasDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;


	m_fSizeX = (_float)g_iWinSizeX / 11;
	m_fSizeY = (_float)g_iWinSizeY * 1.1f;
	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX - m_fSizeX * 0.5f - (585.f), -m_fY + m_fSizeY * 0.5f, 0.1f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	if (FAILED(CUI::SetUp_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeon_Canvas::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	CGameInstance* pInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = pInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Hero_Gully"));
	dynamic_cast<CHero_Knolan*>(pGameObject)->m_Hero_DungeonUIDelegeter.bind(this, &CDungeon_Canvas::Change_UITexture);

	pGameObject = pInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Hero_Alumon"));
	dynamic_cast<CHero_Garrison*>(pGameObject)->m_Hero_DungeonUIDelegeter.bind(this, &CDungeon_Canvas::Change_UITexture);

	pGameObject = pInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Hero_Calibretto"));
	dynamic_cast<CHero_Calibretto*>(pGameObject)->m_Hero_DungeonUIDelegeter.bind(this, &CDungeon_Canvas::Change_UITexture);

	m_bLast_Initlize = true;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CDungeon_Canvas::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);


}

void CDungeon_Canvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

}

HRESULT CDungeon_Canvas::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	CUI::Begin_UI();

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();

	CUI::End_UI();

	return S_OK;
}

void CDungeon_Canvas::Change_Texture(_uint iLevel, const wstring & NewComPonentTag)
{
	Safe_Release(m_pTextureCom);
	Remove_component(TEXT("Com_Texture"));

	lstrcpy(m_UIDesc.m_pTextureTag, NewComPonentTag.c_str());

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(iLevel, m_UIDesc.m_pTextureTag, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		assert("CTurnCharcterUI Change_Texture");

}

HRESULT CDungeon_Canvas::SetUp_Components()
{
	/*For.Com_Renderer */
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

	_tchar* pTemp 	=	m_CanvasDesc.m_pTextureTag;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, pTemp, TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDungeon_Canvas::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTH))))
		return E_FAIL;


	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_pTextureCom->Get_SelectTextureIndex())))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CDungeon_Canvas::Change_UITexture(HIGHLIGHT_UIDESC Desc)
{
	for (auto& pChildUI : m_ChildrenVec)
	{
		if (!lstrcmp(pChildUI->Get_ObjectName(), Desc.szObjectName))
		{
			pChildUI->Set_HighRightUIDesc(Desc);
			break;
		}


	}
}





CDungeon_Canvas * CDungeon_Canvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDungeon_Canvas*		pInstance = new CDungeon_Canvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTurnUICanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDungeon_Canvas::Clone(void * pArg)
{
	CDungeon_Canvas*		pInstance = new CDungeon_Canvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTurnUICanvas");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDungeon_Canvas::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

}


