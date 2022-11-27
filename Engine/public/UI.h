#pragma once
#include "GameObject.h"

BEGIN(Engine)


class ENGINE_DLL CUI abstract : public CGameObject
{
public:
	typedef struct tag_UIDesc :public GAMEOBJECTDESC
	{
		CGameObject*	pBroadCaster;
		wstring			m_pTextureTag;
	}UIDESC;

protected:
	CUI(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;


public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;


protected:
	virtual HRESULT SetUp_UI()sealed;

	virtual void Begin_UI()sealed;
	virtual void End_UI()sealed;

public: /* imgui */
	virtual void Imgui_RenderComponentProperties() {}
	virtual void Imgui_RenderProperty()override {}

protected:
	UIDESC						m_UIDesc;
	
	ID3D11DepthStencilState* m_OldDepthStencilState = nullptr;
	ID3D11DepthStencilState* m_UIdepthStencilState = nullptr;

	UINT                        m_StencilRef = 0;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

};

END