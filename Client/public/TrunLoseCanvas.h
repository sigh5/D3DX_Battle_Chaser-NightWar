#pragma once
#include "Client_Defines.h"
#include "Canvas.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CTrunLoseCanvas final  :public CCanvas
{
private:
	CTrunLoseCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrunLoseCanvas(const CTrunLoseCanvas& rhs);
	virtual ~CTrunLoseCanvas() = default;

public:
	void	Set_LoseRender(_bool bLose) { m_bLoseRender = bLose; }

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public:
	virtual void	Set_RenderActive(_bool bTrue)override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix;

	_bool					m_bLoseRender = false;


public:
	static CTrunLoseCanvas* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};
END
