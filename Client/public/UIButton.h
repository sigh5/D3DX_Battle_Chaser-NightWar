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
class CUIButton final :public CUI
{
public:
	enum DungeonUIType { UITYPE_HIGHLIGHT, UITYPE_NORMAL,UITYPE_END };

private:
	CUIButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIButton(const CUIButton& rhs);
	virtual ~CUIButton() = default;


public:
	void			Set_ButtonImage(CUI* pUI);
	void			Set_ChildFsmButton(CUIButton* pNodeButton);
	
	BUTTON_FSM_STATE	Get_ButtonFsmState()const { return m_eFsmState; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void	Change_Texture(_uint iLevel, const wstring& NewComPonentTag);
	virtual void	Set_HighRightUIDesc(HIGHLIGHT_UIDESC& UIDesc)override;
	virtual void	Set_ParentLoad(CUI * pUI)override;
	virtual  void	Set_RenderActive(_bool bActive)override;
private:
	void			FirstDungeonUIInit();

public:
	void				State_Image_Change(BUTTON_STATE eType);

public:
	_bool			Click_This_Button();
	void			Shaking();
	
	void			Button_RenderSetActive(_bool bRenderActive);

private:
	void			Change_ICON_Active();
	void			Change_ICON_Ablity();
	void			Change_ICON_Item();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


private:
	_float4x4				m_ViewMatrix;
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_double					m_ShakeTimer = 0.0;
	_double					m_fCoolTime = 1.0;
	_int					m_iSwitching = -1;

	BUTTON_FSM_STATE		m_eFsmState = BUTTON_FSM_STATE_END;


private:
	CUI*					m_pButtonImage = nullptr;
	vector<CUIButton*>			m_ChildFsmButton;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
public:
	static CUIButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END
