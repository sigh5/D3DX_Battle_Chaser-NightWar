#include "..\public\Status.h"


CStatus::CStatus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice, pContext)
{
}

CStatus::CStatus(const CStatus & rhs)
	: CComponent(rhs)
{
}

_float CStatus::Get_CurStatusHpRatio()
{
	return (_float)(m_StatusDesc.iHp) / (_float)(m_iMaxHp);
}

_float CStatus::Get_CurStatusMpRatio()
{
	return	 (_float)(m_StatusDesc.iMp) / (_float)(m_iMaxMp);
}

HRESULT CStatus::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStatus::Initialize(void * pArg)
{
	ZeroMemory(&m_StatusDesc, sizeof(m_StatusDesc));
	
	if (pArg != nullptr)
	{
		memcpy(&m_StatusDesc, pArg, sizeof(m_StatusDesc));
		m_iMaxHp = m_StatusDesc.iHp;
		m_iMaxMp = m_StatusDesc.iMp;
	}
	else
		MSG_BOX("??");

	return S_OK;
}

CStatus * CStatus::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStatus*		pInstance = new CStatus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CStatus");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CStatus::Clone(void * pArg)
{
	CStatus*		pInstance = new CStatus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CStatus");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CStatus::Free()
{
	__super::Free();

}
