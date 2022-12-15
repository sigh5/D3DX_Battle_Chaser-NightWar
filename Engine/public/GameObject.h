#pragma once

#include "Transform.h"
#include "GameUtils.h"
BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };
	enum CHAR_STATE {
		m_Normal_Attack, m_Skill1_Attack, m_Skill2_Attack, m_Uitimate, m_Buff
		, m_WideAreaBuff, m_Use_Item, m_Defence, m_Light_Hit, m_Heavy_Hit, m_Flee, m_Die, m_Viroty
		, CHAR_STATE_END
	};
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
	// 이 오브젝트가 가지고 있는 component의 Imgui_RenderProtpery함수를 실행하는 함수.
	void Imgui_RenderComponentProperties();

	// 이 오브젝트에서 보여줄 데이터를 imgui로 작성한다.
	virtual void Imgui_RenderProperty(){}
	virtual _bool Piciking_GameObject() { return false; }

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
	/* 객체들이 사용해야 할 컴포넌트들을 보관한다. */
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
	virtual _bool		To_Normal_Attack();
	virtual _bool		To_Skill1_Attack();
	virtual _bool		To_Skill2_Attack();
	virtual _bool		To_Uitimate();
	virtual _bool		To_Buff();
	virtual _bool		To_WideAreaBuff();
	virtual _bool		To_Use_Item();
	virtual _bool		To_Defence();
	virtual _bool		To_Light_Hit();
	virtual _bool		To_Heavy_Hit();
	virtual _bool		To_Flee();
	virtual _bool		To_Die();
	virtual _bool		To_Viroty();



	void Set_FsmState(_bool bTrue, CHAR_STATE eType) { m_bState[eType] = bTrue; }

private:
	_bool	m_bState[CHAR_STATE_END] = { false, };

public:	
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END