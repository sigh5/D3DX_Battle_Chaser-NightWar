#include "..\public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Imgui_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include  "Sound_Manager.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CGameInstance)

_uint			CGameInstance::m_iStaticLevelIndex = 0;
const _tchar*	CGameInstance::m_pPrototypeTransformTag = TEXT("Prototype_Component_Transform");
_bool			CGameInstance::m_bOnceCreatePlayer = false;

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pImgui_Manager(CImgui_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pFont_Manager(CFont_Manager::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pSound_Manager(CSound_Manager::GetInstance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pImgui_Manager);
	Safe_AddRef(m_pSound_Manager);
}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut)
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pInput_Device ||
		nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;
	
	m_GraphicDesc = GraphicDesc;
	m_hWnd = GraphicDesc.hWnd;

	/* 그래픽 디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWindowMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDeviceOut, ppContextOut)))
		return E_FAIL;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	/* imgui 초기화 */
	m_pImgui_Manager->Ready_Imgui(GraphicDesc.hWnd, *ppDeviceOut, *ppContextOut);

	m_iStaticLevelIndex = iNumLevels;

	/* 입력 디바이스 초기화. */
	if (FAILED(m_pInput_Device->Ready_Input_Device(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels+1)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels+1)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Add_Prototype(m_iStaticLevelIndex, m_pPrototypeTransformTag, CTransform::Create(*ppDeviceOut, *ppContextOut))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Initialize(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	if (FAILED(m_pPipeLine->Ready_PipeLine()))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;
	
	if (FAILED(m_pSound_Manager->Reserve_Manager(GraphicDesc.pSoundFilePath, 10)))
		return E_FAIL;


	return S_OK;
}

void CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pInput_Device ||
		nullptr == m_pLevel_Manager ||
		nullptr == m_pObject_Manager)
		return;

	/* 입력장치의 상태를 갱신받아온다. */
	m_pInput_Device->Invalidate_Input_Device();

	m_pImgui_Manager->Tick_Imgui();

	m_pObject_Manager->Tick(TimeDelta);
	m_pLevel_Manager->Tick(TimeDelta);
	m_pSound_Manager->Tick(TimeDelta);
	
	m_pPipeLine->Tick();

	m_pFrustum->Transform_ToWorldSpace();		// 틱이랑 레이트랑 중간에 놓았다.

	m_pObject_Manager->Late_Tick(TimeDelta);
	m_pLevel_Manager->Late_Tick(TimeDelta);

	m_pInput_Device->Reset_EveryKey();

	m_pObject_Manager->Final_Update();
}

void CGameInstance::Clear_Level(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);

}

void CGameInstance::Copy_Data(_uint iLevelPreIndex, _uint iLevelCur)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Copy_Data(iLevelPreIndex, iLevelCur);
}

HRESULT CGameInstance::Clear_Graphic_Device(const _float4 * pColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	HRESULT			hr = 0;

	hr = m_pGraphic_Device->Clear_BackBuffer_View(*pColor);
	hr = m_pGraphic_Device->Clear_DepthStencil_View();

	return hr;
}


HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Update_SwapChain(hWnd, iWinCX, iWinCY, bIsFullScreen, bNeedUpdate);
}

mutex& CGameInstance::GetContextMtx() const
{
	return m_pGraphic_Device->GetContextMtx();
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE byMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(byMouseID);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMoveState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMoveState);
}

_bool CGameInstance::Mouse_Down(CInput_Device::MOUSEKEYSTATE MouseButton)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Down(MouseButton);
}

_bool CGameInstance::Mouse_Up(CInput_Device::MOUSEKEYSTATE MouseButton)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_Up(MouseButton);
}

_bool CGameInstance::Mouse_DoubleClick(CInput_Device::MOUSEKEYSTATE MouseButton)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Mouse_DoubleClick(MouseButton);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Up(byKeyID);
}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Key_Pressing(byKeyID);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel, _bool bCopy )
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel, bCopy);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();
}

_uint CGameInstance::GetCurLevelIdx() const
{
	if (nullptr == m_pLevel_Manager)
		return 0;

	return m_pLevel_Manager->GetCurLevelIdx();
}

void CGameInstance::Scene_Change()
{
	return m_pLevel_Manager->Scene_Change();
}

void CGameInstance::Set_CopyIndexs(_uint iPrevIndex, _uint iCurIndex)
{
	if (nullptr == m_pLevel_Manager)
		return;

	return m_pLevel_Manager->Set_CopyIndexs(iPrevIndex,iCurIndex);
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ComponentPtr(iLevelIndex, pLayerTag, pComponentTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(const wstring& pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Clone_GameObject(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag,pArg);
}

void CGameInstance::SceneChange_NameVectorClear()
{
	if (nullptr == m_pObject_Manager)
		return;

	return m_pObject_Manager->SceneChange_NameVectorClear();
}

HRESULT CGameInstance::Clone_GameObject_UseImgui(_uint iLevelIndex, const wstring & pLayerTag, const wstring & pPrototypeTag, OUT CGameObject ** ppGameObject, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Clone_GameObject_UseImgui(iLevelIndex, pLayerTag, pPrototypeTag, ppGameObject,pArg);
}

CGameObject* CGameInstance::Clone_UI(_uint iLevel, const wstring & pLayerTag, CGameObject * pGameObject)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_UI(iLevel, pLayerTag, pGameObject);
}

void CGameInstance::DeleteGameObject(_uint iLevelIndex, const wstring & ObjName)
{
	if (nullptr == m_pObject_Manager)
		return ;

	return m_pObject_Manager->DeleteGameObject(iLevelIndex, ObjName);
}

map<const wstring, class CGameObject*>* CGameInstance::Get_ProtoTypes()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Get_ProtoTypes();
}

map<const wstring, class CLayer*>* CGameInstance::Get_Layer()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Layer();
}



CGameObject * CGameInstance::Get_SelectObject()
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Get_SelectObject();
}

HRESULT CGameInstance::Loading_Objects()
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	return m_pObject_Manager->Loading_Objects();
}

CGameObject * CGameInstance::Get_GameObject(_uint iLevelIndex, const wstring & pLayerTag, const wstring & pObjectNameTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Get_GameObject(iLevelIndex, pLayerTag, pObjectNameTag);
}

CGameObject* CGameInstance::Load_Object(const _tchar * pDataFileNam, _uint iCurLevel )
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Load_Object(pDataFileNam, iCurLevel);
}

CGameObject* CGameInstance::Load_Effect(const _tchar * pDataFileName, _uint iCurLevel, _bool IsHaveLayer )
{
	if (nullptr == m_pObject_Manager)
		return nullptr;
	return m_pObject_Manager->Load_Effect(pDataFileName, iCurLevel, IsHaveLayer);
}

void CGameInstance::Change_Level(_uint iLevleIdx)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Change_Level(iLevleIdx);

}

void CGameInstance::All_Object_RenderActive(_bool bRenderActive)
{

}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

void CGameInstance::Imgui_ComponentViewer(_uint iLevel, OUT wstring & TextureTag, COMPONENT_TYPE eType)
{
	if (nullptr == m_pComponent_Manager)
		return;

	m_pComponent_Manager->Imgui_ComponentViewer(iLevel, TextureTag, eType);
}

void CGameInstance::Remove_ProtoComponent(_uint iLevel, const wstring & pComponentName)
{
	if (nullptr == m_pComponent_Manager)
		return;

	m_pComponent_Manager->Remove_ProtoComponent(iLevel, pComponentName);
}



void CGameInstance::Render_ImGui()
{
	if (nullptr == m_pImgui_Manager)
		return;

	m_pImgui_Manager->Render_Imgui();
}

void CGameInstance::Render_Update_ImGui()
{
	if (nullptr == m_pImgui_Manager)
		return;

	m_pImgui_Manager->Render_Update_ImGui();
}

void CGameInstance::Add_ImguiTabObject(CImguiObject* ImguiObject)
{
	if (m_pImgui_Manager == nullptr) return;
	m_pImgui_Manager->Add_ImguiTabObject(ImguiObject);
}

void CGameInstance::Add_ImguiWindowObject(CImguiObject* ImguiObject)
{
	if (m_pImgui_Manager == nullptr) return;
	m_pImgui_Manager->Add_ImguiWindowObject(ImguiObject);
}

void CGameInstance::Clear_ImguiObjects()
{
	if (m_pImgui_Manager == nullptr) return;
	m_pImgui_Manager->Clear_ImguiObjects();
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix_Inverse(eState);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4 CGameInstance::Get_CamPositon() 
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPositon();
}

_double CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL; 
	
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Ready_Timer(pTimerTag);
}

void CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update_Timer(pTimerTag);
}

void CGameInstance::Set_Timedelta(const _tchar * pTimerTag, _double Time)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Set_Timedelta(pTimerTag, Time);
}

 LIGHTDESC * CGameInstance::Get_LightDesc(wstring NameTag) 
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(NameTag);
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, wstring NameTag, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice,pContext, NameTag,LightDesc);
}

HRESULT CGameInstance::Delete_Light(wstring NameTag)
{
	if (nullptr == m_pLight_Manager)
		assert(!"CGameInstance::Clear_Light()");

	return m_pLight_Manager->Delete_Light(NameTag);
}

void CGameInstance::Clear_Light()
{
	if (nullptr == m_pLight_Manager)
		assert(!"CGameInstance::Clear_Light()");

	return m_pLight_Manager->Clear();
}

void CGameInstance::Set_LightView_Matrirx(wstring nameTag, _float4x4 fMatrix)
{
	if (nullptr == m_pLight_Manager)
		assert(!"CGameInstance::Set_LightView_Matrirx()");

	return m_pLight_Manager->Set_LightView_Matrirx(nameTag,fMatrix);
}

void CGameInstance::Set_LightProj_Matrirx(wstring nameTag, _float4x4 fMatrix)
{
	if (nullptr == m_pLight_Manager)
		assert(!"CGameInstance::Set_LightView_Matrirx()");

	return m_pLight_Manager->Set_LightProj_Matrirx(nameTag, fMatrix);
}

_float4x4 CGameInstance::Get_Light_Matrix(wstring nameTag)
{
	if (nullptr == m_pLight_Manager)
		assert(!"CGameInstance::Get_Light_Matrix()");

	return m_pLight_Manager->Get_Light_Matrix(nameTag);
}

_float4x4 CGameInstance::Get_Light_ProjMatrix(wstring nameTag)
{
	if (nullptr == m_pLight_Manager)
		assert(!"CGameInstance::Get_Light_ProjMatrix()");

	return m_pLight_Manager->Get_Light_Matrix(nameTag);
}

HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(pDevice, pContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(pFontTag, pText, vPos, fRadian, vScale, vColor);
}

_bool CGameInstance::isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isInFrustum_WorldSpace(vWorldPos, fRange);
}

_bool CGameInstance::isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isInFrustum_LocalSpace(vLocalPos, fRange);
}

ID3D11ShaderResourceView * CGameInstance::Get_DepthTargetSRV()
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(TEXT("Target_Depth"));
}

ID3D11ShaderResourceView * CGameInstance::Get_DiffuseTargetSRV()
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse"));
}

ID3D11ShaderResourceView * CGameInstance::Get_ShadeTargetSRV()
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(TEXT("Target_Shade"));
}

ID3D11ShaderResourceView * CGameInstance::Get_SpecularTargetSRV()
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(TEXT("Target_Specular"));
}

ID3D11ShaderResourceView * CGameInstance::Get_ShadowTargetSRV()
{
	if (nullptr == m_pTarget_Manager)
		return nullptr;

	return m_pTarget_Manager->Get_SRV(TEXT("Target_ShadowDepth"));
}

void CGameInstance::Play_Sound(const _tchar * pSoundKey, _float fVolume, _bool bIsBGM, _int iManualChannelIndex)
{
	assert(nullptr != m_pSound_Manager && " CGameInstance::Play_Sound");
	return m_pSound_Manager->Play_Sound(pSoundKey, fVolume, bIsBGM, iManualChannelIndex);
}

void CGameInstance::Stop_Sound(_uint iManualChannelIndex)
{
	assert(nullptr != m_pSound_Manager && " CGameInstance::Stop_Sound");
	return m_pSound_Manager->Stop_Sound(iManualChannelIndex);
}

void CGameInstance::Set_Volume(_uint iManualChannelIndex, _float fVolume)
{
	assert(nullptr != m_pSound_Manager && " CGameInstance::Set_Volume");
	return m_pSound_Manager->Set_Volume(iManualChannelIndex,fVolume);
}

void CGameInstance::Set_MasterVolume(_float fVolume)
{
	assert(nullptr != m_pSound_Manager && " CGameInstance::Set_MasterVolume");
	return m_pSound_Manager->Set_MasterVolume(fVolume);
}

void CGameInstance::Set_SoundDesc(const _tchar * pSoundKey, CSound::SOUND_DESC & SoundDesc)
{
	assert(nullptr != m_pSound_Manager && " CGameInstance::Set_SoundDesc");
	return m_pSound_Manager->Set_SoundDesc(pSoundKey,SoundDesc);
}

HRESULT CGameInstance::Load_SoundFile(const char * pFilePath, const _tchar* pTag)
{
	assert(nullptr != m_pSound_Manager && " CGameInstance::Set_SoundDesc");
	return m_pSound_Manager->Load_SoundFile(pFilePath, pTag);
}

map<const _tchar*, CSound*>& CGameInstance::Get_Sound()
{
	assert(nullptr != m_pSound_Manager && " CGameInstance::Get_Sound");
	return m_pSound_Manager->Get_Sound();
}

void CGameInstance::Release_Engine()
{
	CImgui_Manager::GetInstance()->DestroyInstance();

	CGameInstance::GetInstance()->DestroyInstance();

	CSound_Manager::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CFont_Manager::GetInstance()->DestroyInstance();

	CFrustum::GetInstance()->DestroyInstance();

	CTarget_Manager::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pImgui_Manager);
}

