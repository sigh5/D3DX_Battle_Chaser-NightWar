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
	//vector<CGameObject*>	m_CombatActors;	// ��ü���� �帧���
	map<const wstring,  CGameObject*> m_CurActorMap;

	CGameObject*			m_pCurentActor = nullptr;		// ���� �����γ�
	CGameObject*			m_pHitActor =	nullptr;		// ������

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