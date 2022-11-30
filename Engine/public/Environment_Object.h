#pragma once
#include "GameObject.h"
#include "GameUtils.h"

BEGIN(Engine)

class ENGINE_DLL CEnvironment_Object : public CGameObject
{
public:
	typedef struct tagEnvironmentObject :public GAMEOBJECTDESC
	{
		_tchar*			m_pModelTag = TEXT("");

	}ENVIRONMENTDESC;


protected:
	CEnvironment_Object(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CEnvironment_Object(const CEnvironment_Object& rhs);
	virtual ~CEnvironment_Object() = default;

public:
	_tchar*				Get_ModelTag()const { return m_EnviromentDesc.m_pModelTag; }
	void				Set_ModelTag(const _tchar* pModelTag) { lstrcpy(m_EnviromentDesc.m_pModelTag, pModelTag); };

public:
	virtual HRESULT		Initialize_Prototype()override;
	virtual HRESULT		Initialize(void* pArg)override;
	virtual HRESULT		Last_Initialize()override;
	virtual void		Tick(_double TimeDelta)override;
	virtual void		Late_Tick(_double TimeDelta)override;
	virtual HRESULT		Render()override;

protected:
	ENVIRONMENTDESC						m_EnviromentDesc;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

};

END