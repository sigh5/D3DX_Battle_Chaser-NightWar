#pragma once
#include "GameObject.h"
#include "BaseDelegater.h"

BEGIN(Engine)

class CFSMComponent;
class CNavigation;

class ENGINE_DLL CPlayer abstract: public CGameObject
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
	const _bool	Get_IsIdle()const { return m_bIsIdle; }
public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public: /*For.PlayerController*/
	void				 Set_FollowTarget(CGameObject* pPlayer);
	_bool				Get_IsSwap()const { return m_bIsSwap; }
	void				Set_IsSwap(_bool bIsSwap) { m_bIsSwap = bIsSwap; }
	virtual _uint		Get_AnimationIndex() { return 0; }
	void				SyncAnimation(_uint iAnimIndex) { m_iAnimIndex = iAnimIndex; }
	_bool				IsCaptin();
	virtual				 _bool	  Is_PlayerDead() { return false; }
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
	
public: /*For.Animamtion*/
	void	  CurAnimQueue_Play_Tick(_double Time, class CModel* pModel);
	void	  CurAnimQueue_Play_LateTick(class CModel* pModel);
	void	  Set_CombatAnim_Index(class CModel* pModel);
	
public: /*For.Imgui*/
	virtual _bool Piciking_GameObject() { return false; }

private:
	void	ChaseTarget(_double TimeDelta);
	void	LookAtTarget(_double TimeDelta);

protected:
	PLAYERDESC						m_PlayerDesc;
	MAPTYPE							m_ePlayerType = DUNGEON_PLAYER;
	_bool							m_bControlKeyInput = false;
	
	

protected:
	_uint							m_bFinishOption = 0;
	_uint							m_iOldAnim = 0;
	_uint							m_iAnimIndex = 0;
	_bool							m_bIsCombatScene = false;
	_bool							m_bIsCombatLastInit = false;
	_bool							m_bIsCombatAndAnimSequnce = false;
	queue<pair<_uint, _double>>		m_CurAnimqeue;
	_bool							m_bIsIdle = false;;

	_double							m_NormalTikcPerSecond=1.0;
	_double							m_IntroTimer = 0.7;

private:
	_double					m_fWalkTime = 0.f;
	_float					m_fMoveSpeedRatio = 0.f;
	_bool					m_bKeyInput = false;
	CGameObject*			m_pTargetPlayer = nullptr;
	_bool					m_bIsSwap = false;
	_bool					m_isStop = false;
	_bool					m_bIsWalk = false;



public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;


};

END