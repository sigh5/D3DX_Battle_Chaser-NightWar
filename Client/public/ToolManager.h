#pragma once
#include "Client_Defines.h"
#include "Base.h"



BEGIN(Engine)
class CGameObject;

END

BEGIN(Client)

class CToolManager final :public CBase
{
	DECLARE_SINGLETON(CToolManager);

private:
	CToolManager();
	virtual ~CToolManager() = default;

public:
	int		Get_RadioButtonValue()const { return m_iRadioButton;}



public:
	void	Imgui_SelectParentViewer();

private:
	void	Imgui_Select_ParentCanvas();
	void	Imgui_Select_ParentTerrain();
	
	void	Imgui_CreateObject();
	void	Imgui_Create_UI();
	void	Imgui_Create_Object();
	void	Imgui_Show_Parent();


	void	Imgui_Select_LayerType();
	void	Imgui_Select_ProtoType();
	void	Imgui_Select_Texture();
	void	Imgui_Select_Model();

	void	Imgui_Change_model();
	void	Imgui_Change_Texture();

	void	Imgui_Crate_FilterMap();
	void	Imgui_Camera_Type();
	void	Imgui_Create_NaviGator();



	HRESULT	Imgui_Save_NaviGator();

private:
	char						m_szSaveDataname[MAX_PATH] = "";
	char						m_szProtoName[MAX_PATH] = "";
	char						m_szLayerName[MAX_PATH] = "";
	char						m_szObjectName[MAX_PATH] = "";
	char						m_szTexturName[MAX_PATH] = "";
	char						m_szModelName[MAX_PATH] = "";

	int							m_iShaderPass = 0;

	vector<_tchar*>				m_vecNameArray;
	vector<const _tchar*>		m_LayerName;

	wstring						TextureName = TEXT("");
	wstring						ModelName = TEXT("");


	CGameObject*				m_pSelectedObject = nullptr;
	CGameObject*				m_pSelectTerrain = nullptr;
	int							m_iRadioButton = 0;

	int							m_iTerrainPickingRadioButton = 0;
	int							m_iCameraRadioButton = 0;
private:
	void wc2c(const wchar_t* szWchar, char* szChar)
	{
		_int len;
		_int slength = lstrlen(szWchar) + 1;
		len = ::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, 0, 0, 0, 0);
		::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, szChar, len, 0, 0);
	}
	

public:
	virtual void Free()override;

};

END