#pragma once
#include "Client_Defines.h"
#include "Canvas.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)
class CTurnUICanvas final : public CCanvas
{
public:
	enum  TURN_UI_TYPE { TURN_UI_TOP, TURN_UI_BOTTOM, TURN_UI_END };
	enum  TURN_UI_OPTION {OPTION_HERO1, OPTION_HERO2, OPTION_HERO3, 
					OPTION_EMEMY1,OPTION_EMEMY2, OPTION_EMEMY3, TURN_UI_OPTION_END, };

private:
	CTurnUICanvas(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CTurnUICanvas(const CTurnUICanvas& rhs);
	virtual ~CTurnUICanvas() = default;

public:
	const UI_REPRESENT	Get_CurrentActor()const;


public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	
public:
	void	ChildrenMoveCheck(UI_REPRESENT iRepesentNum, _uint iOpiton); // iOption == 0,1,2,3,4,5 RepesetOption
	
	void	DeleteCharUI(UI_REPRESENT UiRepresentNum);		// Obsever
	void	Move_Children(); /*logic*/

	void	Move_ReCoverChild();

	virtual void		Set_RenderActive(_bool bActive)override;
	virtual void			Shaking_Child_UI()override;

	void	Sort_floatYPosChild();
	
	void	Imgui_ViewYPos();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();	
	HRESULT SetUp_ChildrenPosition();

private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix;
	_float					m_fChildBottom = 0.f;

	class CUI*				m_pTopUI = nullptr;
	class CUI*				m_pBottomUI = nullptr;
	_bool					m_bMoveFinish = false;
	vector<CUI*>			OldCharImage;

	
public:
	static CTurnUICanvas* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) ;
	virtual void Free() override;


};

END