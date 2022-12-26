#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStatus final: public CComponent
{
public:
	typedef struct tag_StatusDesc
	{
		_uint		iHp;	
		_uint		iMp;		//나중에 치명타 , 돈 이런거 넣기
	}StatusDesc;

protected:
	CStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStatus(const CStatus& rhs);
	virtual ~CStatus() = default;

public:
	_float			Get_CurStatusHpRatio();
	_float			Get_CurStatusMpRatio();

	void			Take_Damage(_uint iDamgae);
	
	void			Use_SkillMp(_uint iMp);

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual	void	Final_Update()override {}

private:
	StatusDesc			m_StatusDesc;
	_uint				m_iMaxHp = 0;
	_uint				m_iMaxMp = 0;

public:
	static CStatus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END