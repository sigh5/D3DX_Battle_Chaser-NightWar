#pragma once
#include "CombatActors.h"
#include "BaseDelegater.h"

BEGIN(Engine)

class CFSMComponent;
class CNavigation;



class ENGINE_DLL CPlayer abstract: public CCombatActors
{
public:
	enum MAPTYPE { DUNGEON_PLAYER, COMBAT_PLAYER,  MAPTYPE_END };
	
public:
	typedef struct tag_PlayerDesc :GAMEOBJECTDESC
	{

	}PLAYERDESC;

protected:
	CPlayer(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	void	Set_ControlInput(_bool bControl) { m_bControlKeyInput = bControl; }

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public: /*For.PlayerController*/
	void				Set_FollowTarget(CGameObject* pPlayer);
	_bool				Get_IsSwap()const { return m_bIsSwap; }
	void				Set_IsSwap(_bool bIsSwap) { m_bIsSwap = bIsSwap; }
	virtual _uint		Get_AnimationIndex() { return 0; }
	void				SyncAnimation(_uint iAnimIndex) { m_iAnimIndex = iAnimIndex; }
	_bool				IsCaptin();
	void				Get_CaptinPlayer_Item(class CStatus* pStatus);


public: /*For.Combat*/
	virtual void				Use_HpPotion();
	virtual void				Use_MpPotion();
	virtual _int				Get_RestHpPotion();
	virtual _int				Get_RestMpPotion();
	virtual void				Boss_Ultimate_Anim();

protected:
	_bool				KeyInput(_double TimeDelta, CNavigation* pNavigation);
	virtual void		Change_Level_Data(_uint iLevleIdx) {}

protected: /*For.Tick*/
	virtual void				Dungeon_Tick(_double TimeDelta) {};
	virtual void				Combat_Tick(_double TimeDelta) {};

protected: /*For.Dungeon*/
	virtual   void		AnimMove() {}
	virtual	  void		HighLightChar() {}
	virtual	  void		NormalLightCharUI() {}
	
public: /*For.Imgui*/
	virtual _bool Piciking_GameObject() { return false; }

public:
	void	LookAtTarget(_double TimeDelta,class CNavigation* pNavigation);

protected:
	PLAYERDESC						m_PlayerDesc;
	MAPTYPE							m_ePlayerType = DUNGEON_PLAYER;
	_bool							m_bControlKeyInput = false;
	CGameObject*					m_pCaptinPlayer = nullptr;	//Dungeon
	_bool							m_bIsHeavyHit = false;
	_bool							m_bBuffEffectStop = false;
private:
	_double					m_fWalkTime = 0.f;
	_float					m_fMoveSpeedRatio = 0.f;
	_bool					m_bKeyInput = false;
	_bool					m_bIsSwap = false;
	_bool					m_isStop = false;
	_bool					m_bIsWalk = false;


	
public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;


};

END