#pragma once
#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class COnlyFontUI final : public CUI
{
private:
	COnlyFontUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COnlyFontUI(const COnlyFontUI& rhs);
	virtual ~COnlyFontUI() = default;

public:
	void	Set_FontOption(_int iOption);


public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


private:
	_int					m_iOption = 0;

	wstring					m_strText[10];
	_float					m_fFontPosX = 430.f;
	_float					m_fFontPosY = 500.f;

	_int					m_StrMaxNum = 0;
	_int					m_StrCurNum = 0;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


public:
	static COnlyFontUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END