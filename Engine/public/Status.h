#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStatus final: public CComponent
{
public:
	typedef struct tag_StatusDesc
	{
		_int		iHp;	
		_int		iMp;		//나중에 치명타 , 돈 이런거 넣기
	}StatusDesc;

protected:
	CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

public:
	_float			Get_CurStatusHpRatio();
	_float			Get_CurStatusMpRatio();

	void			Take_Damage(_int iDamgae);	
	void			Use_SkillMp(_int iMp);

	_bool			Get_Dead()const { return m_bDead; }

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual	void	Final_Update()override;

private:
	StatusDesc			m_StatusDesc;
	_int				m_iMaxHp = 0;
	_int				m_iMaxMp = 0;
	_bool				m_bDead = false;


public:
	static CStatus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END