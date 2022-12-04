#include "..\public\Environment_Object.h"

CEnvironment_Object::CEnvironment_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CEnvironment_Object::CEnvironment_Object(const CEnvironment_Object & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnvironment_Object::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnvironment_Object::Initialize(void * pArg)
{
	ZeroMemory(&m_EnviromentDesc, sizeof(ENVIRONMENTDESC));

	if (pArg != nullptr)
		memcpy(&m_EnviromentDesc, pArg,sizeof(ENVIRONMENTDESC));

	if (FAILED(__super::Initialize(&m_EnviromentDesc.TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnvironment_Object::Last_Initialize()
{
	return S_OK;
}

void CEnvironment_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CEnvironment_Object::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CEnvironment_Object::Render()
{
	return S_OK;
}


void CEnvironment_Object::Free()
{
	__super::Free();
}
