#pragma once
#include "CombatActors.h"
#include "BaseDelegater.h"		// �ֵ� ü���־ �˷������

BEGIN(Engine)

class ENGINE_DLL CMonster abstract :public CCombatActors
{
protected:
	CMonster(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	_bool		IsHaveUlitmate()const { return m_bHaveUltimate; }
	_bool		IsHaveSkill2() const{ return m_bHaveSkill2; }
	_bool		IsHaveBuff()const { return m_bHaveBuff; }
	_bool		IsHaveUseItem()const { return m_bMonsterHaveUseItem; }
public:
	virtual		_bool	IsCollMouse() = 0;


	const _uint		Get_HitNum()const {return m_iHitNum;}
	const _bool		Get_UseBuff()const { return m_useBuff; }
	void			Set_Me_HitPlayer(CGameObject* pHiter) { m_pMeHit_Player = pHiter; }
	CGameObject*	Get_Me_hitPlayer() { return m_pMeHit_Player; }
	_int	Get_MonsterActiveNum() {
		return m_iMonsterAttackNum;
	}
	void			Set_MonsterActiveNum(_int iMonsterAttackNum) { m_iMonsterAttackNum = iMonsterAttackNum; }
public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public: /*For.Imgui*/
	virtual _bool Piciking_GameObject() { return false; }

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	_uint			m_iHitNum = 0;
	_bool			m_useBuff = false;
	CGameObject*	m_pMeHit_Player = nullptr;
	
	_bool			m_bHaveUltimate = false;
	_bool			m_bHaveSkill2 = false;
	_bool			m_bHaveBuff = false;
	_bool			m_bMonsterHaveUseItem = false;
	_int			m_iMonsterAttackNum = 0;
	_bool			m_bMonster_Victroys = false;
};

END