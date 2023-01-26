#pragma once
#include "Client_Defines.h"
#include "Base.h"



BEGIN(Engine)
class CGameObject;

END

BEGIN(Client)

class CDamageFont;
class CDamage_Font_Manager final: public CBase
{
	DECLARE_SINGLETON(CDamage_Font_Manager);


private:
	CDamage_Font_Manager();
	virtual ~CDamage_Font_Manager() = default;

public:
	HRESULT		Initialize();			// vector 0~10 ¿˙¿Â
	void		 Tick(_double TimeDelta);	
	
	void		Set_Damage_Target0_Font(_float4 vPos, _float3 vScale, _int iDamage, _float vSecontPosX = 0.5f, _float vSecontPosY = 1.0f);

	void		Set_Damage_Target1_Font(_float4 vPos, _float3 vScale, _int iDamage, _float vSecontPosX = 0.5f, _float vSecontPosY = 1.0f);
	void		Set_Damage_Target2_Font(_float4 vPos, _float3 vScale, _int iDamage, _float vSecontPosX = 0.5f, _float vSecontPosY = 1.0f);



	void		Set_HPMPFont_0(_float4 vPos, _float3 vScale, _int Damage);
	/*void		Set_HPMPFont_1(_float4 vPos, _float3 vScale, _int Damage);
	void		Set_HPMPFont_2(_float4 vPos, _float3 vScale, _int Damage);*/
	void		Late_Tick(_double TimeDelta);

private:
	std::vector<CDamageFont*>  m_DamageTenVec;  // 0 ~ 10
	std::vector<CDamageFont*>  m_DamageSingleVec;  // 0 ~ 10

	std::vector<CDamageFont*>  m_Target2_DamageTenVec;  // 0 ~ 10
	std::vector<CDamageFont*>  m_Target2_DamageSingleVec;  // 0 ~ 10

	std::vector<CDamageFont*>  m_Target3_DamageTenVec;  // 0 ~ 10
	std::vector<CDamageFont*>  m_Target3_DamageSingleVec;  // 0 ~ 10


	_bool						m_bInit = false;

	_int						m_iCalDamage = 0;
	_int						m_iNumTens = 0;
	_int						m_iNumSingle = 0;
	
	_float4						m_vFontFirstPos;
	_float4						m_vFontSecondPos;
	_float3						m_vScale;
	
	_bool						m_bTarget0_FontUpdate = false;
	_bool						m_bTarget1_FontUpdate = false;
	_bool						m_bTarget2_FontUpdate = false;

	_int						m_iCalDamage1 = 0;
	_int						m_iNumTens1 = 0;
	_int						m_iNumSingle1 = 0;
	_float4						m_vFontFirstPos1;
	_float4						m_vFontSecondPos1;
	_float3						m_vScale1;


	_int						m_iCalDamage2 = 0;
	_int						m_iNumTens2 = 0;
	_int						m_iNumSingle2 = 0;
	_float4						m_vFontFirstPos2;
	_float4						m_vFontSecondPos2;
	_float3						m_vScale2;



	_bool						m_bShaking = false;
	
	_float						m_fFont_Target0_Timer = 0.f;
	_float						m_fFont_Target1_Timer = 0.f;
	_float						m_fFont_Target2_Timer = 0.f;

	/*ForImgui*/
	_float							m_fBetween = 1.f;

public:
	virtual void Free()override;


};

END