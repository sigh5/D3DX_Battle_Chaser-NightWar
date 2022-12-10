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
	virtual HRESULT Initialize();
	


public: /* For.Dungeon*/
	CPlayer*	Get_Captin() { return m_pCaptinPlayer;}
	void		Set_CaptinPlayer();
	void		SyncAninmation();
	
	void		Change_Scene(_uint	iLevelIndex);
	//template<typename T> 
	//void	Matching_UIToPlayer(T* pUI);		// UI 와 플레이어를 이어주는함수


public:
	_bool			CombatScene();

public:
	vector<CPlayer*>& Get_Players() {return m_pPlayerVec;}

private:
	vector<CPlayer*> m_pPlayerVec;
	CPlayer*		m_pCaptinPlayer = nullptr;

	_uint			m_iCurrentScene = 0;

public:
	virtual void Free()override;


};

END