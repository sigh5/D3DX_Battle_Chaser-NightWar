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


protected:
	CGameObject*					m_pHitTarget = nullptr;		//Combat
	queue<pair<_uint, _double>>		m_CurAnimqeue;
	_vector							m_vOriginPos;				// for.Combat


protected:
	_bool							m_bIsIdle = false;;
	_bool							m_bIsDead = false;
	_bool							m_bDeadEvent = false;
	_bool							m_bCombatChaseTarget = false;
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

protected:
	_bool			m_bDefence = false;
	_bool			m_isWideBuff = false;
public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END