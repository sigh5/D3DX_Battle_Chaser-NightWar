#include "..\public\VIBuffer_Instancing.h"
#include "GameInstance.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing & rhs)
	: CVIBuffer(rhs)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_pInstanceBuffer(rhs.m_pInstanceBuffer)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
{
	Safe_AddRef(m_pInstanceBuffer);
}

HRESULT CVIBuffer_Instancing::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 정점 버퍼들을 장치에 바인딩하는 것 복수바인딩 가능*/
	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		m_pInstanceBuffer
	};

						//정점 한개의크기
	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

				//어디서 부터 시작할 것인가? 거의 0부터
	_uint					iOffsets[] = {
		0,
		0,
	};

	CONTEXT_LOCK
	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/*인덱스버퍼를 장치에 바인딩한다.(단일로 바인딩한다.)  */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/*m_pContext->DrawIndexed(m_iNumIndices, 0, 0);*/
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}



void CVIBuffer_Instancing::Free()
{
	__super::Free();
	Safe_Release(m_pInstanceBuffer);
}
