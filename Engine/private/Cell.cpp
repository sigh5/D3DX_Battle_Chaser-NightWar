#include "..\public\Cell.h"
#include "VIBuffer_Cell.h"
#include "Shader.h"
#include "PipeLine.h"
#include "GameInstance.h"
CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_iIndex{ 0 }
	, m_vPoints{ _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f) }
	, m_iNeighborIndices{ -1, -1, -1 }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	m_iIndex = iIndex;
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);


#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

/* 내 세개의 점들 중, 전달받은 두개의 점과 같은 점이 있는지를 체크. */
_bool CCell::Compare_Points(const _float3 & SourPoint, const _float3 & DestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), XMLoadFloat3(&SourPoint)))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), XMLoadFloat3(&DestPoint)))
			return true;

		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), XMLoadFloat3(&DestPoint)))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vTargetPos, _int* pNeighborIndex)
{
	for (_uint i = 0; i < NEIGHBOR_END; ++i)
	{
		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % NEIGHBOR_END]) - XMLoadFloat3(&m_vPoints[i]);
		_vector		vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));
		_vector		vDir = XMVector3Normalize(vTargetPos - XMLoadFloat3(&m_vPoints[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}


#ifdef _DEBUG
HRESULT CCell::Render(CShader * pShader,_uint iShaderPass)
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;


	_float4x4		WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	pShader->Set_Matrix("g_WorldMatrix", &WorldMatrix);

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	pShader->Set_Matrix("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	pShader->Set_Matrix("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));


	RELEASE_INSTANCE(CPipeLine);

	pShader->Begin(iShaderPass);

	m_pVIBuffer->Render();

	return S_OK;
}
#endif // _DEBUG


_bool CCell::isIn(_fvector vTargetPos, _float4* vOldPos, _int * pNeghborIndex)
{
	for (_uint i = 0; i < NEIGHBOR_END; ++i)
	{
		/* 삼각형의 변 */
		_vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % NEIGHBOR_END]) - XMLoadFloat3(&m_vPoints[i]);

		/* 변의 법선 */
		_vector   vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));

		/* 꼭지점에서 타겟위치로 향하는 벡터 */
		_vector vDir = XMVector3Normalize(vTargetPos - XMLoadFloat3(&m_vPoints[i]));

		/* 0 ~ 90 혹은 270 ~ 360 사이의 각도라면 cos은 양수이다. 즉, 해당 선분 밖으로 벗어났다. */
		if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
		{
			*pNeghborIndex = m_iNeighborIndices[i];

			/* 만약 이웃이 없다면 슬라이딩 벡터를 구한다. */
			if (-1 == m_iNeighborIndices[i])
			{
				_vector vOld = XMLoadFloat4(vOldPos);
				_vector vPosDir = vTargetPos - vOld;
				_float   fDot = abs(XMVectorGetX(XMVector3Dot(vPosDir, vNormal)));
				_vector   vSliding = vPosDir - fDot*vNormal;

				_vector vOrigin = vOld;
				vOld += vSliding;

				/* 이 슬라이딩의 결과가 cell을 벗어나서는 안됨... */
				_float fLength = XMVectorGetX(XMVector3Length(vLine));
				_float fLength1 = XMVectorGetX(XMVector3Length(vOld - XMLoadFloat3(&m_vPoints[i])));
				_float fLength2 = XMVectorGetX(XMVector3Length(vOld - XMLoadFloat3(&m_vPoints[(i + 1) % NEIGHBOR_END])));

				if (fLength < fLength1 || fLength < fLength2) // i+1지점 밖이다.
					vOld = vOrigin;

				XMStoreFloat4(vOldPos, vOld);
			}

			return false;
		}
	}

	return true;
}


CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif
}
