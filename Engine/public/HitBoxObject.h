#pragma once
#include "GameObject.h"
#include "VIBuffer_Point_Instancing.h"

BEGIN(Engine)

class	ENGINE_DLL	CHitBoxObject  abstract : public CGameObject
{
public:
	enum WEAPON_OPTIONAL		//¼Ó¼º
	{
		WEAPON_OPTIONAL_NONE =0,
		WEAPON_OPTIONAL_BLUE = 1,
		WEAPON_OPTIONAL_RED_KNOLAN_SKILL2 =2,
		WEAPON_OPTIONAL_RED_KNOLAN_SKILL1 =3 ,
		WEAPON_OPTIONAL_RED_KNOLAN_NORMAL =4 ,
		WEAPON_OPTIONAL_PULPLE =5 ,
		WEAPON_OPTIONAL_GREEN= 6,
		WEAPON_OPTIONAL_PUNCH_HIT=7,
		WEAPON_OPTIONAL_PUNCH_GUN=8,
		WEAPON_OPTIONAL_GARRISON_NORMAL=9,
		WEAPON_OPTIONAL_GARRISON_SKILL1=10,
		WEAPON_OPTIONAL_GARRISON_SKILL2=11,
		WEAPON_OPTIONAL_GARRISON_Ultimate=12,
		WEAPON_OPTIONAL_SPIDER_ATTACK=13,
		WEAPON_OPTIONAL_SPIDER_HEAD = 14,
		WEAPON_OPTIONAL_SLIME_KING_HEAD = 15,
		WEAPON_OPTIONAL_SLIME_KING_BREATH = 16,
		WEAPON_OPTIONAL_SLIME_KING_Ultimate = 17,
		WEAPON_OPTIONAL_BOSS_RIGHT_HAND = 18,
		WEAPON_OPTIONAL_BOSS_SHILED = 19,
		WEAPON_OPTIONAL_BOSS_WHIP = 20,
		WEAPON_OPTIONAL_BOSS_DOUBLE_WHIP = 21,
		WEAPON_OPTIONAL_BOSS_ULTIMATE_ONE = 22,
		WEAPON_OPTIONAL_BOSS_ULTIMATE_TWO = 23,
		WEAPON_OPTIONAL_END,
	};


	typedef struct tagHitBoxObject :public GAMEOBJECTDESC
	{
		typedef struct tagHitBoxOrigin
		{
			_tchar			m_pModelTag[MAX_PATH] = TEXT("");
			_tchar			m_pTextureTag[MAX_PATH] = TEXT("");
			_uint			m_iShaderPass = 0;
		}HitBoxOriginDesc;
		

		HitBoxOriginDesc					HitBoxOrigin_Desc;
		CVIBuffer_Point_Instancing::VIBUffer_Point_TextureDesc Poing_Desc;
		_tchar			m_pTrailObjectName[MAX_PATH] = TEXT("");

	}HitBoxObject;

protected:
	CHitBoxObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHitBoxObject(const CHitBoxObject& rhs);
	virtual ~CHitBoxObject() = default;

public:
	/*const  _tchar*		Get_ModelTag()const { return m_HitBoxDesc.m_pModelTag; }
	void				Set_ModelTag(const _tchar* pModelTag) { lstrcpy(m_HitBoxDesc.m_pModelTag, pModelTag); };
	const  _tchar*		Get_TextureTag()const { return m_HitBoxDesc.m_pTextureTag; }
	void				Set_TextureTag(const _tchar* pModelTag) { lstrcpy(m_HitBoxDesc.m_pTextureTag, pModelTag); };

	_uint				Get_ShaderPass() const { return m_HitBoxDesc.m_iShaderPass; }
	void				Set_ShaderPass(_uint iShaderPass) { m_HitBoxDesc.m_iShaderPass = iShaderPass; }*/

	const	HitBoxObject& Get_HitBoxDesc()const { return m_HitBoxDesc; }

public:
	void			Set_WeaponDamage(_uint iDamage) { m_iWeaponDamage = iDamage; }
	_uint			Get_WeaponDamage()const { return m_iWeaponDamage; }

	void			Set_HitNum(_uint iHitNum) { m_iHitNum = iHitNum; }
	_uint			Get_HitNum()const { return m_iHitNum; }

	virtual _uint			Get_WeaponOption();
	virtual  void 			Set_WeaponOption(_uint iOption);



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
	HitBoxObject						m_HitBoxDesc;

protected:
	_uint					m_iWeaponDamage = 0;
	_uint					m_iHitNum = 0;
	_bool					m_isCloned = false;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) =0;
	virtual void Free() override;

};

END
