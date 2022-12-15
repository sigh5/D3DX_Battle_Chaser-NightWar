#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

/*
	���� �� �� : ���� �ִϸ��̼� �ٸ��߱� 
	���� �ִϸ��̼����� ���� ����¥��		
*/

class CSlimeKing final :public CMonster
{
private:
	CSlimeKing(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CSlimeKing(const CSlimeKing& rhs);
	virtual ~CSlimeKing() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta);
	virtual void	Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void			Combat_Tick(_double TimeDelta);
	_int			Is_MovingAnim();
	void			CombatAnim_Move(_double TImeDelta);
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	class CMonsterFsm*			m_pFsmCom = nullptr;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ShaderResources();

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
	_int							m_iMovingDir = ANIM_EMD;

public:
	static CSlimeKing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END