#include "..\public\Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "Texture.h"


CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // _DEBUG
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG

}

HRESULT CNavigation::Initialize_Prototype(const _tchar * pNavigationDataFilePath)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pNavigationDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[CCell::POINT_END];

	while (true)
	{
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.push_back(pCell);
	}

	CloseHandle(hFile);

	if (FAILED(Ready_Neighbor()))
		return E_FAIL;

#ifdef _DEBUG

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		memcpy(&m_NaviDesc, pArg, sizeof(NAVIDESC));
	}

	
	/*m_vOldPos.x = m_Cells[0]->Get_Point(CCell::POINT_A).x;
	m_vOldPos.y = m_Cells[0]->Get_Point(CCell::POINT_A).y;
	m_vOldPos.z = m_Cells[0]->Get_Point(CCell::POINT_A).z;
	m_vOldPos.w = 1.f;*/
	return S_OK;
}



_bool CNavigation::isMove_OnNavigation(_fvector TargetPos)
{
	if (-1 == m_NaviDesc.iCurrentIndex)
		return false;

	_int		iNeighborIndex = -1;

	/* 움직이고 난 결과위치가 쎌 안에 있다면.  */
	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(TargetPos, &m_vOldPos, &iNeighborIndex))
	{
		return true; // 움직여. 
	}				 /* 움직이고 난 결과위치가 이쎌을 벗어난다면. */
	else
	{
		/* 나간방향으로 이웃이 있었다면ㄴ. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
				{
					return false;
				}

				if (true == m_Cells[iNeighborIndex]->isIn(TargetPos, &m_vOldPos, &iNeighborIndex))
				{
					// m_NaviDesc.iCurrentIndex = 이웃의 인덱스;
					m_NaviDesc.iCurrentIndex = iNeighborIndex;
					return true;
				}
			}
		}
		/* 나간방향으로 이웃이 없었다면 */
		else
		{
			return false;
		}
	}

}

_bool CNavigation::isMove_OnNavigation_test(_float4& TargetPos)
{
	if (-1 == m_NaviDesc.iCurrentIndex)
		return false;

	_int		iNeighborIndex = -1;

	/* 움직이고 난 결과위치가 쎌 안에 있다면.  */
	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(XMLoadFloat4(&TargetPos), &m_vOldPos, &iNeighborIndex))
	{
		m_vOldPos = TargetPos;
		return true; // 움직여. 
	}				 /* 움직이고 난 결과위치가 이쎌을 벗어난다면. */
	else
	{
		/* 나간방향으로 이웃이 있었다면ㄴ. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
				{
					return false;
				}
				

				if (true == m_Cells[iNeighborIndex]->isIn(XMLoadFloat4(&TargetPos), &m_vOldPos, &iNeighborIndex))
				{
					// m_NaviDesc.iCurrentIndex = 이웃의 인덱스;
					m_vOldPos = TargetPos;
					m_NaviDesc.iCurrentIndex = iNeighborIndex;
					return true;
				}
				
				
			}
		}
		/* 나간방향으로 이웃이 없었다면 */
		else
		{
			if (false == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(XMLoadFloat4(&TargetPos), &m_vOldPos, &iNeighborIndex))
			{
				XMStoreFloat4(&TargetPos, XMLoadFloat4(&m_vOldPos));
				return true;
			}


			return false;
		}
	}
}

_bool CNavigation::isMove_OnNavigation_Chase(_float4 & TargetPos)
{
	if (-1 == m_NaviDesc.iCurrentIndex)
		return false;

	_int		iNeighborIndex = -1;

	/* 움직이고 난 결과위치가 쎌 안에 있다면.  */
	if (true == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(XMLoadFloat4(&TargetPos), &iNeighborIndex))
	{
		return true; // 움직여. 
	}				 /* 움직이고 난 결과위치가 이쎌을 벗어난다면. */
	else
	{
		/* 나간방향으로 이웃이 있었다면ㄴ. */
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
				{
					return false;
				}

				if (true == m_Cells[iNeighborIndex]->isIn(XMLoadFloat4(&TargetPos), &iNeighborIndex))
				{
					// m_NaviDesc.iCurrentIndex = 이웃의 인덱스;
					m_NaviDesc.iCurrentIndex = iNeighborIndex;
					return true;
				}
			}
		}
		/* 나간방향으로 이웃이 없었다면 */
		else
		{
			if (false == m_Cells[m_NaviDesc.iCurrentIndex]->isIn(XMLoadFloat4(&TargetPos), &iNeighborIndex))
			{
				XMStoreFloat4(&TargetPos, XMLoadFloat4(&m_vOldPos));
				return true;
			}
			return false;
		}
	}
}

void CNavigation::Update_OldPos()
{
	m_vOldPos.x = (m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A).x + m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B).x  + m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C).x )/3.f;
	m_vOldPos.y = (m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A).y + m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B).y + m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C).y) / 3.f;
	m_vOldPos.z = (m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_A).z + m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_B).z + m_Cells[m_NaviDesc.iCurrentIndex]->Get_Point(CCell::POINT_C).z) / 3.f;
}


void CNavigation::AddCell(_float3* vPoints)
{
	CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_int)m_Cells.size());
	if (nullptr == pCell)
		assert("CNavigation::AddCell");

	m_Cells.push_back(pCell);

	if (FAILED(Ready_Neighbor()))
		MSG_BOX("AddCell_Error");


}

void CNavigation::Delete_Navi()
{
	m_Cells.erase(m_Cells.end() - 1);
}


#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	_float		fHeight = 0.0f;

	if (-1 == m_NaviDesc.iCurrentIndex)
	{
		fHeight = 0.0f;
		HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
	}
	else
	{
		fHeight = 0.3f;
		HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
		m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

	

		m_Cells[m_NaviDesc.iCurrentIndex]->Render(m_pShader);

		return S_OK;
	}

	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Render(m_pShader);
	}

	return S_OK;
}
#endif // _DEBUG
/* 네비게이션을 구성하는 쎌들의 이웃을 설정한다. */
HRESULT CNavigation::Ready_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_AB, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_BC, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_CA, pDestCell);
			}
		}
	}

	return S_OK;
}



CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pNavigationDataFilePath)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pNavigationDataFilePath)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif // _DEBUG
}

void CNavigation::Set_SaveSort_NavigatorVector(vector<_float3>* SortVec)
{
	for (auto& pCell : m_Cells)
	{
		for (_uint i = 0; i < 3; ++i)
		{
			SortVec->push_back(pCell->Get_Point(CCell::POINT(i)));
		}
	}

}

void CNavigation::Set_OldPos(_float4 vPos)
{
	m_vOldPos = vPos;


}
