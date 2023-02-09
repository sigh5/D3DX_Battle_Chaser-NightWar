#pragma once
#include "Client_Defines.h"
#include "Player.h"
#include "Skill_Object.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
class CNavigation;
class CStatus;
END

BEGIN(Client)

class CHero_Knolan final :public CPlayer
{
protected:
	CHero_Knolan(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHero_Knolan(const CHero_Knolan& rhs);
	virtual ~CHero_Knolan() = default;

public:
	virtual class CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual		_bool	Calculator_HitColl(CGameObject* pWeapon)override;	//무기 아니면 스킬구체
	virtual	  _bool	  Is_Dead()override;

	virtual _int				Get_RestHpPotion()override;
	virtual _int				Get_RestMpPotion()override;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth()override;
public: /*For.SceneChange*/
	virtual void		Change_Level_Data(_uint iLevleIdx)override;
	virtual		void	Fsm_Exit()override;
	void				Intro_Exit();			//얘가 인트로가 제일길어서 
	virtual		void	Defence_Exit()override;

public: /*For.Dungeon*/
	virtual _uint		Get_AnimationIndex() override;
	virtual void		AnimMove()override;
	virtual	  void		HighLightChar()override;
	virtual	  void		NormalLightCharUI()override;
	virtual	  void		Dungeon_Tick(_double TimeDelta)override;;
	
public:  /*For.Combat*/
	HRESULT			  Combat_Initialize();
	virtual	  void	  Combat_Tick(_double TimeDelta)override;
	virtual	  void	  Combat_DeadTick(_double TimeDelta)override;
	virtual		void		Set_StunState()override;
	virtual		void		Set_Misstate()override;

	virtual void	  Initialize_CombatSound()override;
	virtual		void		Is_Stuning()override;


	void			  Create_SkillFire();		// Test_Texture용 나중에 삭제바람
	void			  Create_Test_Effect();		//Test_Effect


	void			  Create_Buff_Effect();
	void			  Create_Skill_Texture();		// 실제로 쓰는 것
	void			  Create_Skill_Texture_On_Hiter();
	void			  Create_Defence_Area();
	void			  Create_WideBuffEffect();
	void			  Create_Wide_BuffEffect_Second();
	void			  Create_Skill_Ultimate_Effect0();
	void			  Create_Buff_MainTain_Effect();
	void			  Create_Ultimate_Start_CamEffect();
	void			  Create_Ultimate_StartFog_CamEffect();

	virtual void	  Create_Heavy_Hit_Effect()override;


	virtual void	  Create_Hit_Effect()override;
	virtual void	  Create_Defence_Effect_And_Action()override;


	virtual void	  Use_HpPotion()override;
	virtual void	  Use_MpPotion()override;
	virtual void	  Create_Wide_Debuff(CStatus::DEBUFFTYPE eDebuffOption)override;

	void			  Anim_Frame_Create_Control();
	
	virtual void	  Boss_Ultimate_Anim()override;


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom =  nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CStatus*				m_pStatusCom[MAPTYPE_END] = { nullptr,nullptr };
	CTexture*				m_pTexturdissolveCom = nullptr;
	class CAnimFsm*			m_pFsmCom = nullptr;
	CGameObject*			m_pFog = nullptr;
	CGameObject*			m_pFullscreenEffect = nullptr;

private:
	_bool					m_bCombat_LastInit = false;
	_float								m_fGlowStrength = 0.f;
	_bool								m_bIsChange = false;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();
	HRESULT					Ready_Parts_Combat();
	virtual		void		Calculator_HitDamage()override;	/*충돌시 함수*/

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
	void					Anim_Defence();
	void					Anim_Light_Hit();
	void					Anim_Heavy_Hit();
	void					Anim_Flee();
	void					Anim_Die();
	void					Anim_Viroty();



public:
	static CHero_Knolan* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

public:
	BaseDelegater<Tag_HighLightUIDesc> m_Hero_DungeonUIDelegeter;

	BaseDelegater<UI_REPRESENT , _uint> m_Hero_CombatTurnDelegeter;
	BaseDelegater<_bool> m_Hero_CombatStateCanvasDelegeter;	// 밑에 상태캔버스 키는것


private:
	_uint			m_iTurnCanvasOption = 0;		// 0이면 턴끝남 1이면 죽음
	UI_REPRESENT	m_Represnt = REPRESENT_KNOLAN;
	WeaponType		m_eWeaponType = WEAPON_SKILL;

	_bool			m_bOnceStop = false;
	_bool			m_bUltimateCam = false;
	_bool			m_bUltimateBuffRenderStop = false;

	_float3			m_vSkill_Scale;
	_float4			m_vSkill_Pos;
	string			m_BoneTag = "";
	wstring			m_TextureTag = TEXT("");
	Skill_DIR		m_SkillDir = Skill_DIR_END;



	CHitBoxObject::WEAPON_OPTIONAL			m_iWeaponOption = CHitBoxObject::WEAPON_OPTIONAL::WEAPON_OPTIONAL_NONE;

	_bool			m_bCreateDefenceTimer = false;
	_float			m_fDefenceFsmTimer = 0.f;
	_bool			m_bOriginBuff = false;
	_bool			m_bFogStart = false;
private:
	vector<CGameObject*>	m_PlayerParts;
	vector<CGameObject*>	m_pEffectParts;
	


private:/*For_Imgui*/
	_float3					m_vTestPos;
	_float3					m_vTestScale;
	 float Temp[3];
	 float Sour[3];
	 int					m_iRadioButton22=0;


};

END
