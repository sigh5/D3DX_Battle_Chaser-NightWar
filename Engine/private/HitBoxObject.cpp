#include "..\public\HitBoxObject.h"



CHitBoxObject::CHitBoxObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext), m_isCloned(false)
{
}

CHitBoxObject::CHitBoxObject(const CHitBoxObject & rhs)
	: CGameObject(rhs), m_isCloned(true)
{
}

HRESULT CHitBoxObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHitBoxObject::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CHitBoxObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CHitBoxObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CHitBoxObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHitBoxObject::Last_Initialize()
{
	return S_OK;
}


void CHitBoxObject::Free()
{
	__super::Free();
}
