#pragma once

#include "HitBoxObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Point_Instancing;
class CShader;
class CBone;
class CTexture;
class CCollider;
class CModel;
END

BEGIN(Client)
class CMesh_Effect final : public CHitBoxObject
{
public:
	typedef struct tag_MeshEffect_Client
	{
		CTransform*			ParentTransform;
		_float4x4			PivotMatrix;			//생성위치를 알기위해서
		_float4				vPosition;
		_float3				vScale;
		_float				vAngle;					// 스킬의 크기
	
		_int				iFrameCnt;
		_int				iMaxNum;
		_bool				bSlimeUltimate;

		_bool				bKnolan;
		_bool				bGarrison;
		_bool				bBretto;
		WeaponType			eType;
		_uint				iHitNum;
		CHitBoxObject::WEAPON_OPTIONAL		iWeaponOption;

	}MeshEffcet_Client;

private:
	CMesh_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Effect(const CMesh_Effect& rhs);
	virtual ~CMesh_Effect() = default;

public:
	void	Set_Client_BuffDesc(MeshEffcet_Client& Desc);
	const	_bool	Get_IsFinish()const { return m_bIsFinsishBuffer; }
	
	void	Set_Glow(_bool bUseGlow, wstring GlowTag);
public:
	virtual	 WeaponType		Get_Type()const override { return m_Client_MeshEffect_Desc.eType; }
	virtual _uint			Get_WeaponOption()override { return m_Client_MeshEffect_Desc.iWeaponOption; }
	virtual	 CCollider*		Get_Colider() override { return m_pColliderCom; }
	
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CModel*								m_pModelCom = nullptr;
	CCollider*							m_pColliderCom = nullptr;
	CTexture*							m_pTextureCom = nullptr;
	CTexture*							m_pGlowTextureCom = nullptr;
private:
	MeshEffcet_Client					m_Client_MeshEffect_Desc;

	_matrix								m_OriginMatrix;
	_float4x4							m_ConvertCurMatrix;
	_float3								m_vScale;
	_bool								m_bIsFinsishBuffer = false;
	_bool								m_bUseGlow = false;
	wstring								m_GlowstrTag = TEXT("");
	_int								m_iGlowTextureNum = 0;
	_float								m_bMainTainTimer = 0.f;
	_float								m_fGlowStrength = 0.f;
	_bool								m_bIsChange = false;

	_bool								m_bIsUpdown = false;

	_int								m_iPlayOnFrameCnt = 0;

	_int								m_iTextureNum = 0;
	_bool								m_bGlowEnd = false;

	/*_float								m_fHitTimer = 0.f;
	_bool								m_bHit = false;*/

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void		Reset_CurMartirx();

public:
	static CMesh_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END