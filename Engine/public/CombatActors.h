#pragma once
#include "GameObject.h"
#include "Bone.h"

BEGIN(Engine)

class CFSMComponent;
class CModel;
class CCollider;

class ENGINE_DLL CCombatActors abstract: public CGameObject
{
protected:
	CCombatActors(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CCombatActors(const CCombatActors& rhs);
	virtual ~CCombatActors() = default;
public:
	const   _bool	Get_IsIdle()const { return m_bIsIdle; }
	void			Set_HitTarget(CGameObject* pGameObject) { m_pHitTarget = pGameObject; }
	virtual  CGameObject*	 Get_Weapon_Or_SkillBody();
	virtual	  _bool	  Is_Dead() {return m_bIsDead;}

	_bool			IsHaveDefence()const { return m_bDefence; }
	const _bool		Get_IsWideBuff()const { return m_isWideBuff; }

	void			Set_Defence_Timer(_bool bEvent) { m_bIsDefenceTimer = bEvent; }
	const _bool		Get_UseDeffece()const { return m_bUseDefence; }
	void			Set_UseDefence(_bool bUseDefence) {
		m_bUseDefence = bUseDefence;
		Set_Defence_Timer(bUseDefence);
	}
	
	void			Set_ModelRender(_bool bModelRender) { m_bModelRender = bModelRender; }
	const _bool		Get_Model_Render()const				{ return m_bModelRender; }

	virtual void			Create_Hit_Effect();
	virtual void			Create_Heacy_Hit_Effect();
	virtual	void			Create_Defence_Effect_And_Action();




public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public: /*For.Imgui*/
	virtual _bool Piciking_GameObject() { return false; }

public: /*For.Animamtion*/
	virtual void	  CurAnimQueue_Play_Tick(_double Time, class CModel* pModel);
	virtual void	  CurAnimQueue_Play_LateTick(class CModel* pModel);
	virtual void	  Set_CombatAnim_Index(class CModel* pModel);

public:
	virtual		void	Fsm_Exit();
	virtual		_bool	Calculator_HitColl(CGameObject* pWeapon);	//무기 아니면 스킬구체
	virtual		void	HitActor_LoseHP(CGameObject* pCurActor);
	virtual		 void	Defence_Exit();

protected:
	CGameObject*					m_pHitTarget = nullptr;		//Combat
	queue<pair<_uint, _double>>		m_CurAnimqeue;
	_vector							m_vOriginPos;				// for.Combat


protected:
	_bool							m_bIsIdle = false;;
	_bool							m_bIsDead = false;
	_bool							m_bDeadEvent = false;
	_bool							m_bCombatChaseTarget = false;
	_bool							m_bUseDefence = false;//디펜스를 사용할때
	_bool							m_bIsDefenceTimer = false;

protected:
	_uint							m_bFinishOption = 0;
	_uint							m_iOldAnim = 0;
	_uint							m_iAnimIndex = 0;
	_uint							m_iStateDamage = 0;		// 어떤 공격인지에 따라 데미지가 다름
	
	_bool							m_bIsCombatScene = false;
	_bool							m_bIsCombatLastInit = false;
	_bool							m_bIsCombatAndAnimSequnce = false;
	_double							m_NormalTikcPerSecond = 1.0;
	_double							m_IntroTimer = 0.7;
	_uint							m_iHitCount = 0;

	_float							m_fDefencTimer = 0.f;

	_int							m_iHitWeaponOption = 0;
	_bool							m_bIs_Multi_Hit = false; //맞은놈이 멀티히트를 맞았냐?
	_bool							m_bIsHeavyHit = false;

	_bool							m_bOnceCreate = false;
	_int							m_iGetDamageNum = 0;
	_bool							m_bIsUseUltimate = false;

protected:
	_bool							m_bDefence = false;			//스킬에 디펜스가있냐?
	_bool							m_isWideBuff = false;
	_bool							m_bModelRender = true;
	
public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END