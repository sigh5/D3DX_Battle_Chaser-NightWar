#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "BaseDelegater.h"

/* 
	Combat_Level�� ��ü���� ������
	���Ϳ� �÷��̾��� ����(���ϱ�) 
	UI����� ��ȣ�ۿ�� ����
	��� �������� �̰����� �̷������.
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
	
	 CGameObject*		Get_HitActor()const { return m_pHitActor; }

	 void				Set_CombatIntro(_bool bIsIntro) { m_bCombatIntro = bIsIntro;}
	 void				Set_MonsterSetTarget(_bool bSetTaget) { m_bMonsterSelect_Target = bSetTaget; }


	 map<const wstring, CGameObject*>* Get_CurActorMap() { return &m_CurActorMap; }

public:	/*For.Scene_Change*/
	void				Scene_Chane_Safe_Release();
	void				Reset_Timer();

public:  /*Combat Logic*/
	HRESULT Initialize(_uint iLevel);
	HRESULT Late_Init();
	void	CurrentTurn_ActorControl(_double TimeDelta);		//Tick
	void	Status_CanvasInit();
	void	PlayerWin();

public:
	void	Refresh_CurActor();
	void	Mana_Refresh();
public:
	void	Active_Fsm();
	void	ResetState();
	void	Render_StopCanvas();
	void	Camera_Shaking();
	void	Camera_Zoom_Out();
	void	Camera_Zoom_In();
	void	UI_Shaking(_bool bShaking);


private:
	void	PickingTarget();		//�÷��̾��� ���϶�
	void	MonsterSetTarget();
	
	void	Collison_Event();
	void	Cal_HitPlayerTarget();

	void	Set_Player_StateCanvas();

	// �� ���� ���� �� ���� ����

public:	/* ���� ������ ���¸� �����ϱ����� �Լ��� */
	_bool		To_Idle();				// �ִ� �� �ָ���
	_bool		To_Intro();				//�̳� ����־����
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
	vector<CCanvas*>		m_CanvasVec;	// CombatScene��ü�� ĵ����

	map<const wstring,  CGameObject*> m_CurActorMap;
	map<const wstring, CStatus*>	  m_ActorsStatusMap;


	CGameObject*			m_pCurentActor	= nullptr;		// ���� �����γ�
	CGameObject*			m_pHitActor			=		nullptr;		// ������
	class CCamera_Combat*			m_pCombatCamera = nullptr;


	class CTurnUICanvas*	m_pTurnCanvas = nullptr;
	class CTurnStateCanvas*	m_pTurnStateButtonCanvas = nullptr;


private:
	_bool					m_bCombatIntro = false;
	_bool					m_bLateInit = false;
	_double					m_dIntroTimer = 0.0;

	_bool					m_bIsHiterhit = false;
	_bool					m_bisHitTimer_Alive = false;

	_float					m_fHitTimer = 0.0f;
	_float					m_fHitRecoverTime =0.03f;		// ��� ���Ӱ����϶� Ư���� ��ġ�� �ʿ�

	_bool					m_bMonsterSelect_Target = false;			
	_uint					m_iMonster_Player_Option=0;
	_bool					m_bIsPlayer = false;
	_bool					m_bMonsterTurnEnd = false;

	_uint					m_iHitNum = 0;
	_bool					m_bVirtory = false;
	
	_bool					m_bCanvasRenderCheck = false;
	_float					m_fAfter_DefenceButtonCheck = 0.f;
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