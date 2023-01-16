#include "..\public\Status.h"


CStatus::CStatus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice, pContext)
{
}

CStatus::CStatus(const CStatus & rhs)
	: CComponent(rhs)
{
}

void CStatus::Incrase_Hp(_int iAmount)
{
	_int iSum = iAmount + m_StatusDesc.iHp;

	if (iSum >= m_iMaxHp)
		m_StatusDesc.iHp = m_iMaxHp;
	else
		m_StatusDesc.iHp = iSum;
}

void CStatus::Incrase_Mp(_int iAmount)
{
	_int iSum = iAmount + m_StatusDesc.iMp;

	if (iSum >= m_iMaxMp)
		m_StatusDesc.iMp = m_iMaxMp;
	else
		m_StatusDesc.iMp = iSum;
}

_float CStatus::Get_CurStatusHpRatio()
{
	return (_float)(m_StatusDesc.iHp) / (_float)(m_iMaxHp);
}

_float CStatus::Get_CurStatusMpRatio()
{
	return	(_float)(m_StatusDesc.iMp) / (_float)(m_iMaxMp);
}

void CStatus::Take_Damage(_int iDamgae)
{
	m_StatusDesc.iHp -= iDamgae;

	if (m_StatusDesc.iHp <= 0)
		m_StatusDesc.iHp = 0;

}

void CStatus::Use_SkillMp(_int iMp)
{
	m_StatusDesc.iMp -= iMp;
	if (m_StatusDesc.iMp <= 0)
		m_StatusDesc.iMp = 0;
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

void CStatus::Final_Update()
{
	if (m_StatusDesc.iHp <= 0)
	{
		m_bDead = true;
		m_StatusDesc.iMp = 0;
	}
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
