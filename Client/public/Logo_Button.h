#pragma once
#include "Client_Defines.h"
#include "UI.h"
#include "Client_Manager.h"


BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CLogo_Button final :public CUI
{

private:
	CLogo_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLogo_Button(const CLogo_Button& rhs);
	virtual ~CLogo_Button() = default;

public:
	_bool	Get_GameStart()const { return m_bIsClicked; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool			Click_This_Button();
	void			In_This_Button();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_float4x4				m_ViewMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	_bool					m_bIsClicked = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
public:
	static CLogo_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END

