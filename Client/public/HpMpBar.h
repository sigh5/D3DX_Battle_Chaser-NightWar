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

class CHpMpBar final : public CUI
{
private:
	CHpMpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHpMpBar(const CHpMpBar& rhs);
	virtual ~CHpMpBar() = default;

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


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CHpMpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};

END