#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
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
	virtual HRESULT Initialize();
	
public: /* For.Dungeon*/
	CPlayer*	Get_Captin() { return m_pCaptinPlayer;}
	void		Set_CaptinPlayer();
	void		SyncAninmation();
	void		Change_Scene(_uint	iLevelIndex);
	
public: /*For.Combat*/
	_bool			CombatScene(void);
	void			CurrentTurn_AnimControl();

private:
	vector<CPlayer*> m_pPlayerVec;
	CPlayer*		m_pCaptinPlayer = nullptr;

	vector<class CGameObject*> CombatActors;	// 전체적인 흐름담당
	class CGameObject*			pCurentActor = nullptr;
	_uint					m_iCurrentScene = 0;

public:
	virtual void Free()override;

	
};

END


