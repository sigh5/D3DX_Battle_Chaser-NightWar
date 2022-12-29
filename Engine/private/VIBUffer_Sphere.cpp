#include "..\public\VIBUffer_Sphere.h"



CVIBUffer_Sphere::CVIBUffer_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice,pContext)
{
}

CVIBUffer_Sphere::CVIBUffer_Sphere(const CVIBUffer_Sphere & rhs)
	:CVIBuffer(rhs)
{
}

HRESULT CVIBUffer_Sphere::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	
	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_iNumPrimitive = 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXTEX*			pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX));

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[0].vTexUV = _float2(0.0f, 0.f);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[1].vTexUV = _float2(1.0f, 0.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[2].vTexUV = _float2(1.0f, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[3].vTexUV = _float2(0.0f, 1.f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


#pragma endregion

	return S_OK;
}

HRESULT CVIBUffer_Sphere::Initialize(void * pArg)
{
	return S_OK;
}

_bool CVIBUffer_Sphere::PickingBuffer(HWND hWnd, CTransform * pCubeTransCom, _float4 * vPosition)
{
	return false;
}

CVIBUffer_Sphere * CVIBUffer_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBUffer_Sphere*		pInstance = new CVIBUffer_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBUffer_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBUffer_Sphere::Clone(void * pArg)
{
	CVIBUffer_Sphere*		pInstance = new CVIBUffer_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBUffer_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CVIBUffer_Sphere::Free()
{
	__super::Free();

}
