#pragma once
#include "Client_Defines.h"
#include "Canvas.h"
#include "CombatController.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Rect;
class CTexture;
END

BEGIN(Client)

#define STATE_ACTION TEXT("UI_State_Action")
#define STATE_ABLILTY TEXT("UI_State_Ablity")
#define STATE_ITEM TEXT("UI_State_Item")


class CTurnStateCanvas final: public CCanvas
{
private:
	CTurnStateCanvas(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CTurnStateCanvas(const CTurnStateCanvas& rhs);
	virtual ~CTurnStateCanvas() = default;

public:
	BUTTON_STATE		Get_ButtonState()const { return m_StateType; }
	BUTTON_FSM_STATE	Get_ButtonFsmState()const { return m_ButtonFsmType; }


	void				Set_ButtonState(BUTTON_STATE eType) { m_StateType = eType; }
	void				Set_ButtonFsmState(BUTTON_FSM_STATE eType) {m_ButtonFsmType = eType;}
	virtual		void	Set_RenderActive(_bool bActive)override;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;


public:
	void					PickingChild();
private:
	void					CurState_Image_Change();
	void					CurState_Fsm_ButtonClick();
	void					CurState_Fsm_ButtonICon();
	void					Player_SceneChane(_bool bEvent);

private:
	void					Control_ChildRender(_bool bRenderActive);
	void					StateButton_Child();


private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();


private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4				m_ViewMatrix;

	BUTTON_STATE			m_StateType = BUTTON_STATE_END;
	BUTTON_FSM_STATE		m_ButtonFsmType = BUTTON_FSM_STATE_END;


	_uint					iTestIConNum = 0;


private:
	map<const wstring, CUI*>	m_pCurState;
	vector<CUI*>				m_pCurFsmButton;

	CUI*					pCurPickingState = nullptr;		// 현재 피킹 트루된 상태버튼
	CUI*					pCurPickingButton = nullptr;	// 현재 피킹 트루된 상태버튼의 노드들(ex)노말공격,막기)
public:
	static CTurnStateCanvas* Create(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;


};

END