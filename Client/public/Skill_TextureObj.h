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

public:
	const	_bool			Get_IsTargetHit()const { return m_IsHitTarget; }
	void					Set_Shoot(_bool bShoot) { m_bShoot = bShoot; }
	void					Set_ChaserBone(_bool bChaserBone) { m_bChaserBoneStop = bChaserBone; }
	void					Set_Skill_Texture_Client(Skill_Texture_Client& Desc);
	_bool					Hit_CountIncrease();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

public:



private:
	CShader*							m_pShaderCom = nullptr;
	CCollider*							m_pColliderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CVIBuffer_Point_Instancing*			m_pVIBufferCom = nullptr;
	CTexture*							m_pTextureCom = nullptr;
private:
	Skill_Texture_Client		m_SkillDesc;
	_bool					m_IsHitTarget = false;
	_bool					m_bShoot = false;
	_bool					m_bChaserBoneStop = true;
	_float4x4				m_SocketMatrix;
	_matrix					m_OriginMatrix;
	_float3					m_vScale;
	_int						iSubMul = 1;

private:

	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;

	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void					Set_Cur_Pos();
	void					Skill_Golf_Shot(_double TimeDelta);
	void					Skill_Meteo(_double TimeDelta);


public:
	static CSkill_TextureObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};

END