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

class CInventory;
class CDungeon_Canvas;

class CPlayerController final :public CBase
{
	DECLARE_SINGLETON(CPlayerController);

private:
	CPlayerController() {}
	virtual ~CPlayerController() = default;

public:
	vector<CPlayer*>& Get_Players() { return m_pPlayerVec; }

	void	Reset_LateInit() { m_bLateInit = false; }

public:
	virtual HRESULT Initialize(_uint iLevel);

	HRESULT			Late_Initialize();


	void			Player_Controll_Tick(_double TimeDelta);



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
	vector<CPlayer*>		m_pPlayerVec;
	CPlayer*				m_pCaptinPlayer = nullptr;
	
	CInventory*				m_pInventory = nullptr;
	CDungeon_Canvas*		m_pDungeonCanvas = nullptr;


private:
	_uint					m_iCurrentScene = 0;
	_bool					m_bInventoryRender = false;
	_bool					m_bLateInit = false;

public:
	virtual void Free()override;

	
};

END


