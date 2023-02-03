#pragma once

#include "Client_Defines.h"
#include "Player.h"
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CNavigation;
class CStatus;
END

BEGIN(Client)

class CHero_Calibretto final :public CPlayer
{
private:
	CHero_Calibretto(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHero_Calibretto(const CHero_Calibretto& rhs);
	virtual ~CHero_Calibretto() = default;

public:
	virtual class CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual		_bool	Calculator_HitColl(CGameObject* pWeapon)override;	//무기 아니면 스킬구체


	virtual _int				Get_RestHpPotion()override;
	virtual _int				Get_RestMpPotion()override;
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
	void				Anim_Frame_Create_Control();

public:  /*For.Combat*/
	virtual	  void	  Combat_Tick(_double TimeDelta)override;
	virtual   void	  Combat_DeadTick(_double TimeDelta)override;
	virtual	  _bool	  Is_Dead()override;
	_int			  Is_MovingAnim();
	void			  CombatAnim_Move(_double TImeDelta);
	void			  MovingAnimControl(_double TimeDelta);
	virtual	   void	  Fsm_Exit()override;
	void			  Intro_Exit();
	virtual void	  Create_Wide_Debuff(CStatus::DEBUFFTYPE eDebuffOption)override;
	virtual		void  Calculator_HitDamage()override;	/*충돌시 함수*/
	virtual void				Initialize_CombatSound()override;
private: /*Create_Effect*/
	void				Create_Test_Effect();		// Test
	void				Create_Test_Rect_Effect();		// Test
	void				Create_Test_TextureObj();		// Test

	void			    Create_Skill1_Bullet();	
	void			    Create_Skill1_Bullet_End();
	
	void			    Create_Skill2_Beam();		
	void			    Create_Skill2_Beam_End();
	void				Create_Buff_MainTain_Effect();


	void				Create_Normal_Attack_Effect();
	void				Create_Skill1_Attack_Effect();
	void				Create_Skill2_Attack_Effect();
	void				Create_Buff_Effect();
	void			    Create_WideBuffEffect();
	void			    Create_Wide_BuffEffect_Second();
	void			    Create_Skill_Ultimate_Effect0();
	void				Create_Ultimate_End_Effect();
	void				Create_Move_Target_Effect();
	virtual void		Create_Heavy_Hit_Effect()override;
	void				Create_Ultimate_StartCam_Effect();
	void			    Create_Ultimate_StartFog_CamEffect();
	void			    Create_FullScreenEffect();


	virtual void		Create_Hit_Effect()override;
	virtual void	    Use_HpPotion()override;
	virtual void	    Use_MpPotion()override;
	
	virtual void				Boss_Ultimate_Anim()override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CStatus*				m_pStatusCom[MAPTYPE_END] = { nullptr,nullptr };
	class CAnimFsm*			m_pFsmCom = nullptr;
	
	CGameObject*			m_pFog = nullptr;
	CGameObject*			m_pFullscreenEffect = nullptr;
	CGameObject*			m_pUltimateEffect = nullptr;
	CGameObject*			m_pLazorEffect = nullptr;
private:
	_bool					m_bCombat_LastInit = false;
	_int					bResult = ANIM_EMD;
	_int					m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_NONE;
	
	_bool					m_bRun = false;
	_bool					m_bBulletShoot = false;
	_bool					m_bCreateSmoke = true;
	_bool					m_bLazorStop = true;
	_bool					m_bUltimateCam = false;
	_bool					m_bUltimateBuffRenderStop = false;
	_bool					m_bFogStart = false;
	_bool					m_bFullScreenEffect = false;
private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					Combat_Initialize();
	
	HRESULT					Ready_Parts_Dungeon();
	HRESULT					Ready_Parts_Combat();

public:
	void					Anim_Idle();
	void					Anim_Intro();
	
	void					AnimNormalAttack();
	void					Anim_Flee();

	void					Anim_Skill1_Attack();
	void					Anim_Skill2_Attack();
	void					Anim_Uitimate();
	void					Anim_Buff();
	void					Anim_WideAreaBuff();

	void					Anim_Use_Item();
	
	void					Anim_Light_Hit();
	void					Anim_Heavy_Hit();
	void					Anim_Die();
	void					Anim_Viroty();


private:
	_uint			m_iTurnCanvasOption = 0;		// 0이면 턴끝남 1이면 죽음
	UI_REPRESENT	m_Represnt = REPRESENT_CALIBRETTO;
	_uint	m_iNonRenderMeshIndex = 0;	// 0~7번까지 안그려야됌
	
public:
	static CHero_Calibretto* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

public:
	BaseDelegater<HIGHLIGHT_UIDESC> m_Hero_DungeonUIDelegeter;
	
	BaseDelegater<UI_REPRESENT, _uint> m_Hero_CombatTurnDelegeter;		//옆에 턴넘기는거
	BaseDelegater<_bool> m_Hero_CombatStateCanvasDelegeter;	// 밑에 상태캔버스 키는것

private:
	vector<CGameObject*>	m_PlayerParts;
	vector<CGameObject*>	m_pEffectParts;



	WeaponType		m_eWeaponType= WEAPON_END;


private:
	_float		m_SpeedRatio = 8.f;
	_float		m_LimitDistance = 12.f;
	_float		m_ReturnDistance = 0.4f;
	_float		m_setTickForSecond = 0.9f;
	wstring		m_TextureTag = TEXT("");
	_float3		m_vSkill_Scale;
	_float4		m_vSkill_Pos;
	_bool		m_bRecoverHeight = false;
	_bool		m_bUltimateStop = false;
private:
	/*Imgui*/
	_float3			m_vTestPos;
	_float3			m_vTestScale;

};

END
