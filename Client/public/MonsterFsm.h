#pragma once

#include "Client_Defines.h"
#include "Animation.h"
#include "FSMComponent.h"


BEGIN(Engine)
class CGameObject;

END

BEGIN(Client)
class CMonsterFsm : public CFSMComponent
{
public:
	CMonsterFsm();
	virtual ~CMonsterFsm() = default;

public:
	HRESULT Initialize(CGameObject *pTarget, AnimType eType);

private:
	HRESULT Init_SlimeKing(CGameObject *pTarget);
	HRESULT Init_Skelton_Naked(CGameObject *pTarget);
	HRESULT Init_Spider_Mana(CGameObject *pTarget);

private:
	class CGameObject*				m_pTarget = nullptr;
	class CCombatController*		m_pCombatController = nullptr;

public:
	static	CMonsterFsm*	Create(CGameObject *pTarget, AnimType eType);
	virtual	void			Free()override;
};

END
