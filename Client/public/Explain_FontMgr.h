#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CExplain_Font;

class CExplain_FontMgr final : public CBase
{
	DECLARE_SINGLETON(CExplain_FontMgr);

public:
	CExplain_FontMgr();
	virtual ~CExplain_FontMgr() = default;

public:
	void		 Set_Explain_Font(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		Set_Explain_Font2(_float4 vPos, _float3 vScale, const _tchar* szName);

	
	void		 Reset_Explain_Font();


public:
	HRESULT		 Initialize();			// vector 0~10 ¿˙¿Â
	void		 Tick(_double TimeDelta);
	void		 Late_Tick(_double TimeDelta);

private:
	std::vector<CExplain_Font*>		m_vecExplainFont;
	_float							m_fFontTimer = 0.f;
	_float4							m_vFontFirstPos;
	_float3							m_vScale;
	_bool							m_bFontUpdate = false;
	_bool							m_bInit = false;
public:
	virtual void Free()override;
};

END