#pragma once
#include "Client_Defines.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CHero_Knolan final :public CPlayer
{
protected:
	CHero_Knolan(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CHero_Knolan(const CHero_Knolan& rhs);
	virtual ~CHero_Knolan() = default;

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

public:  /*For.Combat*/
	HRESULT			  Combat_Initialize();
	virtual	  void	  Combat_Tick(_double TimeDelta)override;
	virtual	  _bool	  Is_PlayerDead()override;
	
private:
	void			ObserverTest(_double TimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom =  nullptr;
	CCollider*				m_pColliderCom = nullptr;
	class CAnimFsm*			m_pFsmCom = nullptr;

private:
	_bool					m_bCombat_LastInit = false;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

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

	/*Test*/
	BaseDelegater<_double, _uint> m_Hero_GullyHPDelegater;

	/*Test*/
	BaseDelegater<_uint> m_Hero_GullyTestShakingDelegater;

};

END
