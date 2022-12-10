#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	HRESULT Initialize();
	void Tick(_double TimeDelta);
	void Late_Tick(_double TimeDelta);
	void Final_Update();
	
public:
	list<class CGameObject*>& GetGameObjects() { return m_GameObjects; }

	void	Delete_GameObject(class CGameObject*& pObject);
	void	CopyData(list<class CGameObject*>& Temp);

public:
	void	Change_Level(_uint iLevleIdx);



private:
	list<class CGameObject*>			m_GameObjects;
	typedef list<class CGameObject*>	GAMEOBJECTS;

public:
	static CLayer* Create();
	virtual void Free() override;

};

END