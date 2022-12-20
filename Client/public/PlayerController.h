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
	virtual HRESULT Initialize(_uint iLevel);
	
public: /* For.Dungeon*/
	CPlayer*	Get_Captin() { return m_pCaptinPlayer;}
	void		Set_CaptinPlayer();
	void		SyncAninmation();
	void		Change_Scene(_uint	iLevelIndex);
	
public: /*For.Combat*/
	_bool			CombatScene(void);
	//void			CurrentTurn_AnimControl();
	
	/* ���ǵ� ���߿� ���� �������´� �ֵ��� ä������*/
	

private:
	_uint					m_iCurrentScene = 0;
	vector<CPlayer*>		m_pPlayerVec;
	CPlayer*				m_pCaptinPlayer = nullptr;


public:
	virtual void Free()override;

	
};

END


