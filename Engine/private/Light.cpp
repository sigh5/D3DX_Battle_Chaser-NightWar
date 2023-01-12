#include "..\public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"


CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Initialize(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(1);

	pVIBuffer->Render();

	return S_OK;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Initialize(LightDesc)))
	{
		MSG_BOX("CLight Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
