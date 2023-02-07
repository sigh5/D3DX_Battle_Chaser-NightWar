#pragma once
#include "Client_Defines.h"
#include "FSMComponent.h"

BEGIN(Engine)
class CGameObject;

END

BEGIN(Client)
class CBossFsm final :public CFSMComponent
{
public:
	CBossFsm();
	virtual ~CBossFsm() = default;
public:
	HRESULT Initialize(CGameObject *pTarget);

private:
	HRESULT Init_Boss(CGameObject *pTarget);

private:
	class   CGameObject*				m_pTarget = nullptr;
	class   CCombatController*		m_pCombatController = nullptr;
public:
	static	CBossFsm*	Create(CGameObject *pTarget);
	virtual	void			Free()override;
};

END
