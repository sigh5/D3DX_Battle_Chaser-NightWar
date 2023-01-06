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
class CTrunWinCanvas  final : public CCanvas
{
private:
	CTrunWinCanvas(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CTrunWinCanvas(const CTrunWinCanvas& rhs);
	virtual ~CTrunWinCanvas() = default;
	
public:
	void	Set_WinRender(_bool bWin) { m_bWinRender = bWin; }


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

	_bool					m_bWinRender = false;


public:
	static CTrunWinCanvas* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;

};


END