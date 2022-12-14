#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CGameObject;
class CPlayer;
END

/* 
	���� ����� ���������ʰ� 
	�÷��̾���� ��Ʈ���ϱ� ���� ��ü
	�÷��̾� ������ ��Ʈ�� �ϴ� ������Ѵ�.
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
	
	/* ���ǵ� ���߿� ���� �������´� �ֵ��� ä������*/
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

	vector<class CGameObject*> CombatActors;	// ��ü���� �帧���
	class CGameObject*			pCurentActor = nullptr;		// ���� �����γ�
	class CGameObject*			pHitActor = nullptr;		// ������




public:
	virtual void Free()override;

	
};

END


