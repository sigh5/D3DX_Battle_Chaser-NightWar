#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Point_Instancing;
class CTexture;
class CVIBuffer_Point;
END


BEGIN(Client)
class CEffectFrame : public CGameObject
{
private:
	CEffectFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectFrame(const CEffectFrame& rhs);
	virtual ~CEffectFrame() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public: /* Imgui */
	virtual _bool	Piciking_GameObject()override;
private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Point_Instancing*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pTextureCom2 = nullptr;
private:
	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	
	_float					m_fPower = 0.f;

	_bool					m_bIsChange = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CEffectFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END