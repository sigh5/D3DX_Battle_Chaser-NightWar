#pragma once

#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
public:
	enum TEXTURE_TYPE { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_FILTER,TYPE_END };
protected:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	ID3D11ShaderResourceView* Get_TextureResouceView(_uint iIndex = 0) const {
		return m_pTextures[iIndex];
	}
	const _uint		Get_SelectTextureIndex() const { return m_iSelectTextureIndex; }
	void			Set_SelectTextureIndex(_uint iIndex) { m_iSelectTextureIndex = iIndex; }


public:
	virtual HRESULT Initialize_Prototype(const wstring& pTextureFilePath, TEXTURE_TYPE eType, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;
	virtual		void	Final_Update()override {}
public:
	HRESULT Bind_ShaderResources(class CShader* pShaderCom, const char* pConstantName);
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName, _uint iTextureIndex = 0);
	
public:
	void	Save_Model_Texture(HANDLE hFile);


public:
	virtual void Imgui_RenderProperty();

	
private:
	_uint										m_iNumTextures = 0; //¼ýÀÚ
	ID3D11ShaderResourceView**					m_pTextures =nullptr;
	char*										m_szName = "";
	typedef vector<ID3D11ShaderResourceView*>	TEXTURES;
	_tchar					m_szTexturePath[MAX_PATH] = TEXT("");

private: /* Imgui*/
	_uint										m_iSelectTextureIndex = 0;


public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pTextureFilePath, TEXTURE_TYPE eType = TYPE_END , _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END