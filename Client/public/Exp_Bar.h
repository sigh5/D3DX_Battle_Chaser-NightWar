#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)

class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
class CStatus;
END

BEGIN(Client)
class CExp_Bar final : public CUI
{
private:
	CExp_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExp_Bar(const CExp_Bar& rhs);
	virtual ~CExp_Bar() = default;

public:
	void		Set_ExpBarStatus(CStatus* pStatus);
	void		Set_OldRatio(_float fOldRatio){
		m_fOldRatio = fOldRatio;
	}
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Change_Texture(_uint iLevel, const wstring& NewComPonentTag);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_float4x4				m_ViewMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	
	_float					m_fOldRatio = 0.f;
	_float					m_fCurRatio = 1.f;
	
	CStatus*				m_pMyStatus = nullptr;

	wstring					m_strText = L"";
	_float					m_fFontPosX=0.f, m_fFontPosY=0.f,m_fScale=0.1f;

	_uint					m_iShaderOption = 1;

	_float					m_SubXRatio = 0.f;
	_bool					m_bMove = false;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CExp_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END
