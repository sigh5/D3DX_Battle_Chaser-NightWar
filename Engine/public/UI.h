#pragma once
#include "GameObject.h"





BEGIN(Engine)


class ENGINE_DLL CUI abstract : public CGameObject
{
public:
	typedef struct tag_UIDesc :public GAMEOBJECTDESC
	{
	 _tchar			pBroadCasterTag[MAX_PATH] =  TEXT("");
	 _tchar			m_pTextureTag[MAX_PATH] = TEXT("");

	}UIDESC;

protected:
	CUI(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	 const _tchar*						Get_BroadCasterTag()const  { return m_UIDesc.pBroadCasterTag; }
	 void								Set_BraodCasterTag(const _tchar* BroadCasterTag) { lstrcpy(m_UIDesc.pBroadCasterTag, BroadCasterTag); };
	  virtual const _tchar*				Get_TextureTag()const { return m_UIDesc.m_pTextureTag; }
	  virtual void						Set_TextureTag(const _tchar* pTextureTag) { lstrcpy(m_UIDesc.m_pTextureTag, pTextureTag); };
	  const		_bool					Get_RenderActive()const { return m_bRenderActive; }
	  virtual  void						Set_RenderActive(_bool bActive) {m_bRenderActive = bActive;  }
	  const _int						Get_Shaking_Option()const { return m_iShakingOption; }
	  void								Set_Shaking_Option(_int iShakingOption) { m_iShakingOption = iShakingOption; }
public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual HRESULT Last_Initialize()override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	virtual void	Change_Texture(_uint iLevel, const wstring& NewComPonentTag) {}

public:
	virtual void	 Set_ParentLoad(CUI * pUI);
	virtual void	Set_parentName(_uint iCulLevel, const _tchar* pParentTag);
	virtual void	Set_HighRightUIDesc(HIGHLIGHT_UIDESC& UIDesc) {};

	void					ReadyShake(_float fDuration, _float fMagnitude)
	{
		m_fCurShakeTime = 0.f;
		m_fShakeTime = fDuration;
		m_fMagnitude = fMagnitude;
	}

#ifdef _DEBUG
public: /* Imgui */
	virtual _bool Piciking_GameObject()override { return false; }


#endif

public:
	virtual void			ShakingControl(_float fCoolTime);

protected:
	virtual void			Shake_Move(_double TimeDelta);
	


protected:
	virtual HRESULT SetUp_UI()sealed;

	virtual void Begin_UI()sealed;
	virtual void End_UI()sealed;

public: /* imgui */
	virtual void Imgui_RenderComponentProperties() {}
	virtual void Imgui_RenderProperty()override {}

protected:
	UIDESC						m_UIDesc;
	_bool						m_bRenderActive = true;			//렌더를 결정 안결정

	// shake
	_float					 m_fShakeTime = 0.f;
	_float					m_fCurShakeTime = 0.f;
	_float					m_fMagnitude = 0.f;
	// ~shake

	_int					m_iShakingOption = 0;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

};

END