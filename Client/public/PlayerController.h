#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CGameObject;
class CPlayer;
END

/*
	렌더 기능은 수행하지않고
	플레이어들을 컨트롤하기 위한 객체
	플레이어 다중을 컨트롤 하는 기능을한다.
*/
BEGIN(Client)

class CInventory;
class CDungeon_Canvas;

class CPlayerController final :public CBase
{
	DECLARE_SINGLETON(CPlayerController);

private:
	CPlayerController() {}
	virtual ~CPlayerController() = default;

public:
	vector<CPlayer*>& Get_Players() { return m_pPlayerVec; }

	void	Reset_LateInit() { m_bLateInit = false; }

public:
	virtual HRESULT Initialize(_uint iLevel);

	HRESULT			Late_Initialize();


	void			Player_Controll_Tick(_double TimeDelta);



public: /* For.Dungeon*/
	CPlayer*	Get_Captin() { return m_pCaptinPlayer;}
	void		Set_CaptinPlayer();
	void		SyncAninmation();
	void		Change_Scene(_uint	iLevelIndex);
	
public: /*For.Combat*/
	_bool			CombatScene(void);
	//void			CurrentTurn_AnimControl();
	
	/* 조건들 나중에 현재 순서에맞는 애들을 채워야함*/
	

private:
	vector<CPlayer*>		m_pPlayerVec;
	CPlayer*				m_pCaptinPlayer = nullptr;
	
	CInventory*				m_pInventory = nullptr;
	CDungeon_Canvas*		m_pDungeonCanvas = nullptr;


private:
	_uint					m_iCurrentScene = 0;
	_bool					m_bInventoryRender = false;
	_bool					m_bLateInit = false;

public:
	virtual void Free()override;

	
};

END


