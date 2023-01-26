#include "..\public\Status.h"


CStatus::CStatus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice, pContext)
{
}

CStatus::CStatus(const CStatus & rhs)
	: CComponent(rhs)
{
}

void CStatus::Set_DebuffOption(DEBUFFTYPE eType,_bool isFlase)
{
	if (isFlase)
	{
		switch (eType)
		{
		case Engine::CStatus::DEBUFF_FIRE:
			m_StatusDesc.m_tagDesc.isDebuff_FIRE = true;
			break;
		case Engine::CStatus::DEBUFF_BLEED:
			m_StatusDesc.m_tagDesc.isDebuff_BLEED = true;
			break;
		case Engine::CStatus::DEBUFF_ARMOR:
			m_StatusDesc.m_tagDesc.isDebuff_ARMOR = true;
			break;
		case Engine::CStatus::DEBUFF_MAGIC:
			m_StatusDesc.m_tagDesc.isDebuff_MAGIC = true;
			break;
		case Engine::CStatus::BUFF_DAMAGE:
			m_StatusDesc.m_tagDesc.isBuff_Damage = true;
			break;
		case Engine::CStatus::DEBUFF_NONE:
			break;
		default:
			break;
		}
	}
	else
	{
		if (CStatus::DEBUFF_FIRE == eType)
		{
			m_StatusDesc.m_tagDesc.isDebuff_FIRE = false;
		}
		else if (CStatus::DEBUFF_BLEED == eType)
		{
			m_StatusDesc.m_tagDesc.isDebuff_BLEED = false;
		}
		else if (CStatus::DEBUFF_ARMOR == eType)
		{
			m_StatusDesc.m_tagDesc.isDebuff_ARMOR = false;
		}
		else if (CStatus::DEBUFF_MAGIC == eType)
		{
			m_StatusDesc.m_tagDesc.isDebuff_MAGIC = false;
		}
		else if (CStatus::BUFF_DAMAGE == eType)
		{
			m_StatusDesc.m_tagDesc.isBuff_Damage = false;
		}
		else
			return;
		
		/*switch (eType)
		{
		case Engine::CStatus::DEBUFF_FIRE:
			m_StatusDesc.m_tagDesc.isDebuff_FIRE = false;
			break;
		case Engine::CStatus::DEBUFF_BLEED:
			m_StatusDesc.m_tagDesc.isDebuff_BLEED = false;
			break;
		case Engine::CStatus::DEBUFF_ARMOR:
			m_StatusDesc.m_tagDesc.isDebuff_ARMOR = false;
			break;
		case Engine::CStatus::DEBUFF_MAGIC:
			m_StatusDesc.m_tagDesc.isDebuff_MAGIC = false;
			break;
		case Engine::CStatus::BUFF_DAMAGE:
			m_StatusDesc.m_tagDesc.isBuff_Damage = false;
			break;
		case Engine::CStatus::DEBUFF_NONE:
			break;
		default:
			break;
		}*/
	}
	


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

void CStatus::Set_Exp(_int iExp)
{
	m_StatusDesc.iExp += iExp;

	if (m_StatusDesc.iExp >= m_iMaxExp)
	{
		m_StatusDesc.iExp -= m_iMaxExp;

		m_iMaxExp += 10;
		++m_StatusDesc.iLevel;
	}
}

_float CStatus::Get_ExpRatio()
{
	return	(_float)(m_StatusDesc.iExp) / (_float)(m_iMaxExp);
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

void CStatus::Add_ItemID(ITEMID iItemID, _int iNum)
{
	auto iter = find_if(m_mapItem.begin(), m_mapItem.end(), [&](auto MyPair)->bool
	{
		if (MyPair.first == iItemID)
			return true;
		return false;
	});

	if (iter != m_mapItem.end())
	{
		_int iOldNum = m_mapItem[iter->first];
		m_mapItem[iter->first] = iNum + iOldNum;
		return;
	}

	m_mapItem.emplace(iItemID, iNum);	
}

void CStatus::Use_Item(ITEMID iItemID)
{
	auto iter = find_if(m_mapItem.begin(), m_mapItem.end(), [&](auto MyPair)->bool
	{
		if (MyPair.first == iItemID)
			return true;
		return false;
	});
	if (iter == m_mapItem.end())
	{
		return;
	}

	_int iOldNum = m_mapItem[iter->first] - 1;

	if (iOldNum <= 0)
	{
		m_mapItem.erase(iter);
	}

	m_mapItem[iter->first] =  iOldNum;


}

_int CStatus::Rest_iTemNum(ITEMID iItemID)
{
	auto iter = find_if(m_mapItem.begin(), m_mapItem.end(), [&](auto MyPair)->bool
	{
		if (MyPair.first == iItemID)
			return true;
		return false;
	});
	if (iter == m_mapItem.end())
	{
		return -1;
	}

	return iter->second;
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
