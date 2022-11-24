#pragma once
#include "Base.h"

BEGIN(Engine)

class CCalculator : public CBase
{
	DECLARE_SINGLETON(CCalculator)
private:
	CCalculator();
	virtual ~CCalculator() = default;
	
public:
	virtual HRESULT Ready_Calculator();

public:
	_vector		MovePicking_Point(class CGameObject* pMoveObject, class CGameObject* pTerrainObject);


public:
	virtual void Free() override;


};

END