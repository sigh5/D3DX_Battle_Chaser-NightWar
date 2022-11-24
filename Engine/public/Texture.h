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
	virtual HRESULT Initialize_Prototype(const wstring& pTextureFilePath, TEXTURE_TYPE eType, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg) override;

public:

	HRESULT Bind_ShaderResources(class CShader* pShaderCom, const char* pConstantName);
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName, _uint iTextureIndex = 0);
	
public:
	virtual void Imgui_RenderProperty();

private:
	_uint										m_iNumTextures = 0;
	ID3D11ShaderResourceView**					m_pTextures =nullptr;
	char*										m_szName = "";
	typedef vector<ID3D11ShaderResourceView*>	TEXTURES;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pTextureFilePath, TEXTURE_TYPE eType = TYPE_END , _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END