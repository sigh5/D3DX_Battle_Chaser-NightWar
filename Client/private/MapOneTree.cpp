#include "stdafx.h"
#include "..\public\MapOneTree.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "MapTile.h"

CMapOneTree::CMapOneTree(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEnvironment_Object(pDevice, pContext)
{
}

CMapOneTree::CMapOneTree(const CMapOneTree & rhs)
	: CEnvironment_Object(rhs)
{
}

HRESULT CMapOneTree::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapOneTree::Initialize(void * pArg)
{
	/*m_ObjectName = TEXT("Map_Tree");
	m_ProtoName = TEXT("Prototype_GameObject_2D_MapOne");*/
	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	
	


	
	return S_OK;
}

HRESULT CMapOneTree::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;


	if (!lstrcmp(m_ObjectName, TEXT("Tree_Cluster_Birch_A")))
	{
		m_pModelCom->Load_TreePos(TEXT("Tree_Cluster_Birch_A"));
	}

	else if (!lstrcmp(m_ObjectName, TEXT("Tree_Cluster_Mid_A")))
	{
		m_pModelCom->Load_TreePos(TEXT("Tree_Cluster_Mid_A"));
	}

	else if (!lstrcmp(m_ObjectName, TEXT("Map_Tree_Cluster_Back_A")))
	{
		m_pModelCom->Load_TreePos(TEXT("Map_Tree_Cluster_Back_A"));
	}
	else if (!lstrcmp(m_ObjectName, TEXT("Map_Tree_Cluster_Back_C")))
	{
		m_pModelCom->Load_TreePos(TEXT("Map_Tree_Cluster_Back_C"));
	}
	else if (!lstrcmp(m_ObjectName, TEXT("Map_Tree_Dead_Bush_A")))
	{
		m_pModelCom->Load_TreePos(TEXT("Map_Tree_Dead_Bush_A"));
	}
	else if (!lstrcmp(m_ObjectName, TEXT("Map_Tree_Cluster_Birch_B")))
	{
		m_pModelCom->Load_TreePos(TEXT("Map_Tree_Cluster_Birch_B"));
	}

	m_bLast_Initlize = true;

	return S_OK;
}

void CMapOneTree::Tick(_double TimeDelta)
{
	Last_Initialize();
	__super::Tick(TimeDelta);
	
	//static char Name[MAX_PATH] = "";
	//WideCharToMultiByte(CP_ACP, 0, m_ObjectName, MAX_PATH, Name, MAX_PATH, NULL, NULL);

	//if (lstrcmp(m_ObjectName, TEXT("Tree_Cluster_Birch_A")) && lstrcmp(m_ObjectName, TEXT("Tree_Cluster_Mid_A"))
	//	&& lstrcmp(m_ObjectName, TEXT("Tree_Cluster_Front_A")) && lstrcmp(m_ObjectName, TEXT("Map_Tree_Cluster_Back_A"))
	//	&& lstrcmp(m_ObjectName, TEXT("Map_Tree_Cluster_Back_C")) && lstrcmp(m_ObjectName, TEXT("Map_Tree_Dead_Bush_A")))
	//{
	//	ImGui::Text("%s", Name);

	//	if (m_iCreateRadioButton == 1)
	//	{
	//		ImGui::InputText("Text", szName, MAX_PATH);

	//		Save_TreePos();
	//		Load_TreePos();
	//	}


	//	ImGui::RadioButton("None_Create_PickPos", &m_iCreateRadioButton, 0);
	//	ImGui::RadioButton("Create_PickPos", &m_iCreateRadioButton, 1);

	//	Picking_pos();
	//}

}

void CMapOneTree::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CMapOneTree::Render()
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

_bool CMapOneTree::Piciking_GameObject()
{
	return false;
}

void CMapOneTree::Picking_pos()
{
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_iCreateRadioButton == 1)
	{
		CMapTile* pTile	=	dynamic_cast<CMapTile*>(pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, LAYER_ENVIRONMENT, TEXT("MapTile_MapOne")));
	
		if (pTile == nullptr)
			assert(!"ERror");
		_float4 vPos;
		if (pTile->Tree_Instancing(vPos))
		{
			static _uint i = 1;
			m_pModelCom->Add_IncreasePosition(vPos, ++i);
		}

	}


	RELEASE_INSTANCE(CGameInstance);

	
}

void CMapOneTree::Save_TreePos()
{
	if (m_iCreateRadioButton == 0)
		return;
	


	if (ImGui::Button("TreePos Save"))
	{
		_tchar szFileName[MAX_PATH] = TEXT("");

		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szName, MAX_PATH, szFileName, 256);

		m_pModelCom->Save_TreePos(szFileName);
	}
}

void CMapOneTree::Load_TreePos()
{
	if (m_iCreateRadioButton == 0)
		return;

	if (ImGui::Button("TreePos Load"))
	{
		_tchar szFileName[MAX_PATH] = TEXT("");
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szName, MAX_PATH, szFileName, 256);
		m_pModelCom->Load_TreePos(szFileName);
	}
}

HRESULT CMapOneTree::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel_Instance"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_EnviromentDesc.m_pModelTag, TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CMapOneTree::SetUp_ShaderResources()
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

CMapOneTree * CMapOneTree::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapOneTree*		pInstance = new CMapOneTree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapOneTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMapOneTree::Clone(void * pArg)
{
	CMapOneTree*		pInstance = new CMapOneTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapOneTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapOneTree::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
