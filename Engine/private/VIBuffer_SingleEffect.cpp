#include "..\public\VIBuffer_SingleEffect.h"


CVIBuffer_SingleEffect::CVIBuffer_SingleEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_SingleEffect::CVIBuffer_SingleEffect(const CVIBuffer_SingleEffect & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_SingleEffect::Initialize_Prototype(_)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 1;
	m_iNumPrimitive = 1;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesPerPrimitive = 1;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXTEX*			pVertices = new VTXTEX;
	ZeroMemory(pVertices, sizeof(VTXTEX));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.0f);		// 처음에 다 원점으로 고정
	pVertices->vTexUV = _float2(0.2f, 0.2f);				// 지름이라고 생각하면 편함 실제 전체사이즈의크기

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX));

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(_float(rand() % 5), 3.0f, _float(rand() % 5), 1.f);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	Safe_Delete_Array(pInstanceVertices);


	return S_OK;
}

HRESULT CVIBuffer_SingleEffect::Initialize(void * pArg)
{
	return E_NOTIMPL;
}

CVIBuffer_SingleEffect * CVIBuffer_SingleEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iNumInstance)
{
	return nullptr;
}

CComponent * CVIBuffer_SingleEffect::Clone(void * pArg)
{
	return nullptr;
}

void CVIBuffer_SingleEffect::Free()
{
}
