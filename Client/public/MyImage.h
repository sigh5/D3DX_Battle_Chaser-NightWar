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
class CMyImage final : public CUI
{

private:
	CMyImage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMyImage(const CMyImage& rhs);
	virtual ~CMyImage() = default;

public:
	void	Set_MaxTextureNum(_uint iMaxTextureNum) { m_iMaxTextureNum = iMaxTextureNum; }
	const _bool Get_BannerTimerFinsish()const { return m_bBanerTimerFinish; }
	void	Set_ShaderPass(_uint iShaderPass) { m_iShaderPassNum = iShaderPass; }
	void	Set_CurTextureNum(_uint iTextureNum);
	void	Renderer_State_Inventory() {
		m_bIsInventory = true;
	}
	void	Set_InventoryNumStr(wstring pNumstr)
	{
		m_bItemNumstr = pNumstr;
	}
	void   Set_InventoryFontPos(_float2 _f2FontPos) {
		m_fFontPosX = _f2FontPos.x;
		m_fFontPosY = _f2FontPos.y;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Change_Texture(_uint iLevel, const wstring& NewComPonentTag);
	virtual void	ShakingControl(_float fCoolTime)override;

	virtual void	Set_RenderActive(_bool bRenderActive)override;
	void			Move_Lose_Texture(_double TimeDelta);				// 텍스쳐 0~ m_iMaxTextureNum 돌리기


private:
	virtual void	Shake_Move(_double TimeDelta)override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	_float4x4				m_ViewMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	_uint					m_iMaxTextureNum = 0;
	_float					m_fTextureMoveTimer = 0.f;
	_bool					m_bLoseRenderFont = false;
	_bool					m_bWinRenderFont = false;


	_float					m_fFontPosX = 545.f;
	_float					m_fFontPosY = 400.f;

	// shake
	_float					m_fCoolTime = 0.f;
	_bool					m_bMove = false;
	_float4					m_vOriginPos;

	_float4x4				m_WorldMat;
	// shake

	_uint					m_iShaderPassNum = 1;
	_float					m_BanerTimer = 0.f;

	_bool					m_bBanerTimerFinish = false;
	_bool					m_bIsInventory = false;
	wstring					m_bItemNumstr = L"";

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMyImage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


};

END


