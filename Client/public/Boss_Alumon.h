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
END

BEGIN(Client)
class CBoss_Alumon final : public CMonster
{
private:
	CBoss_Alumon(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CBoss_Alumon(const CBoss_Alumon& rhs);
	virtual ~CBoss_Alumon() = default;

public:
	virtual class CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual		 _bool			 Calculator_HitColl(CGameObject* pWeapon);	//무기 아니면 스킬구체
	virtual		 _bool			 IsCollMouse()override;
	virtual		 _bool			 Is_Dead()override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth()override;
	virtual void	Initialize_CombatSound()override;

public:
	virtual	void	Fsm_Exit()override;
	void			UltiHeavyHitExit();
	void			Combat_Tick(_double TimeDelta);
	void			Combat_UltimateTick(_double TimeDelta);
	void			CombatAnim_Move_Ultimate(_double TimeDelta);
	_int			Is_MovingAnim();
	void			CombatAnim_Move(_double TImeDelta);
	void			MovingAnimControl(_double TimeDelta);
	void			Anim_Frame_Create_Control();
	virtual	  void  Combat_DeadTick(_double TimeDelta)override;
	void			Fsm_UltimateOne_Exit();



public:		/*Create_EFfect*/
	virtual void		Create_Hit_Effect()override;
	virtual void		Create_Heavy_Hit_Effect()override;
	void				Create_Ultimate_StartCam_Effect();
	void				Create_Move_Target_Effect();
	void				Create_BuffEffect();
	void				Create_Skill_UltimateTwo_FirstEffect();
	void				Create_Skill_UltimateTwo_Effect();
	void				Create_Skill_Ultimate_Effect();
	void				Create_Test_MeshEffect();
	void				Create_Ultimate_StartFog_CamEffect();
	void			    Create_Whip_Trail();
public: /*For.Imgui*/
	void				Create_Test_Effect();		// Test


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CStatus*				m_pStatusCom = nullptr;
	class CBossFsm*			m_pFsmCom = nullptr;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					Ready_Parts();

private:
	virtual		void		Calculator_HitDamage()override;	/*충돌시 함수*/

public:
	void					Anim_Idle();
	void					Anim_Intro();		//0
	void					Anim_NormalAttack(); //0
	void					Anim_Skill1_Attack();
	void					Anim_Skill2_Attack();
	void					Anim_Uitimate();//0
	void					Anim_Defence();//0
	void					Anim_Buff();  //0
	void					Anim_Uitimate2(); //Brust_One(Anim_Uitimate2)

	void					Anim_NormalAttack2();  //NormalAttack2 (UseItem)

	void					Anim_Light_Hit(); //0
	void					Anim_Heavy_Hit(); //0
	void					Anim_Die(); //0
	void					Anim_Viroty();

private:
	vector<CGameObject*>			m_BossParts;
	vector<CGameObject*>			m_pEffectParts;
	CGameObject*					m_pFog = nullptr;
	CGameObject*					m_pTrailEffect = nullptr;

	//CGameObject*					m_pCrackObject = nullptr;
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


	_bool					m_bStunUltimate = false;
	_float					m_bStun_UltimateTimer = 0.f;

	_float					m_bUltimateOneMoveTimer = 0.f;

	// Ultimate
	_bool					m_bUltimateBuffRenderStop = false;
	_bool					m_bUltimateCam = false;
	_bool					m_bUltimateRun = false;
	_bool					m_bUltiWideAttack = false;
	_bool					m_bUltiAttackStart = false;
	_bool					m_bUltiAttackStop = false;
	_bool					m_bUltimateFirstEffect = false;

	_int					m_iFrameCnt = 0;
	_int					m_iMaxCnt = 0;
	_bool					m_bUltimate_AttackEffect = false;
	_float					m_fUltimateTimer = 0.f;
	_bool					m_bAttackColliderOn = false;
	

	WeaponType				m_eWeaponType = WEAPON_HEAD;
	UI_REPRESENT			m_Represnt = REPRESENT_REAL_BOSS;


	_bool					m_bUltimateSoundCheck = false;
	_float					m_fUltimateSoundTimer = 0.f;
	_bool					m_bMaintainEffectErase = false;

	/*Trail*/
	_bool			m_bTraillEffectStartCheck = false;
	_bool			m_bTrailEndCheck = false;

private: /*For.Imgui*/
	wstring			m_TextureTag = TEXT("");
	_float3			m_vSkill_Scale;
	_float4			m_vSkill_Pos;
	_float3			m_vTestPos;
	_float3			m_vTestScale;
	string			m_BoneTag = "";
public:
	BaseDelegater<UI_REPRESENT, _uint> m_Monster_CombatTurnDelegeter;	// 턴제

public:
	static CBoss_Alumon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END