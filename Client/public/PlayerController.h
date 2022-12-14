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
	virtual HRESULT Initialize();
	
public: /* For.Dungeon*/
	CPlayer*	Get_Captin() { return m_pCaptinPlayer;}
	void		Set_CaptinPlayer();
	void		SyncAninmation();
	void		Change_Scene(_uint	iLevelIndex);
	
public: /*For.Combat*/
	CGameObject*		Get_CurActor()const { return pCurentActor; }


	_bool			CombatScene(void);
	void			CurrentTurn_AnimControl();
	
	/* 조건들 나중에 현재 순서에맞는 애들을 채워야함*/
	_bool			To_Idle();
	_bool			To_Intro();
	_bool			To_Normal_Attack();
	_bool			To_Skill1_Attack();
	_bool			To_Skill2_Attack();
	_bool			To_Uitimate();
	_bool			To_Buff();
	_bool			To_WideAreaBuff();
	_bool			To_Use_Item();
	_bool			To_Defence();
	_bool			To_Light_Hit();
	_bool			To_Heavy_Hit();
	_bool			To_Flee();
	_bool			To_Die();
	_bool			To_Viroty();

private:
	_uint					m_iCurrentScene = 0;
	vector<CPlayer*> m_pPlayerVec;
	CPlayer*		m_pCaptinPlayer = nullptr;

	vector<class CGameObject*> CombatActors;	// 전체적인 흐름담당
	class CGameObject*			pCurentActor = nullptr;		// 현재 순서인놈
	class CGameObject*			pHitActor = nullptr;		// 맞은놈




public:
	virtual void Free()override;

	
};

END


