#pragma once

#include "Transform.h"
#include "GameUtils.h"
BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };
public:
	typedef struct tagGameObjectDesc
	{
		CTransform::TRANSFORMDESC  TransformDesc;
	}GAMEOBJECTDESC;



protected:
	CGameObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	static	const wstring			m_pTransformComTag;

public:
	class CComponent* Find_Component(const wstring& pComponentTag);
	const	 _tchar*				Get_ProtoName() const { return m_ProtoName; }
	void							Set_ProtoName(const _tchar* pNametag) { m_ProtoName= pNametag; }

	const    _tchar*				Get_ObjectName()const { return m_ObjectName; }
	void							Set_ObjectName(const _tchar* pNametag) { m_ObjectName = pNametag; }

public:
	virtual void					Change_Level_Data(_uint iLevleIdx) {}


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize() = 0;
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();
public:
	virtual		void	Final_Update() sealed;

public: /* imgui */
	// �� ������Ʈ�� ������ �ִ� component�� Imgui_RenderProtpery�Լ��� �����ϴ� �Լ�.
	void Imgui_RenderComponentProperties();

	// �� ������Ʈ���� ������ �����͸� imgui�� �ۼ��Ѵ�.
	virtual void Imgui_RenderProperty(){}
	virtual _bool Piciking_GameObject() = 0;

	void					Set_parent(CGameObject* pGameObject);
	virtual void			Set_parentName(_uint iCulLevel, const _tchar* pParentTag) {};


	CGameObject*			Get_parentName() { 
		if (m_pParentObject == nullptr)
			return nullptr;
		return m_pParentObject;
	}




public:
	CTransform*		Get_Transform() { return m_pTransformCom; }
	class	CComponent	*Get_Component(const wstring& ComponentTag);


protected:
	CGameObject*			m_pParentObject = nullptr;

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected:
	/* ��ü���� ����ؾ� �� ������Ʈ���� �����Ѵ�. */
	map<const wstring, class CComponent*>			m_Components;
	CTransform*				m_pTransformCom = nullptr;
protected:	
	HRESULT Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

	HRESULT Remove_component(const wstring& pComponentTag);

protected:
	const					_tchar*					m_ObjectName = TEXT("");
	const					_tchar*					m_ProtoName = TEXT("");

	_bool											m_bLast_Initlize = false;

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END