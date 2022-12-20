#include "..\public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_pTextures(rhs.m_pTextures)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_szName(rhs.m_szName)
{
	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_AddRef(m_pTextures[i]);

}

HRESULT CTexture::Initialize_Prototype(const wstring& pTextureFilePath, TEXTURE_TYPE eType, _uint iNumTextures)
{
	m_pTextures = new ID3D11ShaderResourceView*[iNumTextures];

	m_iNumTextures = iNumTextures;

	for (_uint i = 0; i < m_iNumTextures; ++i)
	{
		_tchar	szTexturePath[MAX_PATH] = TEXT("");

		wsprintf(szTexturePath, pTextureFilePath.c_str(), i);

		/* 문자열 분해하기. */
		_tchar			szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szTexturePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT		hr = 0;

		if (!lstrcmp(szExt, TEXT(".tga")))
			return E_FAIL;
		else if (!lstrcmp(szExt, TEXT(".dds")))
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTexturePath,nullptr, &m_pTextures[i]);
		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_pTextures[i]);

		if (FAILED(hr))
			return E_FAIL;

	}

	if (eType == CTexture::TYPE_DIFFUSE)
		m_szName = "Diffuse_Textures";

	else if (eType == CTexture::TYPE_BRUSH)
		m_szName = "Brush_Textures";

	else if (eType == CTexture::TYPE_FILTER)
		m_szName = "Filter_Textures";
	else
		m_szName = "Textures";
	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResources(CShader * pShaderCom, const char * pConstantName)
{
	return pShaderCom->Set_ShaderResourceViewArray(pConstantName, m_pTextures, m_iNumTextures);
}

HRESULT CTexture::Bind_ShaderResource(CShader * pShaderCom, const char * pConstantName, _uint iTextureIndex)
{
	if (nullptr == pShaderCom ||
		iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShaderCom->Set_ShaderResourceView(pConstantName, m_pTextures[iTextureIndex]);
}

void CTexture::Imgui_RenderProperty()
{
	
	

	if (ImGui::CollapsingHeader(m_szName, ImGuiTreeNodeFlags_DefaultOpen))
	{

		for (_uint i = 0; i < m_iNumTextures; ++i)
		{
			if (ImGui::ImageButton((void*)m_pTextures[i], ImVec2(32.f, 32.f)))
			{
				m_iSelectTextureIndex = i;
			}
			if (i == 0 || (i + 1) % 6)
				ImGui::SameLine();
		}
	}


}



CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring& pTextureFilePath, TEXTURE_TYPE eType, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, eType,iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_Release(m_pTextures[i]);

	if (false == m_bClone)
		Safe_Delete_Array(m_pTextures);

}
