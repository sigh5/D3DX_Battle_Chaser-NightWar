#pragma once

#include "Base.h"

/* ���ӳ��� �ʿ��� ��ü���� �� ����(CLayer)�� ���� ������ �����Ѵ�. */
/* ��ü���� ������ �����ϰ� �ִ� CLayer���� �����ϴ� Ŭ�����̴�. */
/* ��� ��ü���� ����(Tick, Late_Tick)�� ����Ѵ�. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager);
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Clear(_uint iLevelIndex);

	HRESULT Loading_Objects();


public:
	HRESULT Add_Prototype(const wstring& pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_UI(_uint iLevel, const wstring& pLayerTag,class CGameObject* pGameObject);



public:
	map<const wstring, class CGameObject*>*		Get_ProtoTypes() { return &m_Prototypes; }
	map<const wstring, class CLayer*>*				Get_Layer() { return m_pLayers; }

	class CGameObject*		Get_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pObjectNameTag);
	void					DeleteGameObject(_uint iLevelIndex, const wstring& ObjName);



	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);
	virtual		void	Final_Update();


public: /* imgui */
	// ��� ���� Object�� Imgui�� ����Ѵ�.
	// ������ ������ �̸��� szSelectedProto�� �����ش�.
	void	Imgui_ProtoViewer(OUT const _tchar*& szSelectedProto);
	
	// iLevel�� �ִ� ��� �纻 Object�� Layer���� Imgui�� ����Ѵ�.
	// ������ Object�� pSelectedObject�� �����ش�.
	// �ش� Object�� ���ų� ������� nullptr�� �����ش�.
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

private: /* ������ü������ ��Ƴ��´�. */
	map<const wstring, class CGameObject*>			m_Prototypes;
	typedef map<const wstring, class CGameObject*>	PROTOTYPES;

private: /* �纻��ü���� �����ϱ����� �����̳�. */
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