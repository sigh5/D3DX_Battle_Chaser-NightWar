#include "stdafx.h"
#include "..\public\MapTile.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "MapOneTree.h"



CMapTile::CMapTile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnvironment_Object(pDevice, pContext)
{
}

CMapTile::CMapTile(const CMapTile & rhs)
	: CEnvironment_Object(rhs)
{
}

HRESULT CMapTile::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapTile::Initialize(void * pArg)
{
	m_ObjectName = TEXT("MapTile_MapOne");
	m_ProtoName = TEXT("Prototype_GameObject_MapTile");
	
	CEnvironment_Object::ENVIRONMENTDESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	lstrcpy(Desc.m_pTextureTag, TEXT("Prototype_Component_Texture_MapOneTile_Diffuse"));
	
	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Ready_FilterBuffer();
	return S_OK;
}

HRESULT CMapTile::Last_Initialize()
{
	if (m_bLast_Initlize)
		return S_OK;

	//dm_pTransformCom->Rotation(XMVectorSet(1.f,0.f,0.f,0.f),XMConvertToRadians(90.f));
	m_pTransformCom->Set_Scaled(_float3(0.11f, 0.11f, 0.11f));

	m_vBrushPos = _float4(0.f, 0.f, 0.f,1.f);

	m_pVIBufferCom->initialize_World(m_pTransformCom);
	m_bLast_Initlize = true;
	return S_OK;
}

void CMapTile::Tick(_double TimeDelta)
{
	Last_Initialize();

	if(ImGui::Button("Pixel Save"))
	{
		_tchar szName[MAX_PATH] = TEXT("../../Data/PixelData_Ground.dat") ;

		HANDLE      hFile = CreateFile(szName,
			GENERIC_WRITE,
			NULL,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	
		if (INVALID_HANDLE_VALUE == hFile)
		{
			return;
		}
		DWORD   dwByte = 0;
	
		WriteFile(hFile, m_pPixel, sizeof(_ulong) *TextureDesc.Width * TextureDesc.Height, &dwByte, nullptr);
		CloseHandle(hFile);
		MSG_BOX("Save_Complete");
	}


	if (ImGui::Button("Pixel LOAD"))
	{
		_tchar szName[MAX_PATH] = TEXT("../../Data/PixelData_Ground.dat");

		HANDLE      hFile = CreateFile(szName,
			GENERIC_READ,
			NULL,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (INVALID_HANDLE_VALUE == hFile)
		{
			return;
		}
		DWORD   dwByte = 0;

		ReadFile(hFile, m_pPixel, sizeof(_ulong) *TextureDesc.Width * TextureDesc.Height, &dwByte, nullptr);
		
		for (_uint i = 0; i < 512; ++i)
		{
			for (_uint j = 0; j < 512; ++j)
			{
				_uint iIndex = i * 512 + j;

				if (m_pPixel[iIndex] == D3DCOLOR_ARGB(255, 0, 0, 0))
				{
					m_FilterIndexSet.insert(_ulong(iIndex));
				}
			}
		}

		TextureDesc.Width = 512;		// 2의 배수로 맞춰야됀다.
		TextureDesc.Height = 512;		// 2의 배수로 맞춰야됀다.
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DYNAMIC;	// 동적으로 만들어야지 락 언락가능
		TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU는 동적할때 무조건
		TextureDesc.MiscFlags = 0;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pTexture2D)))
			

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

		 DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures2D/Filter_2.dds"));

		Safe_Release(pTexture2D);

		pGameInstance->Remove_ProtoComponent(pGameInstance->GetCurLevelIdx(), TEXT("Prototype_Component_Texture_MapOneTile_Filter"));

		(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Filter"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Filter_%d.dds"), CTexture::TYPE_FILTER, 3)));

		__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER]);

		RELEASE_INSTANCE(CGameInstance);

		
		
		CloseHandle(hFile);
		MSG_BOX("Load_Complete");
	}


	static float VBrushPos[4] = { 0.f,0.f,0.f,1.f };
	ImGui::InputFloat4("BrushPos", VBrushPos);

	m_vBrushPos.x = VBrushPos[0];
	m_vBrushPos.y = VBrushPos[1];
	m_vBrushPos.z = VBrushPos[2];
	m_vBrushPos.w = VBrushPos[3];


	Test_Instancing();

	__super::Tick(TimeDelta);
}

void CMapTile::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);




	Ready_BufferLock_UnLock();
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CMapTile::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	m_pVIBufferCom->Render();



#ifdef _DEBUG
	m_pNavigationCom->Render(0);
#endif

	return S_OK;
}

HRESULT CMapTile::Ready_FilterBuffer()
{
	m_pPixel = new _ulong[512 * 512];

	for (_uint i = 0; i < 512; ++i)
	{
		for (_uint j = 0; j < 512; ++j)
		{
			_uint iIndex = i * 512 + j;
			m_pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	return S_OK;
}

HRESULT CMapTile::Ready_BufferLock_UnLock()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CToolManager* ptoolManager = GET_INSTANCE(CToolManager);

	HRESULT hr = 0;
	_float4 iIndex = _float4(1.f, 1.f, 1.f, 1.f);

	if (ImGui::IsMouseDragging(0) && ptoolManager->Get_RadioButtonValue() == 1)
	{
		if (m_pVIBufferCom->PickingRetrunIndex_scale(g_hWnd, m_pTransformCom, iIndex))
		{
			m_FilterIndexSet.insert(_ulong(iIndex.x));
			m_FilterIndexSet.insert(_ulong(iIndex.y));
			m_FilterIndexSet.insert(_ulong(iIndex.z));
			m_FilterIndexSet.insert(_ulong(iIndex.w));
		}

		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = 512;		// 2의 배수로 맞춰야됀다.
		TextureDesc.Height = 512;		// 2의 배수로 맞춰야됀다.
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

		hr = DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures2D/Filter_2.dds"));

		Safe_Release(pTexture2D);

		pGameInstance->Remove_ProtoComponent(pGameInstance->GetCurLevelIdx(), TEXT("Prototype_Component_Texture_MapOneTile_Filter"));

		hr = (pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Filter"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/Filter_%d.dds"), CTexture::TYPE_FILTER,3)));

		hr = __super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Filter"), TEXT("Com_Filter"),
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

void CMapTile::Test_Instancing()
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject * pTree = pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, LAYER_ENVIRONMENT, TEXT("Map_Tree"));

	if (ImGui::IsMouseClicked(0))
	{
		_float4 vRightPos;
		
		if (m_pVIBufferCom->PickingRetrunIndex_scale_Test(g_hWnd, m_pTransformCom, vRightPos))
		{
			static_cast<CMapOneTree*>(pTree)->Picking_pos(vRightPos);
		}

	}

	RELEASE_INSTANCE(CGameInstance);
}


HRESULT CMapTile::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_TerrainTile"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_TerrainTile"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Diffuse"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Brush*/
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Brush"), TEXT("Com_Brush"),
		(CComponent**)&m_pTextureCom[TYPE_BRUSH])))
		return E_FAIL;

	/* For.Com_Filter */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_Filter"), TEXT("Com_Filter"),
		(CComponent**)&m_pTextureCom[TYPE_FILTER])))
		return E_FAIL;


	///* For.Prototype_Component_Texture_MapOneTile_FilterCanvas */
	//if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MapOneTile_FilterCanvas"), TEXT("Com_Filter"),
	//	(CComponent**)&m_pTextureCom[TYPE_FILTER])))
	//	return E_FAIL;


	/* For.Com_Navigation */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"),
		(CComponent**)&m_pNavigationCom)))
		return E_FAIL;


	return S_OK;


}

HRESULT CMapTile::SetUp_ShaderResources()
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


	/*if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResource(m_pShaderCom, "g_FilterCanvasTexture")))
		return E_FAIL;*/

	if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResources(m_pShaderCom, "g_FilterTexture")))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_RawValue("g_vBrushPos", &m_vBrushPos, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CMapTile * CMapTile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapTile*		pInstance = new CMapTile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapTile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMapTile::Clone(void * pArg)
{
	CMapTile*		pInstance = new CMapTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapTile");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapTile::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pPixel);
	m_FilterIndexSet.clear();

	

	for (auto& pTextureCom : m_pTextureCom)
		Safe_Release(pTextureCom);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
