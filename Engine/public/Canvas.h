#pragma once
#include "UI.h"

BEGIN(Engine)

class ENGINE_DLL CCanvas abstract :public CUI
{
public:
	typedef struct tag_CanvasDesc : public UIDESC
	{
		UIDESC		UiDesc;
	}CANVASDESC;


protected:
	CCanvas(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CCanvas(const CCanvas& rhs);
	virtual ~CCanvas() = default;

public:
	void	Set_Child_UI_Shaking(_bool bShaking) { m_bChild_UI_Shaking = bShaking; }

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	
public:
	virtual void Set_ParentLoad(CUI * pUI)override {}
	virtual void Delete_Delegate();
	virtual void	Shaking_Child_UI();


public: /* Imgui */
	virtual _bool Piciking_GameObject()override { return false; }

	virtual const _tchar*		Get_TextureTag()const { return m_CanvasDesc.m_pTextureTag; }
	virtual void				Set_TextureTag(const _tchar* pTextureTag) { lstrcpy(m_CanvasDesc.m_pTextureTag, pTextureTag); };
public:
	virtual		HRESULT	Add_ChildUI(CUI* pUI)sealed;

protected:
	vector<CUI*>		m_ChildrenVec;
	CANVASDESC			m_CanvasDesc;

	_bool				m_bChild_UI_Shaking = false;


public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;


};

END