#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CBone;
class CRenderer;
END

BEGIN(Client)
class CWeapon : public CGameObject
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

	}WEAPONDESC;

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	CCollider*		Get_Colider() { return m_pColliderCom; }
	WeaponType		Get_Type()const { return m_WeaponDesc.eType; }

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

private:
	CCollider*				m_pColliderCom = nullptr;
	WEAPONDESC				m_WeaponDesc;

	_float4x4				m_SocketMatrix;
	_bool					m_isCloned = false;

	_uint					m_iWeaponDamage = 0;
	_uint					m_iHitNum = 0;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
