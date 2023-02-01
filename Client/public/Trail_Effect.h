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
class CTrail_Effect final : public CHitBoxObject
{
public:
	typedef struct  tag_Trail_Effect
	{
		CCollider* pColider;
		CModel*	   pModel;
	}tag_Trail_Effect_DESC;


public:
	CTrail_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail_Effect(const CTrail_Effect& rhs);
	virtual ~CTrail_Effect() = default;
public:
	const	_bool	Get_IsFinish()const { return m_bIsFinsishBuffer; }
	void			Set_Desc(tag_Trail_Effect_DESC& Desc,_int iTextureNum);
	void			End_Anim();


	void			Set_RenderStop() { m_bIsTrailStartCheck = false; }
	void			First_Edition();
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
	CVIBuffer_Point_Instancing*			m_pVIBufferCom = nullptr;
	CTexture*							m_pTextureCom = nullptr;

private:
	_bool								m_bIsFinsishBuffer = false;
	_bool								m_bIsTrailStartCheck = false;
	
	_float4x4							m_viewMatrix;

	tag_Trail_Effect_DESC				m_Desc;
	
	_float3								m_vOldCenter;
	_float3								m_vNewCenter;
	_int								m_iCenterCount = 0;
	_int								m_iTextureNum = 0;

	_vector								m_Trail_Pos[4];
	_vector								m_Collider_pos[12];
	
	_int								m_iCUrCOunt = 0;


	_uint								m_iShaderPass = 0;
	_float								m_fTrailTimer = 0.f;
	_vector								m_vFirstPos;
	_bool								m_bTrail_Maintain = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTrail_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END