#pragma once

#include "Base.h"

/* 게임내에 필요한 객체들을 내 기준(CLayer)에 따라 나누어 보관한다. */
/* 객체들을 나누어 저장하고 있는 CLayer들을 보관하는 클래스이다. */
/* 모든 객체들의 갱신(Tick, Late_Tick)을 담당한다. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager);
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Clear(_uint iLevelIndex);
	HRESULT Copy_Data(_uint iLevelIndex);			// 씬전환시 데이터들을 카피한다.
	HRESULT Loading_Objects();


public:
	HRESULT Add_Prototype(const wstring& pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_UI(_uint iLevel, const wstring& pLayerTag,class CGameObject* pGameObject);


	void	Load_Object(const _tchar *pDataFileName = nullptr, _uint iCurLevel = 0);


public:
	map<const wstring, class CGameObject*>*			Get_ProtoTypes() { return &m_Prototypes; }
	map<const wstring, class CLayer*>*				Get_Layer() { return m_pLayers; }

	class CGameObject*		Get_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjectNameTag);
	void					DeleteGameObject(_uint iLevelIndex, const wstring& ObjName);
	void					Change_Level(_uint iLevleIdx);


	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);
	virtual		void	Final_Update();


public: /* imgui */
	// 모든 원본 Object를 Imgui로 출력한다.
	// 선택한 원본의 이름을 szSelectedProto로 돌려준다.
	void	Imgui_ProtoViewer(OUT const _tchar*& szSelectedProto);
	
	// iLevel에 있는 모든 사본 Object를 Layer별로 Imgui로 출력한다.
	// 선택한 Object는 pSelectedObject로 돌려준다.
	// 해당 Object가 없거나 사라지면 nullptr로 돌려준다.
	void	Imgui_ObjectViewer(_uint iLevel, OUT CGameObject*& pSelectedObject);

	void	Imgui_Picking(_uint iLevel, OUT CGameObject*& pSelectedObject);

	HRESULT Clone_GameObject_UseImgui(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, OUT CGameObject** ppGameObject, void* pArg = nullptr);
	

	void		Set_SelectedGameObject(class CGameObject* pGameObject){m_pSelectObject =  pGameObject;}
	class CGameObject* Get_SelectObject() { return m_pSelectObject; }


private:
	//void	Imgui_Select_LayerType();
	//void	Imgui_Select_ProtoType();
	//void	Imgui_Select_Texture();
	//void	Imgui_Select_Model();

	void	Imgui_RemoveObject(_uint iLevel,OUT class CGameObject** ppGameObject);
	void	Imgui_Save();
	void	Imgui_Load();

private: /* 원형객체들을ㅇ 모아놓는다. */
	map<const wstring, class CGameObject*>			m_Prototypes;
	typedef map<const wstring, class CGameObject*>	PROTOTYPES;

private: /* 사본객체들을 보관하기위한 컨테이너. */
	map<const wstring, class CLayer*>*			m_pLayers = nullptr;
	typedef map<const wstring, class CLayer*>	LAYERS;
	_uint										m_iNumLevels = 0;

private:
	char						m_szSaveDataname[MAX_PATH] = "";
	char						m_szTexturName[MAX_PATH] = "";
	char						m_szProtoName[MAX_PATH] = "";
	char						m_szLayerName[MAX_PATH] = "";
	char						m_szObjectName[MAX_PATH] = "";

	vector<_tchar*>				m_vecNameArray;
	
	wstring						TextureOrModelName = TEXT("");


	class CGameObject*			m_pSelectObject = nullptr;


	vector<const _tchar*>		m_LayerName;


private:
	class CGameObject* Find_Prototype(const wstring&  pObjectNameTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& pLayerTag);

public:
	virtual void Free() override;
};

END