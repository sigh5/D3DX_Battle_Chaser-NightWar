#pragma once

/* 싱글톤.  */
/* 클라이언트와 엔진의 소통을 위해 만들어진 클래스. */

#include "Base.h"
#include "Input_Device.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Graphic_Device.h"
#include "Sound_Manager.h"
#define CONTEXT_LOCK CContext_LockGuard _CtxLock_(CGameInstance::GetInstance()->GetContextMtx());

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	static				_bool		m_bOnceCreatePlayer;
	static	 _uint  Get_StaticLevelIndex() { return m_iStaticLevelIndex; }
	const  _bool  Get_IsProtoTypeCreate(_uint iLevel) const { return m_bIsCreatePrototypes[iLevel]; }
	void		 Set_ProtoTypeCreate(_uint iLevel) { m_bIsCreatePrototypes[iLevel] = true; }	//트루만 선언해야됌

	HWND GetHWND() { return m_hWnd; }
	const GRAPHIC_DESC&	Get_GraphicDesc()const { return m_GraphicDesc; }


	void			Setting_MonsterScene(_int iMonsterNum) { m_iMonsterNum = iMonsterNum; }
	const			_int Get_Setting_MonsterScene()const { return m_iMonsterNum; }

public: /* For.GameInstance*/
	static const		_tchar*     m_pPrototypeTransformTag;

public: /* For.GameInstance */
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	void Tick_Engine(_double TimeDelta);
	void Clear_Level(_uint iLevelIndex);
	void Copy_Data(_uint iLevelPreIndex, _uint iLevelCur);

public: /* For.Graphic_Device */
	HRESULT Clear_Graphic_Device(const _float4* pColor);
	HRESULT Present();
	HRESULT Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);
	mutex& GetContextMtx() const;

public: /* For.Input_Device */
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(CInput_Device::MOUSEKEYSTATE byMouseID);
	_long		Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMoveState);
	_bool		Mouse_Down(CInput_Device::MOUSEKEYSTATE MouseButton);
	_bool		Mouse_Up(CInput_Device::MOUSEKEYSTATE MouseButton);
	_bool		Mouse_DoubleClick(CInput_Device::MOUSEKEYSTATE MouseButton);
	_bool		Key_Down(_ubyte byKeyID);
	_bool		Key_Up(_ubyte byKeyID);
	_bool		Key_Pressing(_ubyte byKeyID);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel,_bool bCopy=false);
	HRESULT Render_Level();
	_uint GetCurLevelIdx() const;
	void	Scene_Change();
	void	Set_CopyIndexs(_uint iPrevIndex, _uint iCurIndex);

public: /* For.Object_Manager */
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	HRESULT Add_Prototype(const wstring& pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	void	SceneChange_NameVectorClear();

	HRESULT Clone_GameObject_UseImgui(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, OUT CGameObject** ppGameObject, void* pArg = nullptr);
	class CGameObject* Clone_UI(_uint iLevel, const wstring& pLayerTag, class CGameObject* pGameObject);
	void	DeleteGameObject(_uint iLevelIndex, const wstring& ObjName);

	map<const wstring, class CGameObject*>* 	Get_ProtoTypes();
	map<const wstring, class CLayer*>*			Get_Layer();

	class CGameObject* Get_SelectObject();
	HRESULT Loading_Objects();
	class CGameObject*		Get_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjectNameTag);
	class CGameObject*	Load_Object(const _tchar *pDataFileName = nullptr,_uint iCurLevel=0);
	class CGameObject*	Load_Effect(const _tchar *pDataFileName = nullptr, _uint iCurLevel = 0,  _bool IsHaveLayer = false);
	
	
	void	Change_Level(_uint iLevleIdx);
	void	All_Object_RenderActive(_bool bRenderActive);


public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void* pArg = nullptr);
	void		Imgui_ComponentViewer(_uint iLevel, OUT wstring& TextureTag, COMPONENT_TYPE eType);
	void		Remove_ProtoComponent(_uint iLevel, const wstring& pComponentName);

public: // for imgui manager
	void Render_ImGui();
	void Render_Update_ImGui();
	void Add_ImguiTabObject(class CImguiObject* ImguiObject);
	void Add_ImguiWindowObject(class CImguiObject* ImguiObject);
	void Clear_ImguiObjects();

public: /* For.PipeLine */
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState);
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4		Get_CamPositon();

public:/* For.TimeManager */
	_double		Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT		Ready_Timer(const _tchar* pTimerTag);
	void		Update_Timer(const _tchar* pTimerTag);
	void		Set_Timedelta(const _tchar * pTimerTag, _double Time);

public:/* For.Light_Manager */
	LIGHTDESC*			Get_LightDesc(wstring NameTag);
	HRESULT				Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, wstring NameTag, const LIGHTDESC & LightDesc);
	HRESULT				Delete_Light(wstring NameTag);
	void				Clear_Light();
	void				Set_LightView_Matrirx(wstring nameTag, _float4x4 fMatrix);
	void				Set_LightProj_Matrirx(wstring nameTag, _float4x4 fMatrix);
	_float4x4			Get_Light_Matrix(wstring nameTag);
	_float4x4			Get_Light_ProjMatrix(wstring nameTag);




public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor = XMVectorSet(1.f, 0.f, 0.f, 1.f));

public: /* For.Frustum */
	_bool isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);


public: /* For.Target_Manager */
	ID3D11ShaderResourceView* Get_DepthTargetSRV();
	ID3D11ShaderResourceView* Get_DiffuseTargetSRV();
	ID3D11ShaderResourceView* Get_ShadeTargetSRV();
	ID3D11ShaderResourceView* Get_SpecularTargetSRV();
	ID3D11ShaderResourceView* Get_ShadowTargetSRV();
public: /* For.Sound_Manager */
	void Play_Sound(const _tchar *pSoundKey, _float fVolume, _bool bIsBGM = false, _int iManualChannelIndex = -1);
	void Stop_Sound(_uint iManualChannelIndex);
	void Set_Volume(_uint iManualChannelIndex, _float fVolume);
	void Set_MasterVolume(_float fVolume);
	void Set_SoundDesc(const _tchar *pSoundKey, CSound::SOUND_DESC& SoundDesc);
	HRESULT Load_SoundFile(const char *pFilePath, const _tchar* pTag = nullptr);
	map<const _tchar*, CSound*>& Get_Sound();


private:
	static				_uint		m_iStaticLevelIndex;
	_bool							m_bIsCreatePrototypes[20] = { false, };	//넉넉하게 20으로 잡자고 씬마다
	_int							m_iMonsterNum = -99;

private:
	class CGraphic_Device*			m_pGraphic_Device		= nullptr;
	class CInput_Device*			m_pInput_Device			= nullptr;
	class CLevel_Manager*			m_pLevel_Manager		= nullptr;
	class CObject_Manager*			m_pObject_Manager		= nullptr;
	class CComponent_Manager*		m_pComponent_Manager	= nullptr;
	class CImgui_Manager*			m_pImgui_Manager		= nullptr;
	class CPipeLine*				m_pPipeLine				= nullptr;
	class CTimer_Manager*			m_pTimer_Manager		= nullptr;
	class CLight_Manager*			m_pLight_Manager		= nullptr;
	class CFont_Manager*			m_pFont_Manager			= nullptr;
	class CFrustum*					m_pFrustum				= nullptr;
	class CTarget_Manager*			m_pTarget_Manager		= nullptr;
	class CSound_Manager*			m_pSound_Manager		= nullptr;
public:
	static void Release_Engine();

private:
	GRAPHIC_DESC		m_GraphicDesc;
	HWND m_hWnd;


public:
	virtual void Free() override;
};

END