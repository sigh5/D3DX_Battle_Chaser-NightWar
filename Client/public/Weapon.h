#pragma once
#include "Client_Defines.h"
#include "HitBoxObject.h"

BEGIN(Engine)
class CCollider;
class CBone;
class CRenderer;
END

BEGIN(Client)
class CWeapon final: public CHitBoxObject
{
public:
	
	typedef struct tagWeaponDesc
	{
		_float4x4			PivotMatrix;
		CBone*				pSocket;
		CTransform*			pTargetTransform;
		_float3				vScale;
		_float4				vPosition;
		WeaponType			eType;
		WEAPON_OPTIONAL		iWeaponOption;

	}WEAPONDESC;

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual CCollider*				Get_Colider() override { return m_pColliderCom; };
	virtual WeaponType				Get_Type()const override { return m_WeaponDesc.eType; }
	virtual _uint					Get_WeaponOption()override { return m_WeaponDesc.iWeaponOption; }
	virtual  void 					Set_WeaponOption(_uint iOption)override { m_WeaponDesc.iWeaponOption = static_cast<CHitBoxObject::WEAPON_OPTIONAL>(iOption); }
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

private:
	CCollider*				m_pColliderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;

protected:
	_float4x4				m_SocketMatrix;
	WEAPONDESC				m_WeaponDesc;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
