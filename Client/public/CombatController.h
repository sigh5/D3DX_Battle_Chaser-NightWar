#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "BaseDelegater.h"
#include "Status.h"
/* 
	Combat_Level의 전체적인 턴적용
	몬스터와 플레이어의 순서(정하기) 
	UI들과의 상호작용시 연락
	모든 로직들은 이곳에서 이루어진다.
*/	

BEGIN(Engine)

class CGameInstance;
class CombatActors;
class CGameObject;
class CPlayer;
class CMonster;
class CCanvas;
class CStatus;
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
	 CStatus*			Get_CurActorStatus();


	 const _bool		Get_Late_Init()const { return m_bLateInit; }

	 CGameObject*		Get_HitActor()const { return m_pHitActor; }

	 void				Set_CombatIntro(_bool bIsIntro) { m_bCombatIntro = bIsIntro;}
	 void				Set_MonsterSetTarget(_bool bSetTaget) { m_bMonsterSelect_Target = bSetTaget; }
	 void				Set_Ultimate_End(_bool bUltimateEnd) { m_bCurActorUltimateEnd = bUltimateEnd; }


	 void				Setting_Win_Canvas(CCanvas* pCanvas);
	 void				Set_MouseTextureOption(_int iMouseTextureNum);
/*for_Tool*/
	 CGameObject*				Get_Player(wstring pName);


	 map<const wstring, CGameObject*>* Get_CurActorMap() { return &m_CurActorMap; }

public:	/*For.Scene_Change*/
	void				Scene_Chane_Safe_Release();
	void				Reset_Timer();

public:  /*Combat Logic*/
	HRESULT Initialize(_uint iLevel);
	HRESULT Late_Init();
	void	CurrentTurn_ActorControl(_double TimeDelta);		//Tick
	void	Status_CanvasInit();
	void	Late_Tick(_double TimeDelta);
	void	Active_Fsm();


	void	Load_CamBG2();
	void	Wide_Debuff(_bool bIsPlayer, CStatus::DEBUFFTYPE eDebuffOption);


	void	HPMp_Update(CGameObject* pHiter);



private:
	void	PickingTarget();		//플레이어의 
	void	MouseChangeOn_Target();		//플레이어의 턴일때
	void	MonsterSetTarget(_double TimeDelta);

public:	/*Reset*/
	void	Refresh_CurActor();
	void	Mana_Refresh();
	void	ResetState();
	void	Set_Player_StateCanvas();
	/*Win*/
	void	PlayerWin();	
	void	Render_StopCanvas();
	
	/*Camera*/
	void	Camera_Shaking();
	void	Camera_ShortShaking();
	void	Camera_Zoom_Out();
	void	Camera_Zoom_In();
	void	UI_Shaking(_bool bShaking);
	void	Wide_Attack(_bool IsPlayer, _int iDamage);
	void	Collison_Event();
	void	Cal_HitPlayerTarget();

public:		/*플레이어턴 + 얼티메이트 */
	void	Ultimate_LateTick(_double TimeDelta);

private:
	void	Ultimate_Start_LateTick(_double TimeDelta);
	void	Ultimate_Timedelta_Tick(_double TimeDelta);
	void	Ultimate_End_LateTick(_double TimeDelta);



public:
	void	Ultimate_Camera_On();
	void	Ultimate_Camera_Off();
	




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
	class   CDamage_Font_Manager*	m_pFontManager = nullptr;
	class   CExplain_FontMgr*		m_pExplainFontManager = nullptr;
	class	CMouse*					m_pMouse = nullptr;
private:
	vector<CCanvas*>		m_CanvasVec;	// CombatScene전체의 캔버스
	//vector<CCanvas*>		m_CombatMapVec;
	map<const wstring,  CGameObject*>     m_CurActorMap;
	map<const wstring, CStatus*>		  m_ActorsStatusMap;




private:
	CGameObject*			m_pCurentActor	= nullptr;		// 현재 순서인놈
	CGameObject*			m_pHitActor			=		nullptr;		// 맞은놈
	class CCamera_Combat*			m_pCombatCamera = nullptr;
	class CTurnUICanvas*	m_pTurnCanvas = nullptr;
	class CTurnStateCanvas*	m_pTurnStateButtonCanvas = nullptr;
	class CMyImage*			m_pCurBannerImage = nullptr;
	class CGameObject*		m_pCombatBG = nullptr;
	class CGameObject*		m_pCombatBG2 = nullptr;

private:
	_bool					m_bCombatIntro = false;
	_bool					m_bLateInit = false;
	_double					m_dIntroTimer = 0.0;

	_bool					m_bIsHiterhit = false;
	_bool					m_bisHitTimer_Alive = false;

	_float					m_fHitTimer = 0.0f;
	_float					m_fHitRecoverTime =0.02f;		// 모든 연속공격일때 특단의 조치가 필요

	_bool					m_bMonsterSelect_Target = false;			
	_uint					m_iMonster_Player_Option=0;
	_bool					m_bIsPlayer = false;
	_bool					m_bMonsterTurnEnd = false;

	_uint					m_iHitNum = 0;
	_bool					m_bVirtory = false;
	
	_bool					m_bCanvasRenderCheck = false;
	_float					m_fAfter_DefenceButtonCheck = 0.f;
	
	_bool					m_bCurActorUltimateUse = false;
	_float					m_fTimeSlowRatio = 0.5f;
	_bool					m_bCurActorUltimateEnd = false;
	_bool					m_bBannerClose = false;

	
	_bool					m_bMonsterUseUltimate = false;
	
	_uint					m_iFsmOption = 0;


private:
	HRESULT					Set_CurrentActor();
	HRESULT					Set_ActorsStatus();
	
	CGameObject*			Find_CurActor(const wstring& pNameTag);
	CStatus*				Find_CurStatus(const wstring& pNameTag);


public:
	virtual void	Free()override;


#ifdef _DEBUG
public: /*For_Imgui*/
	void	Imgui_CharAnim();
#endif // !DEBUG


};

END