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
	void	Refresh_CurActor() { Set_CurrentActor(); }


	_bool			Test();


public:	/* ���� ������ ���¸� �����ϱ����� �Լ��� */
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
	CGameInstance*			m_pGameInstace = nullptr;
	vector<CCanvas*>		m_CanvasVec;	// CombatScene��ü�� ĵ����
	//vector<CGameObject*>	m_CombatActors;	// ��ü���� �帧���
	map<const wstring,  CGameObject*> m_CurActorMap;

	CGameObject*			m_pCurentActor = nullptr;		// ���� �����γ�
	CGameObject*			m_pHitActor =	nullptr;		// ������


private:
	_bool					m_bCombatIntro = false;
	_bool					m_bLateInit = false;
	_double					m_dIntroTimer = 0.0;

private:
	HRESULT					Set_CurrentActor();
	CGameObject*			Find_CurActor(const wstring& pNameTag);


public:
	virtual void	Free()override;

public:
	BaseDelegater<_double, _uint> m_CurActorDelegator;


#ifndef DEBUG
public: /*For_Imgui*/
	void	Imgui_CharAnim();
#endif // !DEBUG


};

END