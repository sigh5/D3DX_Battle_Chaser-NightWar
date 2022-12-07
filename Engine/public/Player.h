#pragma once
#include "GameObject.h"
#include "BaseDelegater.h"

BEGIN(Engine)

class ENGINE_DLL CPlayer abstract: public CGameObject
{
public:
	enum MAPTYPE { DUNGEON_PLAYER, COMBAT_PLAYER,  MAPTYPE_END };

public:
	typedef struct tag_PlayerDesc :GAMEOBJECTDESC
	{

	}PLAYERDESC;

protected:
	CPlayer(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;
public:
	virtual void KeyInput(_double TimeDelta) = 0;

public: /* imgui */
	virtual _bool Piciking_GameObject()override { return false; }

protected:
	PLAYERDESC				m_PlayerDesc;
	MAPTYPE					m_ePlayerType = DUNGEON_PLAYER;
public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;


};

END