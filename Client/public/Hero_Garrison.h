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


class CHero_Garrison final : public CPlayer	
{
/* ��� �ִϸ��̼��� ���� IDLE�� �����̴�. ���� �ʿ䰡����.*/
private:
	CHero_Garrison(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHero_Garrison(const CHero_Garrison& rhs);
	virtual ~CHero_Garrison() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public: /*For.SceneChange*/
	virtual void		Change_Level_Data(_uint iLevleIdx)override;

public: /*For.Dungeon*/
	virtual  _uint		Get_AnimationIndex() override;
	virtual   void		AnimMove()override;
	virtual	  void		HighLightChar()override;
	virtual	  void		NormalLightCharUI()override;
	virtual	  void		Dungeon_Tick(_double TimeDelta)override;;

public:  /*For.Combat*/
	virtual	  void	  Combat_Tick(_double TimeDelta)override;
	virtual	  void	  Combat_Ultimate(_double TimeDelta);
	

public:
	virtual	  _bool	  Is_PlayerDead()override;
	_int	  Is_MovingAnim();
	 void	  CombatAnim_Move(_double TImeDelta);



public: 
	void				Anim_Idle();
	void				Anim_Intro();
	void				AnimNormalAttack();
	void				Anim_Skill1_Attack();
	void				Anim_Skill2_Attack();
	void				Anim_Uitimate();
	void				Anim_Buff();
	void				Anim_WideAreaBuff();
	void				Anim_Use_Item();
	void				Anim_Defence();
	void				Anim_Light_Hit();
	void				Anim_Heavy_Hit();
	void				Anim_Flee();
	void				Anim_Die();
	void				Anim_Viroty();

private:
	_int	 bResult = ANIM_EMD;
	_bool	m_bCombatInit = false;
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	class CAnimFsm*				m_pAnimFsm = nullptr;

private:
	HRESULT		SetUp_Components();
	HRESULT		SetUp_ShaderResources();
	HRESULT		Combat_Init();
public:
	static CHero_Garrison* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

public:
	BaseDelegater<Tag_HighLightUIDesc> m_Hero_DungeonUIDelegeter;
	
private:
	//queue<pair<_uint, _double>>	m_CurAnimqeue;


};

END