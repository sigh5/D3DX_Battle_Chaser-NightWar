#include "stdafx.h"
#include "..\public\Terrain.h"
#include "GameInstance.h"
#include "Environment_Object.h"

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

	m_bLast_Initlize = true;


	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	Last_Initialize();

	__super::Tick(TimeDelta);
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	Create_Object();

	Ready_BufferLock_UnLock();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

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
	
	_float3 iIndex = _float3(1.f, 1.f, 1.f);

	if (ImGui::IsMouseClicked(0))
	{
		if (m_pVIBufferCom->PickingRetrunIndex(g_hWnd, m_pTransformCom, iIndex))
		{
			m_pPixel[ (_uint(iIndex.x)) ] = D3DCOLOR_ARGB(255, 0, 0, 0);
			m_pPixel[(_uint(iIndex.y)) ] = D3DCOLOR_ARGB(255, 0, 0, 0);
			m_pPixel[( _uint(iIndex.z)) ] = D3DCOLOR_ARGB(255, 0, 0, 0);
		}

		Safe_Release(pTexture2D);
		pTexture2D = nullptr;
		TextureDesc;
		
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

		_ulong*		pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];

		
		for (_uint i = 0; i < TextureDesc.Height; ++i)
		{
			for (_uint j = 0; j < TextureDesc.Width; ++j)
			{
				_uint iIndex = i*TextureDesc.Height + j;
				pPixel[iIndex] = m_pPixel[iIndex];
			}
		}


		D3D11_MAPPED_SUBRESOURCE		SubResource;
		ZeroMemory(&SubResource, sizeof SubResource);

		// D3D11_MAP_WRITE_NO_OVERWRITE 툴작업할때는 이녀석으로 하는게 좋음

		m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);  //DX_9 Lock ==Map

		memcpy(SubResource.pData, pPixel, sizeof(_ulong) *TextureDesc.Width * TextureDesc.Height);

		m_pContext->Unmap(pTexture2D, 0);

		if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures2D/Filter.dds"))))
			return E_FAIL;

		Safe_Delete_Array(pPixel);
		Safe_Release(pTexture2D);


		Safe_Release(m_pTextureCom[TYPE_FILTER]);
		Remove_component(TEXT("Com_Filter"));


		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Test_Filter"), TEXT("Com_Filter"),
			(CComponent**)&m_pTextureCom[TYPE_FILTER])))
			return E_FAIL;

	}



	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}



void CTerrain::Create_Object()
{
	if (!m_bCreateObject)
		return;

	_float4 Temp;
	if (m_pVIBufferCom->PickingBuffer(g_hWnd, m_pTransformCom, Temp))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->Mouse_Down(CInput_Device::MOUSEKEYSTATE::DIM_LB))
		{
			CGameObject* pGameObject = nullptr;
			CEnvironment_Object::ENVIRONMENTDESC Desc;
			ZeroMemory(&Desc, sizeof(CEnvironment_Object::ENVIRONMENTDESC));

			_tchar *Text = new _tchar[MAX_PATH];
			m_NameVector.push_back(Text);
			lstrcpy(Text, m_ObjData.szTextureName.c_str());
			Desc.m_pModelTag = Text;

			pGameInstance->Clone_GameObject_UseImgui(LEVEL_GAMEPLAY, m_ObjData.szLayerName,m_ObjData.szProtoName, &pGameObject,&Desc);
			if (pGameObject == nullptr)
				MSG_BOX(" CTerrain::Piciking_GameObject");

			wstring sour = to_wstring(m_iObjNameNumber++);
			_tchar *NewName = new _tchar[MAX_PATH];
			lstrcpy(NewName, (m_ObjData.szTextureName + sour).c_str());
			m_NameVector.push_back(NewName);

			pGameObject->Set_ObjectName(NewName);
			pGameObject->Set_ProtoName(m_ObjData.szProtoName.c_str());
			pGameObject->Get_Transform()->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&Temp));

		}

		RELEASE_INSTANCE(CGameInstance);
	}



	
}


void CTerrain::Set_MapObject(Create_OBJECTDESC& pArg)
{
	m_bCreateObject = true;
	
	m_ObjData.szLayerName = pArg.szLayerName;
	m_ObjData.szObjectName = pArg.szObjectName;
	m_ObjData.szProtoName = pArg.szProtoName;
	m_ObjData.szTextureName = pArg.szTextureName;
	
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
	if (FAILED(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0)))
		return E_FAIL;
	if (FAILED(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResource(m_pShaderCom, "g_FilterTexture", 0)))
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



	for (auto& pName : m_NameVector)
		Safe_Delete_Array(pName);


	for (auto& pTextureCom : m_pTextureCom)
		Safe_Release(pTextureCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
