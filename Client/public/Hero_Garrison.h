#pragma once
#include "Client_Defines.h"
#include "Player.h"
#include "HitBoxObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CNavigation;
class CStatus;
END

BEGIN(Client)


class CHero_Garrison final : public CPlayer
{
	/* 모든 애니메이션의 끝은 IDLE로 고정이다. 따라서 필요가없다.*/
private:
	CHero_Garrison(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHero_Garrison(const CHero_Garrison& rhs);
	virtual ~CHero_Garrison() = default;

public:
	virtual class CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual		_bool	Calculator_HitColl(CGameObject* pWeapon)override;	//무기 아니면 스킬구체


	virtual _int				Get_RestHpPotion()override;
	virtual _int				Get_RestMpPotion()override;
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth()override;

public: /*For.SceneChange*/
	virtual void		Change_Level_Data(_uint iLevleIdx)override;

public: /*For.Dungeon*/
	virtual  _uint		Get_AnimationIndex() override;
	virtual   void		AnimMove()override;
	virtual	  void		HighLightChar()override;
	virtual	  void		NormalLightCharUI()override;
	virtual	  void		Dungeon_Tick(_double TimeDelta)override;;

public:  /*For.Combat*/
	virtual	  void		Combat_Tick(_double TimeDelta)override;
	virtual	  void		Combat_Ultimate(_double TimeDelta);
	virtual   void		Combat_DeadTick(_double TimeDelta)override;

	virtual   void		Combat_BlendAnimTick(_double TimeDelta);
	virtual	  void		Fsm_Exit()override;
	virtual	  void		Defence_Exit()override;
	void				MovingAnimControl(_double TimeDelta);
	virtual void		Create_Wide_Debuff(CStatus::DEBUFFTYPE eDebuffOption)override;
	virtual void		Initialize_CombatSound()override;
	void				Create_Trail_Object();


public:	 /*Create_Effect*/
	virtual void		Create_Hit_Effect()override;
	virtual void		Create_Defence_Effect_And_Action()override;
	void				Create_Normal_Attack_Effect();
	void				Create_Skill1_Attack_Effect();		// Test_Texture용 나중에 삭제바람
	void				Create_Skill2_Attack_Effect();
	void				Create_Ultimate_Effect();
	void				Create_Ultimate_End_Effect();
	void				Create_BuffEffect();
	void				Create_Move_Target_Effect();
	void				Create_Defence_Area();
	void				Create_Buff_MainTain_Effect();
	void				Create_Ultimate_StartCam_Effect();
	void			    Create_Ultimate_StartFog_CamEffect();
	void			    Create_FullScreenEffect();

	virtual void		Create_Heavy_Hit_Effect()override;
	virtual void		Boss_Ultimate_Anim()override;


public:
	void				Anim_Frame_Create_Control();
	virtual void	    Use_HpPotion()override;
	virtual void	    Use_MpPotion()override;

	virtual		void		Calculator_HitDamage();
public:
	virtual	  _bool	  Is_Dead()override;
	_int			  Is_MovingAnim();
	void			  CombatAnim_Move_Ultimate(_double TImeDelta);

	void			  Is_Skill1MovingAnim();
	void			  CombatAnim_Move(_double TImeDelta);
	void		      Ultimate_Anim_Frame_Control();

	
	void			 Create_Sword_Trail();

public:
	void				Anim_Idle();
	void				Anim_Intro();
	void				AnimNormalAttack();
	void				Anim_Defence();

	void				Anim_Skill1_Attack();
	void				Anim_Skill2_Attack();
	void				Anim_Uitimate();
	void				Anim_Buff();

	void				Anim_Use_Item();

	void				Anim_Light_Hit();
	void				Anim_Heavy_Hit();
	void				Anim_Flee();
	void				Anim_Die();
	void				Anim_Viroty();
	virtual		void		Set_StunState()override;
	virtual		void		Is_Stuning()override;
	virtual		void		Set_Misstate()override;
private:
	void				Create_Test_Effect();
	

private:
	_int			bResult = ANIM_EMD;
	_bool			m_bCombatInit = false;
	
	_bool			m_bOnceStop = false;
	_bool			m_bRun = false;
	_bool			m_bCreateDefenceTimer = false;
	_float			m_fDefenceFsmTimer = 0.f;
	_bool			m_bUltimateCam = false;
	_bool			m_bUltimateBuffRenderStop = false;
	_bool			m_bFogStart = false;
	_bool			m_bFullScreenEffect = false;
	_bool			m_bTrailStop = false;
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CStatus*				m_pStatusCom[MAPTYPE_END] = { nullptr,nullptr };
	CTexture*				m_pTexturdissolveCom = nullptr;
	class CAnimFsm*			m_pAnimFsm = nullptr;

	CGameObject*			m_pFog = nullptr;
	CGameObject*			m_pFullscreenEffect = nullptr;
	CGameObject*			m_pTrailEffect = nullptr;
	CGameObject*			m_pTrailUVMoveEffect = nullptr;

private:
	vector<CGameObject*>	m_PlayerParts;
	vector<CGameObject*>	m_pEffectParts;
private:
	HRESULT		SetUp_Components();
	HRESULT		SetUp_ShaderResources();
	HRESULT		Combat_Init();
	HRESULT		Ready_CombatParts();
public:
	static CHero_Garrison* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	BaseDelegater<Tag_HighLightUIDesc> m_Hero_DungeonUIDelegeter;
	BaseDelegater<UI_REPRESENT, _uint> m_Hero_CombatTurnDelegeter;
	BaseDelegater<_bool> m_Hero_CombatStateCanvasDelegeter;	// 밑에 상태캔버스 키는것

private:
	_uint			m_iTurnCanvasOption = 0;		// 0이면 턴끝남 1이면 죽음
	UI_REPRESENT	m_Represnt = REPRESENT_GARRISON;
	WeaponType		m_eWeaponType = WEAPON_SWORD;
	_bool			m_bUltimamteEffect = false;
	_float			m_UltimateTimer = 0.f;
	_float								m_fGlowStrength = 0.f;
	_bool								m_bIsChange = false;
private:
	_float		m_SpeedRatio = 7.f;
	_float		m_LimitDistance = 8.f;
	_float		m_ReturnDistance = 0.15f;
	_float		m_setTickForSecond = 0.9f;
	_uint		m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_NONE;
	wstring			m_TextureTag = TEXT("");
	_float3			m_vSkill_Scale;
	_float4			m_vSkill_Pos;

	_bool		m_bUltimateHit[15] = { false };
	_bool		m_bOriginBuff = false;
	_bool			m_bTraillEffectStartCheck = false;
	_bool			m_bTrailEndCheck = false;

	/*Imgui*/
	_float3			m_vTestPos;
	_float3			m_vTestScale;

	

};

END