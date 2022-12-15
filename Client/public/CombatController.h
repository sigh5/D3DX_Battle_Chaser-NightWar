#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "BaseDelegater.h"

/* 
	Combat_Level의 전체적인 턴적용
	몬스터와 플레이어의 순서(정하기) 
	UI들과의 상호작용시 연락
	모든 로직들은 이곳에서 이루어진다.
*/	

BEGIN(Engine)

class CGameInstance;
class CGameObject;
class CPlayer;
class CMonster;
class CCanvas;
END

BEGIN(Client)

class CCombatController final :public CBase
{
	DECLARE_SINGLETON(CCombatController);

public:
	CCombatController();
	virtual ~CCombatController() = default;
public:
	 CGameObject*		Get_CurActor()const { return m_pCurentActor; }
	
	 const	_bool		Get_CombatIntro()const { return m_bCombatIntro; }
	 void				Set_CombatIntro(_bool bIsIntro) { m_bCombatIntro = bIsIntro;}
	
public:  /*Combat Logic*/
	HRESULT Initialize(_uint iLevel);
	HRESULT Late_Init();
	void	CurrentTurn_ActorControl(_double TimeDelta);		//Tick
	
public:
	void	Refresh_CurActor();

public:
	void	Active_Fsm();
	void	ResetState();



public:	/* 현재 액터의 상태를 제어하기위한 함수들 */
	_bool		To_Idle();				// 애는 좀 애매함
	_bool		To_Intro();				//이놈만 살아있어야함
	void		To_Normal_Attack();
	void		To_Skill1_Attack();
	void		To_Skill2_Attack();
	void		To_Uitimate();
	void		To_Buff();
	void		To_WideAreaBuff();
	void		To_Use_Item();
	void		To_Defence();
	void		To_Light_Hit();
	void		To_Heavy_Hit();
	void		To_Flee();
	void		To_Die();
	void		To_Viroty();

private:
	CGameInstance*			m_pGameInstace = nullptr;
	vector<CCanvas*>		m_CanvasVec;	// CombatScene전체의 캔버스
	//vector<CGameObject*>	m_CombatActors;	// 전체적인 흐름담당
	map<const wstring,  CGameObject*> m_CurActorMap;

	CGameObject*			m_pCurentActor = nullptr;		// 현재 순서인놈
	CGameObject*			m_pHitActor =	nullptr;		// 맞은놈

	class CTurnUICanvas*	m_pTurnCanvas = nullptr;

private:
	_bool					m_bCombatIntro = false;
	_bool					m_bLateInit = false;
	_double					m_dIntroTimer = 0.0;

private:
	HRESULT					Set_CurrentActor();
	CGameObject*			Find_CurActor(const wstring& pNameTag);


public:
	virtual void	Free()override;


#ifndef DEBUG
public: /*For_Imgui*/
	void	Imgui_CharAnim();
#endif // !DEBUG


};

END