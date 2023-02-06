#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight final :public CBase
{
private:
	explicit CLight(ID3D11Device* pDevice , ID3D11DeviceContext * pContext);
	virtual ~CLight() = default;

public:
	 LIGHTDESC*	Get_LightDesc() { return &m_LightDesc; }

public:
	void				Set_LightView_Matrirx(_float4x4 fMatrix) { m_LightViewMatrix = fMatrix; }
	void				Set_LightProj_Matrirx(_float4x4 fMatrix) { m_LightProjMatrix = fMatrix; }

	_float4x4			Get_Light_Matrix() {
		return m_LightViewMatrix;
	}

	_float4x4			Get_Light_ProjMatrix() {
		return m_LightProjMatrix;
	}
public:
	HRESULT				Initialize(const LIGHTDESC& LightDesc);
	HRESULT				Render(class CVIBuffer_Rect* pVIBuffer, class CShader* pShader);

private:
	ID3D11Device*					m_pDevice;
	ID3D11DeviceContext *			m_pContext;

private:
	LIGHTDESC						m_LightDesc;
	_float4x4						m_LightViewMatrix;
	_float4x4						m_LightProjMatrix;

public:
	static		CLight*				Create(ID3D11Device* pDevice, ID3D11DeviceContext * pContext,const LIGHTDESC& LightDesc);
	virtual		void				Free()override;

};
END
