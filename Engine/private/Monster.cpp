#include "..\public\Monster.h"

#include "Model.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCombatActors(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CCombatActors(rhs)
{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMonster::Last_Initialize()
{
	return S_OK;
}

void CMonster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMonster::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMonster::Render()
{
	return S_OK;
}

void CMonster::Free()
{
	__super::Free();
}
