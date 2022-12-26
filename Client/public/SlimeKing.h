#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CStatus;
END

BEGIN(Client)

/*
	내일 할 것 : 몬스터 애니메이션 다맞추기 
	맞춘 애니메이션으로 턴제 로직짜기		
*/

class CSlimeKing final :public CMonster
{
private:
	CSlimeKing(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CSlimeKing(const CSlimeKing& rhs);
	virtual ~CSlimeKing() = default;

public:
	virtual		_bool	IsCollMouse()override;
	virtual class CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual		_bool	Calculator_HitColl(CGameObject* pWeapon);	//무기 아니면 스킬구체

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual	void	Fsm_Exit()override;
	void			Combat_Tick(_double TimeDelta);
	_int			Is_MovingAnim();
	void			CombatAnim_Move(_double TImeDelta);
	void			MovingAnimControl(_double TimeDelta);
	
	



private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CStatus*				m_pStatusCom = nullptr;
	class CMonsterFsm*			m_pFsmCom = nullptr;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					Ready_Parts();


public:
	void					Anim_Idle();
	void					Anim_Intro();
	void					AnimNormalAttack();
	void					Anim_Skill1_Attack();
	void					Anim_Uitimate();
	void					Anim_Buff();
	void					Anim_Light_Hit();
	void					Anim_Heavy_Hit();
	void					Anim_Die();
	void					Anim_Viroty();

private:
	_int					m_iMovingDir = ANIM_EMD;
	_float					m_SpeedRatio = 6.f;
	_float					m_LimitDistance = 6.f;
	_float					m_ReturnDistance = 0.1f;
	_float					m_setTickForSecond = 1.f;

public:
	BaseDelegater<_double, _uint> m_Monster_CombatTurnDelegeter;	// 턴제

private:
	vector<CGameObject*>	m_MonsterParts;
	WeaponType		m_eWeaponType = WEAPON_HEAD;


public:
	static CSlimeKing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END