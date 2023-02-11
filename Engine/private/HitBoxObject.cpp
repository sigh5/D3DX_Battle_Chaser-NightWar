#include "..\public\HitBoxObject.h"



CHitBoxObject::CHitBoxObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext), m_isCloned(false)
{
}

CHitBoxObject::CHitBoxObject(const CHitBoxObject & rhs)
	: CGameObject(rhs), m_isCloned(true)
{
}

_uint CHitBoxObject::Get_WeaponOption()
{
	return 0;
}

void CHitBoxObject::Set_WeaponOption(_uint iOption)
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
	ZeroMemory(&m_HitBoxDesc, sizeof(m_HitBoxDesc));

	if (pArg != nullptr)
		memcpy(&m_HitBoxDesc, pArg, sizeof(m_HitBoxDesc));

	if (FAILED(__super::Initialize(&m_HitBoxDesc.TransformDesc)))
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
