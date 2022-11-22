#pragma once
#include "GameObject.h"

BEGIN(Engine)


class ENGINE_DLL CUI abstract : public CGameObject
{
public:
	typedef struct tag_UIDesc :public GAMEOBJECTDESC
	{
		CGameObject*	pBroadCaster;
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


public: /* imgui */
	virtual void Imgui_RenderComponentProperties() {}
	virtual void Imgui_RenderProperty()override {}

protected:
	UIDESC				m_UIDesc;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

};

END