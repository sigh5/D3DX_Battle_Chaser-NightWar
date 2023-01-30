#pragma once

#include "Client_Defines.h"
#include "Monster.h"
#include "HitBoxObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CStatus;
class CGameInstance;
END

BEGIN(Client)
class CSkeleton_Naked final :public CMonster
{
private:
	CSkeleton_Naked(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CSkeleton_Naked(const CSkeleton_Naked& rhs);
	virtual ~CSkeleton_Naked() = default;

public:
	virtual class CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual		_bool	Calculator_HitColl(CGameObject* pWeapon);	//무기 아니면 스킬구체
	virtual		_bool	IsCollMouse()override;
	virtual	  _bool	  Is_Dead()override;
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual	void	Fsm_Exit()override;
	void			UltiHeavyHitExit();

	void			Combat_Tick(_double TimeDelta);
	virtual	  void  Combat_DeadTick(_double TimeDelta)override;
	_int			Is_MovingAnim();
	void			CombatAnim_Move(_double TImeDelta);
	void			MovingAnimControl(_double TimeDelta);
	void			Anim_Frame_Create_Control();

public:		/*Create_EFfect*/
	virtual void	Create_Hit_Effect()override;
	virtual void	Create_Heavy_Hit_Effect()override;
	void			Multi_Hit_Effect(class CGameInstance*pInstance);
	void			Create_Move_Target_Effect();
	void			Create_BuffEffect();
public: /*For.Imgui*/
	void				Create_Test_Effect();		// Test

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
	virtual		void		Calculator_HitDamage()override;	/*충돌시 함수*/
	

public:
	void					Anim_Idle();
	void					Anim_Intro();
	void					Anim_NormalAttack();
	void					Anim_Skill1_Attack();
	void					Anim_Defence();
	void					Anim_Buff();
	void					Anim_Light_Hit();
	void					Anim_Heavy_Hit();
	void					Anim_Die();
	void					Anim_Viroty();


private:
	vector<CGameObject*>			m_MonsterParts;
	

	
private:
	_int					m_iMovingDir = ANIM_EMD;
	_float					m_SpeedRatio = 6.f;
	_float					m_LimitDistance = 6.f;
	_float					m_ReturnDistance = 0.1f;
	_float					m_setTickForSecond = 1.f;
	_uint					m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_NONE;
	_bool					m_bChange_hit_Effect = false;
	_bool					m_bRun = false;
	_uint					m_iTurnCanvasOption = 0;		// 0이면 턴끝남 1이면 죽음
	_float					m_fHitPerSecond = 1.f;
	_int					m_iSign = 1;
	_bool					m_bClearScene = false;

	WeaponType				m_eWeaponType = WEAPON_SWORD;
	UI_REPRESENT	m_Represnt = REPRESENT_SKELTON_NAKED;


private: /*For.Imgui*/
	wstring			m_TextureTag = TEXT("");
	_float3			m_vSkill_Scale;
	_float4			m_vSkill_Pos;
	_float3			m_vTestPos;
	_float3			m_vTestScale;


public:
	BaseDelegater<UI_REPRESENT, _uint> m_Monster_CombatTurnDelegeter;	// 턴제
	
public:
	static CSkeleton_Naked* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};
END
