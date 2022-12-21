#pragma once

#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CHero_Calibretto final :public CPlayer
{
protected:
	CHero_Calibretto(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHero_Calibretto(const CHero_Calibretto& rhs);
	virtual ~CHero_Calibretto() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();



public: /*For.SceneChange*/
	virtual void		Change_Level_Data(_uint iLevleIdx)override;

public: /*For.Dungeon*/
	virtual _uint		Get_AnimationIndex() override;
	virtual void		AnimMove()override;
	virtual	  void		HighLightChar()override;
	virtual	  void		NormalLightCharUI()override;
	virtual	  void		Dungeon_Tick(_double TimeDelta)override;;

public:  /*For.Combat*/
	HRESULT			  Combat_Initialize();
	virtual	  void	  Combat_Tick(_double TimeDelta)override;
	virtual	  _bool	  Is_PlayerDead()override;
	
	_int	  Is_MovingAnim();
	void	  CombatAnim_Move(_double TImeDelta);
	void	  MovingAnimControl(_double TimeDelta);


	virtual		void	Fsm_Exit()override;
	void				Intro_Exit();
	
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	class CAnimFsm*			m_pFsmCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
private:
	_bool					m_bCombat_LastInit = false;
	_int					bResult = ANIM_EMD;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

public:
	void					Anim_Idle();
	void					Anim_Intro();
	void					AnimNormalAttack();
	void					Anim_Skill1_Attack();
	void					Anim_Skill2_Attack();
	void					Anim_Uitimate();
	void					Anim_Buff();
	void					Anim_WideAreaBuff();
	void					Anim_Use_Item();
	void					Anim_Light_Hit();
	void					Anim_Heavy_Hit();
	void					Anim_Flee();
	void					Anim_Die();
	void					Anim_Viroty();


private:
	_uint iTestNum = 0;
	_double TEst = 0.0;
	_vector		m_vOriginPos;				// for.Combat

	_uint	m_iNonRenderMeshIndex = 0;	// 0~7번까지 안그려야됌

public:
	static CHero_Calibretto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

public:
	BaseDelegater<HIGHLIGHT_UIDESC> m_Hero_DungeonUIDelegeter;
	
	BaseDelegater<_double, _uint> m_Hero_CombatTurnDelegeter;			//옆에 턴 넘기는것
	BaseDelegater<_bool> m_Hero_CombatStateCanvasDelegeter;	// 밑에 상태캔버스 키는것


private:
	_bool		m_bCombatChaseTarget = false;


	_float		m_SpeedRatio = 8.f;
	_float		m_LimitDistance = 12.f;
	_float		m_ReturnDistance = 0.1f;
	_float		m_setTickForSecond = 0.9f;

};

END
