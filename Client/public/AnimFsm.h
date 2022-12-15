#pragma once

#include "Client_Defines.h"
#include "Animation.h"
#include "FSMComponent.h"

BEGIN(Engine)
class CGameObject;

END

BEGIN(Client)

class CAnimFsm : public CFSMComponent
{
private:
	CAnimFsm();
	virtual ~CAnimFsm()=default;

public:
	HRESULT Initialize(CGameObject *pTarget, AnimType eType);

private:
	HRESULT Init_Knolan(CGameObject *pTarget);
	HRESULT Init_Garrison(CGameObject *pTarget);
	HRESULT Init_Calibretto(CGameObject *pTarget);



private:
	class CGameObject*				m_pTarget = nullptr;
	class CPlayerController*		m_pPlayerController = nullptr;
	class CCombatController*		m_pCombatController = nullptr;
public:
	static	CAnimFsm*		Create(CGameObject *pTarget, AnimType eType);
	
	virtual	void			Free()override;
};


END