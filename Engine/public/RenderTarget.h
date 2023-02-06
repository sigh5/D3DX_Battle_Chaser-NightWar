#pragma once

#include "Base.h"

BEGIN(Engine)

/* ������ �׷��������� �����ʹ�. */
/* ���۵�� : DIFFUSE(�ݻ��), NORMAL(��ֺ���), SHADE(���)*/

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

	ID3D11ShaderResourceView* Get_SRV() const {
		return m_pSRV;
	}

	ID3D11DepthStencilView* Get_SDSV() const {
		return m_pShadowDepthView;
	}

	D3D11_VIEWPORT	Get_ViewPortDesc()const { return m_ViewPort; }

public:
	HRESULT Initialize(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4* pClearColor);
	HRESULT Clear();
	HRESULT		Reday_ShadowDepthStencilView( _uint iWidth, _uint iHeight);

#ifdef _DEBUG

public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	void Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif

private:
	ID3D11Device*						m_pDevice = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;

private:
	ID3D11Texture2D*					m_pTexture2D = nullptr;
	ID3D11RenderTargetView*				m_pRTV = nullptr;
	ID3D11ShaderResourceView*			m_pSRV = nullptr;
	ID3D11DepthStencilView *			m_pShadowDepthView = nullptr;
private:
	_float4								m_vClearColor;
	D3D11_VIEWPORT						m_ViewPort;
#ifdef _DEBUG
private:
	_float4x4							m_WorldMatrix;

#endif

public:
	static CRenderTarget* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4* pClearColor);
	virtual void Free() override;
};

END