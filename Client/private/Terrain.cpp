#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"
#include "Environment_Object.h"
#include "ToolManager.h"

#include "Cell.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	m_ObjectName = TEXT("Terrain");

	if (FAILED(__super::Initialize(&pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Ready_FilterBuffer();
	return S_OK;
}

HRESULT CTerrain::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	m_ProtoName = TEXT("Prototype_GameObject_Terrain");

	/*For.Imgui */
	//m_pNavigationCom->Set_SaveSort_NavigatorVector(&CToolManager::GetInstance()->m_SaveSort_PickingPos);



	m_bLast_Initlize = true;


	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	Last_Initialize();

	ImGui::RadioButton("NaviWireFrame", &m_iNaviShaderPass, 0);
	ImGui::RadioButton("NaviSolid", &m_iNaviShaderPass, 1);


	__super::Tick(TimeDelta);
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Create_Object();

	Ready_BufferLock_UnLock();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

#ifdef _DEBUG
	m_pNavigationCom->Render(m_iNaviShaderPass);
#endif

	return S_OK;
}

_float4 CTerrain::Get_Position() const
{
	return m_pVIBufferCom->PickingTerrain(g_hWnd, m_pTransformCom);
}

_bool CTerrain::Piciking_GameObject()
{
	
	return false;
}

HRESULT CTerrain::Ready_FilterBuffer()
{
	m_pPixel = new _ulong[128 * 128];

	for (_uint i = 0; i < 128; ++i)
	{
		for (_uint j = 0; j < 128; ++j)
		{
			_uint iIndex = i * 128 + j;
			m_pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	return S_OK;

}

HRESULT CTerrain::Ready_BufferLock_UnLock()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CToolManager* ptoolManager = GET_INSTANCE(CToolManager);

	HRESULT hr = 0;
	_float4 iIndex = _float4(1.f, 1.f, 1.f,1.f);

	if (ImGui::IsMouseDragging(0) && ptoolManager->Get_RadioButtonValue() == 1)
	{
		if (m_pVIBufferCom->PickingRetrunIndex(g_hWnd, m_pTransformCom, iIndex))
		{
			m_FilterIndexSet.insert(_ulong(iIndex.x));
			m_FilterIndexSet.insert(_ulong(iIndex.y));
			m_FilterIndexSet.insert(_ulong(iIndex.z));
			m_FilterIndexSet.insert(_ulong(iIndex.w));
		}

		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = 128;		// 2의 배수로 맞춰야됀다.
		TextureDesc.Height = 128;		// 2의 배수로 맞춰야됀다.
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
			return E_FAIL;

		for (auto iter = m_FilterIndexSet.begin(); iter != m_FilterIndexSet.end(); ++iter)
		{
			m_pPixel[(*iter)] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}

		D3D11_MAPPED_SUBRESOURCE		SubResource;
		ZeroMemory(&SubResource, sizeof SubResource);

		m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);  //DX_9 Lock ==Map

		memcpy(SubResource.pData, m_pPixel, (sizeof(_ulong) *TextureDesc.Width * TextureDesc.Height));

		m_pContext->Unmap(pTexture2D, 0);


		Safe_Release(m_pTextureCom[TYPE_FILTER]);
		Remove_component(TEXT("Com_Filter"));

		hr = DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures2D/Filter.dds"));

		Safe_Release(pTexture2D);

		pGameInstance->Remove_ProtoComponent(pGameInstance->GetCurLevelIdx(), TEXT("Prototype_Component_Texture_Test_Filter"));

		hr = (pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Test_Filter"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Filter.dds"), CTexture::TYPE_FILTER)));

		hr = __super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Test_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER]);
	}


	RELEASE_INSTANCE(CToolManager);
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CTerrain::Create_Object()
{
	if (!m_bCreateObject)
		return;

	_float4 vPos;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
	{
		if (m_pVIBufferCom->PickingBuffer(g_hWnd, m_pTransformCom, &vPos))
		{
			CGameObject* pGameObject = nullptr;
			CEnvironment_Object::ENVIRONMENTDESC Desc;
			ZeroMemory(&Desc, sizeof(CEnvironment_Object::ENVIRONMENTDESC));

			lstrcpy(Desc.m_pModelTag, m_ObjData.szModelName);
			pGameInstance->Clone_GameObject_UseImgui(pGameInstance->GetCurLevelIdx(), m_ObjData.szLayerName,m_ObjData.szProtoName, &pGameObject,&Desc);
			if (pGameObject == nullptr)
				MSG_BOX(" CTerrain::Piciking_GameObject");

			_tchar	szIndex[MAX_PATH] = L"";
			_itow_s(m_iObjNameNumber++, szIndex, 10);
			lstrcat(m_ObjData.szObjectName, szIndex);
			
			pGameObject->Set_ObjectName(m_ObjData.szObjectName);
			pGameObject->Set_ProtoName(m_ObjData.szProtoName);
			pGameObject->Get_Transform()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&vPos));
			
		
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	
}

void CTerrain::Get_PickingPos()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float4	vPickPos;

	if (pGameInstance->Mouse_Down(CInput_Device::DIM_LB))
	{
		if (m_pVIBufferCom->PickingNavi(g_hWnd, m_pTransformCom, vPickPos))
		{
			XMStoreFloat4(&vPickPos, XMVector3TransformCoord(XMLoadFloat4(&vPickPos), m_pTransformCom->Get_WorldMatrix()));
			Create_NaviMap(vPickPos);
		}
	}


	RELEASE_INSTANCE(CGameInstance);


}

void CTerrain::DeleteNavi()
{
	m_pNavigationCom->Delete_Navi();

	size_t size_Num = m_vecAllNavi.size()-3;
	
	_uint i = 0;

	m_vecAllNavi.erase(m_vecAllNavi.end()-1);
	m_vecAllNavi.erase(m_vecAllNavi.end()-1);
	m_vecAllNavi.erase(m_vecAllNavi.end()-1);
}

void CTerrain::Create_NaviMap(_float4 vPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CToolManager* ptoolManager = GET_INSTANCE(CToolManager);

	for (auto PrevPos : m_vecAllNavi)
	{
		_vector vPrevPos = XMVectorSetW(XMLoadFloat3(&PrevPos), 1.f);
		_vector		vDir = XMLoadFloat4(&vPos) - vPrevPos;
		_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

		if (fDistance < 2.f)
		{
			XMStoreFloat4(&vPos, vPrevPos);
			break;
		}
	}

	_float3		vRealPos;
	vRealPos.x = vPos.x;
	vRealPos.y = 0;			// 일단은 0
	vRealPos.z = vPos.z;
	m_vNaviPosVec.push_back(vRealPos);


	if (m_vNaviPosVec.size() == 3)
	{
		Create_Cells();
	}


	RELEASE_INSTANCE(CToolManager);
	RELEASE_INSTANCE(CGameInstance);
}

void CTerrain::Create_Cells()
{
	if (FLOAT_EQ(m_vNaviPosVec[0].z, m_vNaviPosVec[1].z) && FLOAT_EQ(m_vNaviPosVec[1].z, m_vNaviPosVec[2].z))
	{
		m_vNaviPosVec.clear();		// 있을수 없는 경우
		return;
	}
	else   // 3개 다 다를경우
	{
		if ((m_vNaviPosVec[0].z >= m_vNaviPosVec[1].z) && (m_vNaviPosVec[0].z >= m_vNaviPosVec[2].z))
		{
			m_vecSortNavi.push_back(m_vNaviPosVec[0]);

			if (m_vNaviPosVec[1].x >= m_vNaviPosVec[2].x)
			{
				m_vecSortNavi.push_back(m_vNaviPosVec[1]);
				m_vecSortNavi.push_back(m_vNaviPosVec[2]);
			}
			else
			{
				m_vecSortNavi.push_back(m_vNaviPosVec[2]);
				m_vecSortNavi.push_back(m_vNaviPosVec[1]);
			}

		}
		else if ((m_vNaviPosVec[1].z > m_vNaviPosVec[0].z) && (m_vNaviPosVec[1].z >= m_vNaviPosVec[2].z))
		{
			m_vecSortNavi.push_back(m_vNaviPosVec[1]);

			if (m_vNaviPosVec[0].x >= m_vNaviPosVec[2].x)
			{
				m_vecSortNavi.push_back(m_vNaviPosVec[0]);
				m_vecSortNavi.push_back(m_vNaviPosVec[2]);
			}
			else
			{
				m_vecSortNavi.push_back(m_vNaviPosVec[2]);
				m_vecSortNavi.push_back(m_vNaviPosVec[0]);
			}
		}

		else if ((m_vNaviPosVec[2].z > m_vNaviPosVec[0].z) && (m_vNaviPosVec[2].z > m_vNaviPosVec[1].z))
		{
			m_vecSortNavi.push_back(m_vNaviPosVec[2]);

			if (m_vNaviPosVec[0].x >= m_vNaviPosVec[1].x)
			{
				m_vecSortNavi.push_back(m_vNaviPosVec[0]);
				m_vecSortNavi.push_back(m_vNaviPosVec[1]);
			}
			else
			{
				m_vecSortNavi.push_back(m_vNaviPosVec[1]);
				m_vecSortNavi.push_back(m_vNaviPosVec[0]);
			}
		}
	}

	m_vNaviPosVec.clear();


	_float3 Temp[3] = { m_vecSortNavi[0],m_vecSortNavi[1],m_vecSortNavi[2] };

	m_pNavigationCom->AddCell(Temp);

	for (_uint i = 0; i < 3; ++i)
	{
		m_vecAllNavi.push_back(m_vecSortNavi[i]);
	}
	
	m_vecSortNavi.clear();

}


void CTerrain::Set_MapObject(Create_OBJECTDESC& pArg)
{
	m_bCreateObject = true;
	
	m_ObjData.szLayerName = pArg.szLayerName;
	m_ObjData.szProtoName = pArg.szProtoName;
	m_ObjData.szTextureName = pArg.szTextureName;
	m_ObjData.szModelName = pArg.szModelName;
	m_ObjData.szObjectName = pArg.szObjectName;
	m_ObjData.iShaderPass = pArg.iShaderPass;
	
	
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNormalTex_Phong"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Brush*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Brush"),
		(CComponent**)&m_pTextureCom[TYPE_BRUSH])))
		return E_FAIL;

	/* For.Com_Filter */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Test_Filter"), TEXT("Com_Filter"),
		(CComponent**)&m_pTextureCom[TYPE_FILTER])))
		return E_FAIL;

	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
		(CComponent**)&m_pNavigationCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
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

	// For_Lights
	const	LIGHTDESC*	pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositon(), sizeof(_float4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	_uint	m_iDiffuseTextureIndex = 1;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iDiffuseTextureIndex", &m_iDiffuseTextureIndex, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture")))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResource(m_pShaderCom, "g_FilterTexture")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &_float4(15.f, 0.f, 15.f, 1.f), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBackGround");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pPixel);
	m_FilterIndexSet.clear();

	for (auto& pName : m_NameVector)
		Safe_Delete_Array(pName);
	m_NameVector.clear();

	for (auto& pTextureCom : m_pTextureCom)
		Safe_Release(pTextureCom);

	
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
