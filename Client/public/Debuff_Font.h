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
class CDebuff_Font  final : public CHitBoxObject
{
public:
	typedef struct tag_Debuff_Font
	{
		_tchar		szRepresent;
	}FONT_DEBUFF_DESC;

public:
	CDebuff_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebuff_Font(const CDebuff_Font& rhs);
	virtual ~CDebuff_Font() = default;

public:
	void	Set_RenderActive(_bool bRenderActive) { m_bIsRendering = bRenderActive; }
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

	void		Render_Font(_float4 vPos, _float3 vScale, _tchar szChar);

private:
	HRESULT		SetUp_Components();
	HRESULT		SetUp_ShaderResources();

private:
	_bool								m_bMoveTrue = false;
	FONT_DEBUFF_DESC					m_fontDesc;

	_bool								m_bIsRendering = false;

	_float								m_fMoveSpeed = 50.f;

public:
	static CDebuff_Font* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END

