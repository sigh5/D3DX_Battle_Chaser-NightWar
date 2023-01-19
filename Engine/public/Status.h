#pragma once

#include "Component.h"


BEGIN(Engine)



class ENGINE_DLL CStatus final: public CComponent
{
public:
	enum DEBUFFTYPE { DEBUFF_FIRE , DEBUFF_BLEED , DEBUFF_ARMOR, DEBUFF_MAGIC,BUFF_DAMAGE, DEBUFF_NONE };
	enum ITEMID {ITEM_HP_POTION, ITEM_MP_POSION, ITEM_ULTIMATE_BOOK, ITEM_END};

	

	typedef struct tag_DebuffTypeDesc
	{
		_bool isDebuff_FIRE = false;
		_bool isDebuff_BLEED = false;
		_bool isDebuff_ARMOR = false;
		_bool isDebuff_MAGIC = false;
		_bool isBuff_Damage = false;


	}DEBUFF_TYPE_Desc;


	typedef struct tag_StatusDesc
	{
		_int				iHp;	
		_int				iMp;		//나중에 치명타 , 돈 이런거 넣기
		_int				iExp;
		_int				iLevel;
		_float4				vDungeon_Pos;
		_float3				vDungeon_Scale;
		_float4				vCombat_Pos;
		_float3				vCombat_Scale;


		DEBUFF_TYPE_Desc	m_tagDesc;
	}StatusDesc;

protected:
	CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

public:
	DEBUFF_TYPE_Desc	Get_DebuffType() {
		return m_StatusDesc.m_tagDesc;
	}
	void		Set_Dungeon_PosScale(_float4 vPos, _float3 vScale) {
		m_StatusDesc.vDungeon_Pos = vPos;
		m_StatusDesc.vDungeon_Scale = vScale;
	}

	void		Set_Combat_PosScale(_float4 vPos, _float3 vScale) {
		m_StatusDesc.vCombat_Pos = vPos;
		m_StatusDesc.vCombat_Scale = vScale;
	}
	void					Set_DebuffOption(DEBUFFTYPE eType, _bool isFlase=true);
	const _float4			Get_DungeonPos()const { return m_StatusDesc.vDungeon_Pos; }
	const _float3			Get_DungeonScale()const { return m_StatusDesc.vDungeon_Scale; }

	const _float4			Get_CombatPos()const { return m_StatusDesc.vCombat_Pos; }
	const _float3			Get_CombatScale()const { return m_StatusDesc.vCombat_Scale; }

	void					Incrase_Hp(_int iAmount);
	void					Incrase_Mp(_int iAmount);

	_float					Get_CurStatusHpRatio();
	_float					Get_CurStatusMpRatio();

	void					Take_Damage(_int iDamgae);	
	void					Use_SkillMp(_int iMp);

	_bool					Get_Dead()const { return m_bDead; }

	void					Set_Exp(_int iExp);
	_float					Get_ExpRatio();
	_int					Get_Level()const { return m_StatusDesc.iLevel; }


	

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual	void	Final_Update()override;

public:
	void					Add_ItemID(ITEMID iItemID, _int iNum);
	void					Use_Item(ITEMID iItemID);
	
	_int					Rest_iTemNum(ITEMID iItemID);


	map<ITEMID, _int>		&Get_ITemMap() { return m_mapItem; }


private:
	StatusDesc			m_StatusDesc;
	_int				m_iMaxHp = 0;
	_int				m_iMaxMp = 0;
	_int				m_iMaxExp = 100;
	_bool				m_bDead = false;
	
	map<ITEMID, _int>	m_mapItem;

public:
	static CStatus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END