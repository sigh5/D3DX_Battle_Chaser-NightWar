#pragma once

#include "HitBoxObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CRenderer;
class CVIBuffer_Point_Instancing;
class CShader;
class CTexture;
END


BEGIN(Client)
class CDamageFont   final : public CHitBoxObject
{
public:
	typedef struct tag_DamageFont_Desc
	{
		_uint		iRepresentNum;
	}FONT_DESC;


public:
	CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDamageFont(const CDamageFont& rhs);
	virtual ~CDamageFont() = default;


public:
	

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Last_Initialize()override;

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CVIBuffer_Point_Instancing*			m_pVIBufferCom = nullptr;
	CTexture*							m_pTextureCom = nullptr;


public:
	HRESULT		Ready_Font();

	void		Render_Font(_float4 vPos, _float3 vScale);



private:
	_bool								m_bMoveTrue = false;
	
	FONT_DESC							m_fontDesc;
	

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CDamageFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END
