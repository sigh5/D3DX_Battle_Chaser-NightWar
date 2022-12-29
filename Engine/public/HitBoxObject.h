#pragma once
#include "GameObject.h"


BEGIN(Engine)

class	ENGINE_DLL	CHitBoxObject  abstract : public CGameObject
{
public:


protected:
	CHitBoxObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHitBoxObject(const CHitBoxObject& rhs);
	virtual ~CHitBoxObject() = default;

public:
	void			Set_WeaponDamage(_uint iDamage) { m_iWeaponDamage = iDamage; }
	_uint			Get_WeaponDamage()const { return m_iWeaponDamage; }

	void			Set_HitNum(_uint iHitNum) { m_iHitNum = iHitNum; }
	_uint			Get_HitNum()const { return m_iHitNum; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

public:
	virtual class CCollider*				Get_Colider()	{ return nullptr; }
	virtual WeaponType						Get_Type()const	{ return WEAPON_END; }


protected:
	_uint					m_iWeaponDamage = 0;
	_uint					m_iHitNum = 0;
	_bool					m_isCloned = false;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) =0;
	virtual void Free() override;

};

END
