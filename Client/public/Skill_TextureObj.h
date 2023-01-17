#pragma once

#include "HitBoxObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CVIBuffer_Point_Instancing;
class CVIBuffer_Point;
class CShader;
class CBone;
class CTexture;
END

BEGIN(Client)
class CSkill_TextureObj final : public CHitBoxObject
{
public:
	typedef struct tagSkill_Texture_Desc : public HitBoxObject
	{

	}SKILL_TEXTURE_DESC;

	typedef struct  tag_Skill_Texture_Client
	{
		CTransform*			ParentTransform;
		_float4x4			PivotMatrix;			//생성위치를 알기위해서
		CBone*				pSocket;				//생성위치를 알기위해서
		CTransform*			pTraget;			// 스킬이 가는 위치
		_float4				vPosition;
		_float3				vScale;

		_float				vAngle;		// 스킬의 크기

		CHitBoxObject::WEAPON_OPTIONAL		iWeaponOption;
		WeaponType			eType;
		Skill_DIR			eDir;
	}Skill_Texture_Client;


private:
	CSkill_TextureObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_TextureObj(const CSkill_TextureObj& rhs);
	virtual ~CSkill_TextureObj() = default;

public:
	virtual	 WeaponType		Get_Type()const override { return m_SkillDesc.eType; }
	virtual	 CCollider*		Get_Colider() override { return m_pColliderCom; }
	virtual _uint			Get_WeaponOption()override {return m_SkillDesc.iWeaponOption;}

	void					Set_Glow(_bool bUseGlow, wstring GlowTag, _int iGlowTextureNumber);
	void	Set_ShaderPass(_uint iShaderPass){
		m_HitBoxDesc.HitBoxOrigin_Desc.m_iShaderPass = iShaderPass;
	}
public:
	const	_bool			Get_IsTargetHit()const { return m_IsHitTarget; }
	void					Set_Shoot(_bool bShoot) { m_bShoot = bShoot; }
	void					Set_ChaserBone(_bool bChaserBone) { m_bChaserBoneStop = bChaserBone; }
	void					Set_Skill_Texture_Client(Skill_Texture_Client& Desc);
	void					Set_SKill_Texture_Client_Make_Hiter(Skill_Texture_Client& Desc);
	
	void					Set_ChaserBone();

	_bool					Hit_CountIncrease();
	_bool					Is_HitReady();
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

private:
	CShader*							m_pShaderCom = nullptr;
	CCollider*							m_pColliderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CVIBuffer_Point_Instancing*			m_pVIBufferCom = nullptr;
	CTexture*							m_pTextureCom = nullptr;
	CTexture*							m_pGlowTextureCom = nullptr;
private:
	Skill_Texture_Client				m_SkillDesc;
	_bool								m_IsHitTarget = false;
	_bool								m_bShoot = false;
	_bool								m_bChaserBoneStop = true;
	_float4x4							m_SocketMatrix;
	_matrix								m_OriginMatrix;
	_float3								m_vScale;
	_int								iChange_Sign = 1;

	_float								m_fGOlfTimer = 0.f;
	_float								m_fFixTimer = 0.25f;
	_float								m_fIncrease = 6.f;
	_float								m_fDecreate = 7.f;


	_float								m_fHiter_CreateTimer;
	_bool								m_bIncreateHit = false;
	_bool								m_bSignChanger = false;

	_bool								m_bUseGlow = false;
	wstring								m_GlowstrTag = TEXT("");
	_int								m_iGlowTextureNum = 0;
	_float								m_fGlowStrength = 0.f;
	_bool								m_bIsChange = false;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void					Set_Cur_Pos();
	void					Set_Cur_Pos_On_Hiter();

	void					Skill_Charging(_double TimeDelta);
	void					Skill_Golf_Shot(_double TimeDelta);
	void					Skill_Meteo(_double TimeDelta);
	void					Skill_Create_Hiter(_double TimeDelta);

public:
	static CSkill_TextureObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};

END