#include "stdafx.h"
#include "..\public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Dynamic.h"
#include "LoadingImage.h"
#include "Client_Manager.h"
#include "ToolManager.h"
#include "PlayerController.h"
#include "CombatController.h"
#include "Damage_Font_Manager.h"
#include "Explain_FontMgr.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
	, m_pToolManager(CToolManager::GetInstance())

{
	
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* 게임엔진 초기화 */
	GRAPHIC_DESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHIC_DESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWindowMode = GRAPHIC_DESC::WINMODE_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	/*if (FAILED(Ready_BufferLock_UnLock()))
		return E_FAIL;*/

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Comic"), TEXT("../Bin/Resources/fonts/131.SpriteFont"))))
		return E_FAIL;


	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL_LOGO)))
		return E_FAIL;


	srand((unsigned int)time(NULL));

	


	return S_OK;
}

HRESULT CMainApp::LastInitalize()
{
	/* 이곳에서 싱클톤 관련 Initialize 진행합니다*/
	if (m_bLastUpdate)
		return S_OK;

	

	m_bLastUpdate = true;

	return S_OK;
}



void CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	LastInitalize();

	
	m_pGameInstance->Tick_Engine(TimeDelta);
	m_pToolManager->Imgui_SelectParentViewer();

	CClient_Manager::TimeDelta = TimeDelta;
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance || 
		nullptr == m_pRenderer)
		return E_FAIL;


	m_pGameInstance->Render_ImGui();
	m_pGameInstance->Clear_Graphic_Device(&_float4(0.0f, 0.0f, 0.0f, 1.f));
	m_pRenderer->Draw_RenderGroup();
	m_pGameInstance->Render_Update_ImGui();
	m_pGameInstance->Render_Level();
	m_pGameInstance->Present();


	return S_OK;
}

//HRESULT CMainApp::Ready_Gara()
//{
//	_ulong		dwByte = 0;
//	HANDLE		hFile = CreateFile(TEXT("../..Bin/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
//	if (0 == hFile)
//		return E_FAIL;
//
//	_float3		vPoints[3];
//
//	vPoints[0] = _float3(0.0f, 0.f, 5.0f);
//	vPoints[1] = _float3(5.0f, 0.f, 0.0f);
//	vPoints[2] = _float3(0.0f, 0.f, 0.0f);
//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	vPoints[0] = _float3(0.0f, 0.f, 5.0f);
//	vPoints[1] = _float3(5.0f, 0.f, 5.0f);
//	vPoints[2] = _float3(5.0f, 0.f, 0.0f);
//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	vPoints[0] = _float3(0.0f, 0.f, 10.0f);
//	vPoints[1] = _float3(5.0f, 0.f, 5.0f);
//	vPoints[2] = _float3(0.0f, 0.f, 5.0f);
//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	vPoints[0] = _float3(5.0f, 0.f, 5.0f);
//	vPoints[1] = _float3(10.0f, 0.f, 0.0f);
//	vPoints[2] = _float3(5.0f, 0.f, 0.0f);
//	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
//
//	CloseHandle(hFile);
//
//	return S_OK;
//}

HRESULT CMainApp::Resize_BackBuffer()
{
	GRAPHIC_DESC	tGraphicDesc;
	ZeroMemory(&tGraphicDesc, sizeof(GRAPHIC_DESC));

	tGraphicDesc.hWnd = g_hWnd;
	if (!g_bFullScreen)
	{
		tGraphicDesc.iViewportSizeX = g_iWinSizeX;
		tGraphicDesc.iViewportSizeY = g_iWinSizeY;
	}
	else
	{
		tGraphicDesc.iViewportSizeX = GetSystemMetrics(SM_CXSCREEN);
		tGraphicDesc.iViewportSizeY = GetSystemMetrics(SM_CYSCREEN);
	}

	if (FAILED(m_pGameInstance->Update_SwapChain(tGraphicDesc.hWnd, tGraphicDesc.iViewportSizeX, tGraphicDesc.iViewportSizeY, g_bFullScreen, g_bNeedResizeSwapChain)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID || 
		nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */	
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Texture_LoadingImage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures2D/LoadingImage/LoadScreen%d.png"),CTexture::TYPE_END,6))))
		return E_FAIL;

	



	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingImage"),
		CLoadingImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}


HRESULT CMainApp::Ready_BufferLock_UnLock()
{
	ID3D11Texture2D*			pTexture2D = nullptr;

	D3D11_TEXTURE2D_DESC		TextureDesc;
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

			//if (j < 128)
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255,255);
		/*	else
				pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);*/
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

	return S_OK;
}



CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	CCombatController::DestroyInstance();
	CPlayerController::DestroyInstance();
	CToolManager::DestroyInstance();
	CDamage_Font_Manager::DestroyInstance();
	CExplain_FontMgr::DestroyInstance();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();


	

}
