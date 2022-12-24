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
class CSpider_Mana final : public CMonster
{
private:
	CSpider_Mana(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CSpider_Mana(const CSpider_Mana& rhs);
	virtual ~CSpider_Mana() = default;

public:
	virtual  CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual		_bool	Calculator_HitColl(CGameObject* pWeapon);
	virtual		_bool	IsCollMouse()override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void				Combat_Tick(_double TimeDelta);
	_int				Is_MovingAnim();
	void				CombatAnim_Move(_double TImeDelta);
	void				MovingAnimControl(_double TimeDelta);
	
	virtual		void	Fsm_Exit()override;
	


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CStatus*				m_pStatusCom = nullptr;
	class CMonsterFsm*		m_pFsmCom = nullptr;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					Ready_Parts();


private:
	_int					m_iMovingDir = ANIM_EMD;

public:
	void					Anim_Idle();
	void					Anim_Intro();
	void					Anim_NormalAttack();
	void					Anim_Skill1_Attack();
	void					Anim_Skill2_Attack();
	void					Anim_Buff();
	void					Anim_Light_Hit();
	void					Anim_Heavy_Hit();
	void					Anim_Die();
	void					Anim_Viroty();

public:
	BaseDelegater<_double, _uint> m_Monster_CombatTurnDelegeter;	// ≈œ¡¶

private:
	vector<CGameObject*>	m_MonsterParts;
	WeaponType		m_eWeaponType = WEAPON_END;

private:
	_float					m_SpeedRatio = 6.f;
	_float					m_LimitDistance = 6.f;
	_float					m_ReturnDistance = 0.1f;
	_float					m_setTickForSecond = 1.f;



public:
	static CSpider_Mana* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END