#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CExplain_Font;
class CDebuff_Font;

class CExplain_FontMgr final : public CBase
{
	DECLARE_SINGLETON(CExplain_FontMgr);

public:
	CExplain_FontMgr();
	virtual ~CExplain_FontMgr() = default;

public:
	void		 Set_Explain_Target0_Font0(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Set_Explain_Target0_Font1(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Set_Debuff_Target0_Font(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Reset_Explain_Target0_Font0();
	void		 Reset_Explain_Target0_Font1();
	void		 Reset_Debuff_Target0_Font();


	void		 Set_Explain_Target1_Font0(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Set_Explain_Target1_Font1(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Set_Debuff_Target1_Font(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Reset_Explain_Target1_Font0();
	void		 Reset_Explain_Target1_Font1();
	void		 Reset_Debuff_Target1_Font();

	void		 Set_Explain_Target2_Font0(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Set_Explain_Target2_Font1(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Set_Debuff_Target2_Font(_float4 vPos, _float3 vScale, const _tchar* szName);
	void		 Reset_Explain_Target2_Font0();
	void		 Reset_Explain_Target2_Font1();
	void		 Reset_Debuff_Target2_Font();



public:
	HRESULT		 Initialize();			// vector 0~10 ¿˙¿Â
	void		 Tick(_double TimeDelta);
	void		 Late_Tick(_double TimeDelta);

private:
	/*First_Target*/
	std::vector<CExplain_Font*>		m_vecExplainTarget0_Font0;
	std::vector<CExplain_Font*>		m_vecExplainTarget0_Font1;
	std::vector<CDebuff_Font*>		m_vecTarget0_DeBuffFont;

	_float							m_fTarget0_FontTimer = 0.f;
	_bool							m_bTarget0_FontExplain_Update = false;
	
	_bool							m_bTarget0_Font1Explain_Update = false;
	_float							m_fTarget0_Font1_Timer = 0.f;
	
	_bool							m_bTarget0_FontDebuff_Update = false;
	_float							m_fTarget0_DebuffFontTimer = 0.f;
	/*~First_Target*/

	/*Second_Target*/
	std::vector<CExplain_Font*>		m_vecExplainTarget1_Font0;
	std::vector<CExplain_Font*>		m_vecExplainTarget1_Font1;
	std::vector<CDebuff_Font*>		m_vecTarget1_DeBuffFont;

	_float							m_fTarget1_FontTimer = 0.f;
	_bool							m_bTarget1_FontExplain_Update = false;

	_bool							m_bTarget1_Font1Explain_Update = false;
	_float							m_fTarget1_Font1_Timer = 0.f;

	_bool							m_bTarget1_FontDebuff_Update = false;
	_float							m_fTarget1_DebuffFontTimer = 0.f;
	/*~Second_Target*/

	/*Third_Target*/
	std::vector<CExplain_Font*>		m_vecExplainTarget2_Font0;
	std::vector<CExplain_Font*>		m_vecExplainTarget2_Font1;
	std::vector<CDebuff_Font*>		m_vecTarget2_DeBuffFont;

	_float							m_fTarget2_FontTimer = 0.f;
	_bool							m_bTarget2_FontExplain_Update = false;

	_bool							m_bTarget2_Font1Explain_Update = false;
	_float							m_fTarget2_Font1_Timer = 0.f;

	_bool							m_bTarget2_FontDebuff_Update = false;
	_float							m_fTarget2_DebuffFontTimer = 0.f;
	/*~Third_Target*/



	
	
	
private:	
	_float4							m_vFontFirstPos;
	_bool							m_bInit = false;
public:
	virtual void Free()override;
};

END