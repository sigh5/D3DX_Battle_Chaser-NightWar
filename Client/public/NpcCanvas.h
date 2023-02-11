#pragma once
#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CStatus;
END

BEGIN(Client)
class CNpcCanvas : public CCanvas
{
private:
	CNpcCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpcCanvas(const CNpcCanvas& rhs);
	virtual ~CNpcCanvas() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public:
	virtual void	Set_RenderActive(_bool bTrue)override;
	void			Set_DialogOption(_int iOption);
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
	_int					m_iOption = 0;



public:
	static CNpcCanvas* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;


};
END
