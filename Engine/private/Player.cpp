#include "..\public\Player.h"


CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	ZeroMemory(&m_PlayerDesc, sizeof(m_PlayerDesc));

	if(nullptr != pArg)
		memcpy(&m_PlayerDesc, pArg, sizeof(m_PlayerDesc));

	if (FAILED(__super::Initialize(&m_PlayerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Last_Initialize()
{
	return E_NOTIMPL;
}

void CPlayer::Tick(_double TimeDelta)
{
}

void CPlayer::Late_Tick(_double TimeDelta)
{
}

HRESULT CPlayer::Render()
{
	return S_OK;
}


void CPlayer::Free()
{
	__super::Free();
}
