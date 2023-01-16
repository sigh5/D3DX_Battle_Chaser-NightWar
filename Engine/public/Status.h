#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStatus final: public CComponent
{
public:
	typedef struct tag_StatusDesc
	{
		_int				iHp;	
		_int				iMp;		//나중에 치명타 , 돈 이런거 넣기
		_float4				vDungeon_Pos;
		_float3				vDungeon_Scale;
		_float4				vCombat_Pos;
		_float3				vCombat_Scale;
	}StatusDesc;

protected:
	CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

public:
	void		Set_Dungeon_PosScale(_float4 vPos, _float3 vScale)
	{
		m_StatusDesc.vDungeon_Pos = vPos;
		m_StatusDesc.vDungeon_Scale = vScale;
	}

	void		Set_Combat_PosScale(_float4 vPos, _float3 vScale)
	{
		m_StatusDesc.vCombat_Pos = vPos;
		m_StatusDesc.vCombat_Scale = vScale;
	}

	const _float4			Get_DungeonPos()const { return m_StatusDesc.vDungeon_Pos; }
	const _float3			Get_DungeonScale()const { return m_StatusDesc.vDungeon_Scale; }

	const _float4			Get_CombatPos()const { return m_StatusDesc.vCombat_Pos; }
	const _float3			Get_CombatScale()const { return m_StatusDesc.vCombat_Scale; }


	void					Incrase_Hp(_int iAmount);
	void					Incrase_Mp(_int iAmount);


	_float			Get_CurStatusHpRatio();
	_float			Get_CurStatusMpRatio();

	void			Take_Damage(_int iDamgae);	
	void			Use_SkillMp(_int iMp);

	_bool			Get_Dead()const { return m_bDead; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual	void	Final_Update()override;

private:
	StatusDesc			m_StatusDesc;
	_int				m_iMaxHp = 0;
	_int				m_iMaxMp = 0;
	_bool				m_bDead = false;


public:
	static CStatus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END