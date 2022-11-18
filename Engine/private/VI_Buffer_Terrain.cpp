#include "..\public\VI_Buffer_Terrain.h"


CVI_Buffer_Terrain::CVI_Buffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVI_Buffer_Terrain::CVI_Buffer_Terrain(const CVI_Buffer_Terrain & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVI_Buffer_Terrain::Initialize_Prototype(const wstring & terrainFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(terrainFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		assert("CVI_Buffer_Terrain::Initialize_Prototype File");

	BITMAPFILEHEADER			fh;
	BITMAPINFOHEADER			ih;
	_ulong*						pPixel = nullptr;

	ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iNumVertices = m_iNumVerticesZ*m_iNumVerticesX;

	pPixel = new _ulong[m_iNumVertices];
	ZeroMemory(pPixel, sizeof(_ulong) * m_iNumVertices);

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVertices, &dwByte, nullptr);
	CloseHandle(hFile);

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);

	m_iNumPrimitive = (m_iNumVerticesZ - 1)*(m_iNumVerticesX - 1) * 2;
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

	VTXNORTEX*			pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX));

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i* m_iNumVerticesX + j;

			//	11111111 11111011 11111011 11111011	ARGB 순이다. 16진수이다.
			//& 00000000 00000000 00000000 11111111	& 연산 시 제일 마지막이 B다.
			//  다른쪽을 and 연산하면 16진수라 숫자가 커서 B로하는게 속도적으로 이득이다.

			pVertices[iIndex].vPosition = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 15.f, (_float)i);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));
		}
	}
	
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

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint		iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint			iIndex = m_iNumVerticesX*i + j;

			_uint			iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];
			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];
			++iNumFaces;
		}
	}



	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pPixel);

#pragma endregion

	return S_OK;
}

HRESULT CVI_Buffer_Terrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CVI_Buffer_Terrain * CVI_Buffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & terrainFilePath)
{
	CVI_Buffer_Terrain*		pInstance = new CVI_Buffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(terrainFilePath)))
	{
		MSG_BOX("Failed to Created : CVI_Buffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVI_Buffer_Terrain::Clone(void * pArg)
{
	CVI_Buffer_Terrain*		pInstance = new CVI_Buffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVI_Buffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVI_Buffer_Terrain::Free()
{
	__super::Free();
}
