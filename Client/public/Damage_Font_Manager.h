#pragma once
#include "Client_Defines.h"
#include "Base.h"



BEGIN(Engine)
class CGameObject;

END

BEGIN(Client)

class CDamageFont;
class CDamage_Font_Manager : public CBase
{
	DECLARE_SINGLETON(CDamage_Font_Manager);


private:
	CDamage_Font_Manager();
	virtual ~CDamage_Font_Manager() = default;

public:
	HRESULT		Initialize();			// vector 0~10 ¿˙¿Â
	void		 Tick(_double TimeDelta);	
	
	void		Set_DamageFont(_float4 vPos, _float3 vScale , _int Damage);

	void		Late_Tick(_double TimeDelta);

private:
	std::vector<CDamageFont*>  m_DamageTenVec;  // 0 ~ 10
	std::vector<CDamageFont*>  m_DamageSingleVec;  // 0 ~ 10

	_bool						m_bInit = false;

	_int						m_iCalDamage = 0;
	_int						m_iNumTens = 0;
	_int						m_iNumSingle = 0;
	_float4						m_vFontFirstPos;
	_float4						m_vFontSecondPos;
	_float3						m_vScale;
	_bool						m_bFontUpdate = false;
	_float						m_fFontTimer = 0.f;

	/*ForImgui*/
	_float							m_fBetween = 1.f;

public:
	virtual void Free()override;


};

END