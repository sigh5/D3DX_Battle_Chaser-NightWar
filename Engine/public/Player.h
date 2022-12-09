#pragma once
#include "GameObject.h"
#include "BaseDelegater.h"

BEGIN(Engine)

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

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	
public: /*For.PlayerController*/
	virtual  _vector	Get_CameraBoneVector() { return _vector(); }
	void	  Set_FollowTarget(CGameObject* pPlayer);
	


public: /*For.Dungeon*/
	virtual _uint	Get_AnimationIndex() { return 0; }
	void	SyncAnimation(_uint iAnimIndex) { m_iAnimIndex = iAnimIndex; }
	void	LookAtTarget();
	_bool	Get_IsSwap()const { return m_bIsSwap; }
	void	Set_IsSwap(_bool bIsSwap) { m_bIsSwap = bIsSwap; }

protected: /*For.Dungeon*/
	_bool				IsCaptin();
	virtual   void		AnimMove() {}
	virtual	  void		HighLightChar() {}
	virtual	  void		NormalLightCharUI() {}

protected:
	_bool		KeyInput(_double TimeDelta);
	
public: /* imgui */
	virtual _bool Piciking_GameObject()override { return false; }

private:
	void	ChaseTarget(_double TimeDelta);

protected:
	PLAYERDESC				m_PlayerDesc;
	MAPTYPE					m_ePlayerType = DUNGEON_PLAYER;
	
protected:
	_bool					m_bControlKeyInput = false;
	_uint					m_iAnimIndex = 0;

private:
	_double					m_fWalkTime = 0.f;
	_float					m_fMoveSpeedRatio = 0.f;
	_bool					m_bKeyInput = false;
	CGameObject*			m_pTargetPlayer = nullptr;
	_bool					m_bIsSwap = false;
	_bool					m_isStop = false;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;


};

END