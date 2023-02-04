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
	virtual class CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual		_bool	Calculator_HitColl(CGameObject* pWeapon);	//무기 아니면 스킬구체
	virtual		_bool	IsCollMouse()override;
	virtual		_bool	Is_Dead()override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void				Initialize_CombatSound()override;
public:
	virtual	void	Fsm_Exit()override;
	void			UltiHeavyHitExit();
	void			Combat_Tick(_double TimeDelta);
	_int			Is_MovingAnim();
	void			CombatAnim_Move(_double TImeDelta);
	void			MovingAnimControl(_double TimeDelta);
	void			Anim_Frame_Create_Control();
	virtual	  void  Combat_DeadTick(_double TimeDelta)override;

public:		/*Create_EFfect*/
	virtual void		Create_Hit_Effect()override;
	virtual void		Create_Heavy_Hit_Effect()override;
	void				Create_Ultimate_StartCam_Effect();
	void				Create_Move_Target_Effect();
	void				Create_BuffEffect();

	void				Create_Skill_Ultimate_Effect0();
	void				Create_Skill_Ultimate_Effect1();
	void				Create_Skill_Ultimate_Effect2();

public: /*For.Imgui*/
	void				Create_Test_Effect();		// Test
	void				Create_Test_MeshEffect();

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
	void					AnimNormalAttack();
	void					Anim_Skill1_Attack();
	void					Anim_Uitimate();
	void					Anim_Buff();
	void					Anim_Light_Hit();
	void					Anim_Heavy_Hit();
	void					Anim_Die();
	void					Anim_Viroty();


private:
	vector<CGameObject*>	m_MonsterParts;
	list<CGameObject*>		m_UltimateBullet;
	CGameObject*			m_pCamEffectObj = nullptr;

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
	_bool					m_bSkill2UpPos = false;
	_bool					m_bSkill2_AttackEffect = false;
	_bool					m_bClearScene = false;
						
	
	// Ultimate
	_bool					m_bUltimateBuffRenderStop = false;
	_bool					m_bUltimateCam = false;
	_bool					m_bUltimateRun = false;
	_bool					m_bUltiWideAttack = false;
	_bool					m_bUltiAttackStart = false;
	_bool					m_bUltiAttackStop = false;
	_float					m_fCreatePosX = 1.5f;
	_float					m_fCreatePosY = 2.2f;
	_float					m_fCreatePosZ = 2.0f;
	_float					m_fCreateScale = 0.5f;

	_int					m_iFrameCnt = 0;
	_int					m_iMaxCnt = 0;
	_bool					m_bUltimate_AttackEffect = false;
	_float					m_fUltimateTimer = 0.f;



	WeaponType				m_eWeaponType = WEAPON_HEAD;
	UI_REPRESENT			m_Represnt = REPRESENT_SLIMEKING;


	_bool					m_bUltimateSoundCheck = false;
	_float					m_fUltimateSoundTimer = 0.f;


private: /*For.Imgui*/
	wstring			m_TextureTag = TEXT("");
	_float3			m_vSkill_Scale;
	_float4			m_vSkill_Pos;
	_float3			m_vTestPos;
	_float3			m_vTestScale;

public:
	BaseDelegater<UI_REPRESENT, _uint> m_Monster_CombatTurnDelegeter;	// 턴제

public:
	static CSlimeKing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END