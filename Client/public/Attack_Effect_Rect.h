#pragma once
#include "Client_Defines.h"
#include "HitBoxObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CBone;
END

BEGIN(Client)
class CAttack_Effect_Rect final : public CHitBoxObject
{

public:
	typedef struct tag_CAttack_Effec_Client
	{
		_float				fCoolTime;				// 몇초뒤에 삭제
		_bool				bIsMainTain;			// 유지될지 유지가 안될지 결정
		_int				iFrameCnt;
		_int				iMaxTextureNum;
		_int				iShaderPass;
	}Attack_Effec_Client;


private:
	CAttack_Effect_Rect(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CAttack_Effect_Rect(const CAttack_Effect_Rect& rhs);
	virtual ~CAttack_Effect_Rect() = default;

public:
	void	Set_Client_BuffDesc(Attack_Effec_Client& Desc, _bool bCalibretto = false);
	
	const	_bool	Get_IsFinish()const { return m_bIsFinsishBuffer; }
	const   _bool  Get_MainTain()const { return m_Client_AttackEffect_Desc.bIsMainTain; }

	void	Set_Glow(_bool bUseGlow, wstring GlowTag, _int iGlowTextureNumber);

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*							m_pGlowTextureCom = nullptr;
private:
	_float								m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4							m_ViewMatrix;
	Attack_Effec_Client					m_Client_AttackEffect_Desc;
	_matrix								m_OriginMatrix;
	_float4x4							m_SocketMatrix;
	_float3								m_vScale;
	_bool								m_bIsFinsishBuffer = false;
	_bool								m_bUseGlow = false;
	wstring								m_GlowstrTag = TEXT("");
	_int								m_iGlowTextureNum = 0;
	_float								m_bMainTainTimer = 0.f;
	_float								m_fGlowStrength = 0.f;
	_bool								m_bIsChange = false;


	_int								m_iPlayOnFrameCnt = 0;

	_int								m_iTextureNum = 0;
	_bool								m_bGlowEnd = false;
	_bool								m_bCalibretto = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	void	Reset_CurMartirx();
	void	Reset_CurCrackMartirx();
public:
	static CAttack_Effect_Rect* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END


