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

class CPlayerController final :public CBase
{
	DECLARE_SINGLETON(CPlayerController);

private:
	CPlayerController() {}
	virtual ~CPlayerController() = default;

public:
	vector<CPlayer*>& Get_Players() { return m_pPlayerVec; }

public:
	virtual HRESULT Initialize(_uint iLevel);
	
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
	_uint					m_iCurrentScene = 0;
	vector<CPlayer*>		m_pPlayerVec;
	CPlayer*				m_pCaptinPlayer = nullptr;


public:
	virtual void Free()override;

	
};

END


