#pragma once

#include "HitBoxObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Point_Instancing;
class CShader;
class CBone;
class CTexture;
END

BEGIN(Client)
class CBuff_Effect final : public CHitBoxObject
{
public:
	typedef struct tag_BuffEffcet_Client
	{
		CTransform*			ParentTransform;
		_float4x4			PivotMatrix;			//생성위치를 알기위해서
		CBone*				pSocket;				//생성위치를 알기위해서
	
		_float4				vPosition;
		_float3				vScale;
		_float				vAngle;					// 스킬의 크기
		_float				fCoolTime;				// 몇초뒤에 삭제
		_bool				bIsMainTain;			// 유지될지 유지가 안될지 결정
		BUFF_TYPE			eBuffType;
		_int				iFrameCnt;
		_bool				bIsUp;
		_bool				bIsStraight;
		_bool				bIsBack;
	
	}BuffEffcet_Client;


public:
	CBuff_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBuff_Effect(const CBuff_Effect& rhs);
	virtual ~CBuff_Effect() = default;

public:
	void	Set_Client_BuffDesc(BuffEffcet_Client& Desc);
	void	Set_Client_BuffDesc(BuffEffcet_Client& Desc, class CBone* pSocket, _float4x4 PivotMatrix);


	const	_bool	Get_IsFinish()const { return m_bIsFinsishBuffer; }
	const   _bool  Get_MainTain()const { return m_Client_BuffEffect_Desc.bIsMainTain; }
	
	void	Set_Glow(_bool bUseGlow, wstring GlowTag,_int iGlowTextureNumber);
	void	Set_Dissove(_bool bUseGlow, wstring GlowTag, _int iGlowTextureNumber);
	void	Set_ShaderPass(_uint iShaderPass);


	void	Set_CamEffect(BuffEffcet_Client & Desc);


	void	Is_Particle_Effect(_int iInstanceNum);

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
	CTexture*							m_pGlowTextureCom = nullptr;
	CTexture*							m_pDeTextureCom = nullptr;

private:
	BuffEffcet_Client					m_Client_BuffEffect_Desc;

	_matrix								m_OriginMatrix;
	_float4x4							m_SocketMatrix;
	_float3								m_vScale;
	_bool								m_bIsFinsishBuffer = false;
	_bool								m_bUseGlow = false;
	wstring								m_GlowstrTag = TEXT("");
	_int								m_iGlowTextureNum = 0;
	_float								m_bMainTainTimer = 0.f;
	_float								m_fGlowStrength=0.f;
	_bool								m_bIsChange = false;

	_bool								m_bIsUpdown = false;
	_int								m_iDeSolveNum = 0;

	_float								m_fTick = 0.f;
	_float								m_fIncraseX = 0.f;
	_float								m_fIncraseY = 1.f;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	
private:
	void		Reset_CurMartirx();

	

public:
	static CBuff_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END