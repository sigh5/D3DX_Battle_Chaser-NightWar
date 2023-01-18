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
class CCam_Effect final : public CHitBoxObject
{
private:
	CCam_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCam_Effect(const CCam_Effect& rhs);
	virtual ~CCam_Effect() = default;

public:
	const	_bool	Get_IsFinish()const { return m_bIsFinsishBuffer; }
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
	_float4x4							m_viewMatrix;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CCam_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END