#include "..\public\VI_Buffer_Terrain.h"

#include "GameInstance.h"

CVI_Buffer_Terrain::CVI_Buffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVI_Buffer_Terrain::CVI_Buffer_Terrain(const CVI_Buffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_pVtx(rhs.m_pVtx)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
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
	VTXNORTEX*			pVertices = new VTXNORTEX[m_iNumVertices];
	m_pVtx = new _float4[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXNORTEX));

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i* m_iNumVerticesX + j;

			//	11111111 11111011 11111011 11111011	ARGB 순이다. 16진수이다.
			//& 00000000 00000000 00000000 11111111	& 연산 시 제일 마지막이 B다.
			//  다른쪽을 and 연산하면 16진수라 숫자가 커서 B로하는게 속도적으로 이득이다.

			//pVertices[iIndex].vPosition = _float3((_float)j, (pPixel[iIndex] & 0x000000ff) / 100.f, (_float)i);
			pVertices[iIndex].vPosition = _float3((_float)j, 0.f, (_float)i);
			m_pVtx[iIndex] = _float4(pVertices[iIndex].vPosition.x, pVertices[iIndex].vPosition.y, pVertices[iIndex].vPosition.z, 1.f);
			pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));
		}
	}


	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEX_BUFFER
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

			_vector		vSour, vDest, vNormal;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[1];
			pIndices[iNumFaces]._2 = iIndices[2];
			
			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) -  XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));
			
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;

			pIndices[iNumFaces]._0 = iIndices[0];
			pIndices[iNumFaces]._1 = iIndices[2];
			pIndices[iNumFaces]._2 = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;
		}
	}

#pragma region VERTEX

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

#pragma endregion VERTEX

#pragma region INDEX
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

#pragma endregion INDEX



	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);


#pragma endregion

	return S_OK;
}

HRESULT CVI_Buffer_Terrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_float4 CVI_Buffer_Terrain::PickingTerrain(HWND hWnd, CTransform * pCubeTransCom)
{
	CGameInstance* pGameIntance = GET_INSTANCE(CGameInstance);

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		vPoint;

	D3D11_VIEWPORT		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
	ViewPort.Width = 1280;
	ViewPort.Height = 720;


	vPoint.x = ptMouse.x / (1280 * 0.5f) - 1.f;
	vPoint.y = ptMouse.y / -(720 * 0.5f) + 1.f;
	vPoint.z = 1.f;
	vPoint.w = 1.f;

	_matrix		matProj;
	matProj = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matProj));

	_matrix		matView;
	matView = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matView));

	_float4		vRayPos;
	memcpy(&vRayPos, &matView.r[3], sizeof(_float4));
	_float4		vRayDir;
	XMStoreFloat4(&vRayDir, (XMLoadFloat4(&vPoint) - XMLoadFloat4(&vRayPos)));


	_matrix		matWorld;
	matWorld = pCubeTransCom->Get_WorldMatrix_Inverse();
	XMVector3TransformCoord(XMLoadFloat4(&vRayPos), matWorld);
	XMVector3TransformNormal(XMLoadFloat4(&vRayDir), matWorld);

	_ulong	dwVtxIdx[3]{};
	_float fDist;

	for (_ulong i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_ulong j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_ulong dwIndex = i * (m_iNumVerticesX) + j;

			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + (m_iNumVerticesX) + 1;
			dwVtxIdx[2] = dwIndex + 1;

			_float4 Normal;
			XMStoreFloat4( &Normal, XMLoadFloat4(&m_pVtx[dwIndex + (m_iNumVerticesX) + 1]) + (XMLoadFloat4(&m_pVtx[dwIndex]) / 2));


			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]) ,
				fDist))
			{
			
				XMVector3TransformCoord(XMLoadFloat4(&Normal), matWorld);
				return  Normal;
			}


			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;


			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]),
				fDist))
			{

				XMVector3TransformCoord(XMLoadFloat4(&Normal), matWorld);
				return  Normal;
			}
		}
	}


	return _float4(0.f,0.f,0.f,0.f);
}

_bool CVI_Buffer_Terrain::PickingBuffer(HWND hWnd, CTransform * pCubeTransCom, _float4 *vPosition)
{
	CGameInstance* pGameIntance = GET_INSTANCE(CGameInstance);

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		vPoint;

	D3D11_VIEWPORT		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
	ViewPort.Width = 1280;
	ViewPort.Height = 720;

	vPoint.x = ptMouse.x / (1280 * 0.5f) - 1.f;
	vPoint.y = ptMouse.y / -(720 * 0.5f) + 1.f;
	vPoint.z = 1.f;
	vPoint.w = 1.f;

	_matrix		matProj;
	matProj = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matProj));

	_matrix		matView;
	matView = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matView));

	_float4		vRayPos;
	memcpy(&vRayPos, &matView.r[3], sizeof(_float4));
	_float4		vRayDir;
	XMStoreFloat4(&vRayDir, (XMLoadFloat4(&vPoint) - XMLoadFloat4(&vRayPos)));


	_matrix		matWorld;
	matWorld = pCubeTransCom->Get_WorldMatrix_Inverse();
	XMVector3TransformCoord(XMLoadFloat4(&vRayPos), matWorld);
	XMVector3TransformNormal(XMLoadFloat4(&vRayDir), matWorld);

	RELEASE_INSTANCE(CGameInstance);
	_ulong	dwVtxIdx[3]{};
	_float fDist;

	for (_ulong i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_ulong j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_ulong dwIndex = i * (m_iNumVerticesX) + j;

			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + (m_iNumVerticesX) + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]) ,
				fDist))
			{
			
				vPosition->x = (m_pVtx[dwVtxIdx[0]].x + m_pVtx[dwVtxIdx[1]].x + m_pVtx[dwVtxIdx[2]].x)/3.f;
				vPosition->y = 0.f;
				vPosition->z = (m_pVtx[dwVtxIdx[0]].z + m_pVtx[dwVtxIdx[1]].z + m_pVtx[dwVtxIdx[2]].z) / 3.f;
				vPosition->w = 1.f;
				return  true;
			}


			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;


			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]),
				fDist))
			{
				vPosition->x = (m_pVtx[dwVtxIdx[0]].x + m_pVtx[dwVtxIdx[1]].x + m_pVtx[dwVtxIdx[2]].x) / 3.f;
				vPosition->y = 0.f;
				vPosition->z = (m_pVtx[dwVtxIdx[0]].z + m_pVtx[dwVtxIdx[1]].z + m_pVtx[dwVtxIdx[2]].z) / 3.f;
				vPosition->w = 1.f;
				return  true;
			}
		}
	}



	return false;
}

_bool CVI_Buffer_Terrain::PickingRetrunIndex(HWND hWnd, CTransform * pCubeTransCom, _float4& fIndexs)
{

	CGameInstance* pGameIntance = GET_INSTANCE(CGameInstance);

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		vPoint;

	D3D11_VIEWPORT		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
	ViewPort.Width = 1280;
	ViewPort.Height = 720;


	vPoint.x = ptMouse.x / (1280 * 0.5f) - 1.f;
	vPoint.y = ptMouse.y / -(720 * 0.5f) + 1.f;
	vPoint.z = 1.f;
	vPoint.w = 1.f;

	_matrix		matProj;
	matProj = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matProj));

	_matrix		matView;
	matView = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matView));

	_float4		vRayPos;
	memcpy(&vRayPos, &matView.r[3], sizeof(_float4));
	_float4		vRayDir;
	XMStoreFloat4(&vRayDir, (XMLoadFloat4(&vPoint) - XMLoadFloat4(&vRayPos)));


	_matrix		matWorld;
	matWorld = pCubeTransCom->Get_WorldMatrix_Inverse();
	XMVector3TransformCoord(XMLoadFloat4(&vRayPos), matWorld);
	XMVector3TransformNormal(XMLoadFloat4(&vRayDir), matWorld);

	RELEASE_INSTANCE(CGameInstance);
	_ulong	dwVtxIdx[3]{};
	_float fDist;

	for (_ulong i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_ulong j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_ulong dwIndex = i * (m_iNumVerticesX)+j;

			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + (m_iNumVerticesX)+1;
			dwVtxIdx[2] = dwIndex + 1;

			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]),
				fDist))
			{
				// 여기서 바꿔야함
				dwIndex  = i * (m_iNumVerticesX-1)+j;

				fIndexs.x = _float(dwIndex + (m_iNumVerticesX-1));
				fIndexs.y = _float(dwIndex + (m_iNumVerticesX-1)+1);
				fIndexs.z = _float(dwIndex + 1);
				fIndexs.w = _float(dwIndex);
				return  true;
			}


			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;


			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]),
				fDist))
			{
				dwIndex = i * (m_iNumVerticesX - 1) + j;
				fIndexs.x = _float(dwIndex + (m_iNumVerticesX - 1));
				fIndexs.y = _float(dwIndex + (m_iNumVerticesX - 1) + 1);
				fIndexs.z = _float(dwIndex + 1);
				fIndexs.w = _float(dwIndex);
				return  true;
			}
		}
	}


	return false;

}

_bool CVI_Buffer_Terrain::PickingNavi(HWND hWnd, CTransform * pCubeTransCom, _float4& vPosition)
{
	CGameInstance* pGameIntance = GET_INSTANCE(CGameInstance);

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		vPoint;

	D3D11_VIEWPORT		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
	ViewPort.Width = 1280;
	ViewPort.Height = 720;

	vPoint.x = ptMouse.x / (1280 * 0.5f) - 1.f;
	vPoint.y = ptMouse.y / -(720 * 0.5f) + 1.f;
	vPoint.z = 1.f;
	vPoint.w = 1.f;

	_matrix		matProj;
	matProj = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matProj));

	_matrix		matView;
	matView = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matView));

	_float4		vRayPos;
	memcpy(&vRayPos, &matView.r[3], sizeof(_float4));
	_float4		vRayDir;
	XMStoreFloat4(&vRayDir, (XMLoadFloat4(&vPoint) - XMLoadFloat4(&vRayPos)));


	_matrix		matWorld;
	matWorld = pCubeTransCom->Get_WorldMatrix_Inverse();
	XMVector3TransformCoord(XMLoadFloat4(&vRayPos), matWorld);
	XMVector3TransformNormal(XMLoadFloat4(&vRayDir), matWorld);

	RELEASE_INSTANCE(CGameInstance);
	_ulong	dwVtxIdx[3]{};
	_float fDist;

	for (_ulong i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_ulong j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_ulong dwIndex = i * (m_iNumVerticesX)+j;

			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + (m_iNumVerticesX)+1;
			dwVtxIdx[2] = dwIndex + 1;

			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]),
				fDist))
			{

				_vector vPos = XMLoadFloat4(&vRayPos) + XMVector3Normalize(XMLoadFloat4(&vRayDir))* fDist;

				XMStoreFloat4(&vPosition, vPos);

				return  true;
			}


			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;


			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]),
				fDist))
			{
				_vector vPos = XMLoadFloat4(&vRayPos) + XMVector3Normalize(XMLoadFloat4(&vRayDir))* fDist;

				XMStoreFloat4(&vPosition, vPos);
				return true;
			}
		}
	}


	return false;
}

_bool CVI_Buffer_Terrain::Picking_Index(HWND hWnd, class CTransform * pCubeTransCom, _float4* fIndexs)
{
	CGameInstance* pGameIntance = GET_INSTANCE(CGameInstance);

	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	_float4		vPoint;

	D3D11_VIEWPORT		ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3D11_VIEWPORT));
	ViewPort.Width = 1280;
	ViewPort.Height = 720;


	vPoint.x = ptMouse.x / (1280 * 0.5f) - 1.f;
	vPoint.y = ptMouse.y / -(720 * 0.5f) + 1.f;
	vPoint.z = 1.f;
	vPoint.w = 1.f;

	_matrix		matProj;
	matProj = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matProj));

	_matrix		matView;
	matView = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matView));

	_float4		vRayPos;
	memcpy(&vRayPos, &matView.r[3], sizeof(_float4));
	_float4		vRayDir;
	XMStoreFloat4(&vRayDir, (XMLoadFloat4(&vPoint) - XMLoadFloat4(&vRayPos)));


	_matrix		matWorld;
	matWorld = pCubeTransCom->Get_WorldMatrix_Inverse();
	XMVector3TransformCoord(XMLoadFloat4(&vRayPos), matWorld);
	XMVector3TransformNormal(XMLoadFloat4(&vRayDir), matWorld);

	RELEASE_INSTANCE(CGameInstance);
	_ulong	dwVtxIdx[3]{};
	_float fDist;

	for (_ulong i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_ulong j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_ulong dwIndex = i * (m_iNumVerticesX)+j;

			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + (m_iNumVerticesX)+1;
			dwVtxIdx[2] = dwIndex + 1;

			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]),
				fDist))
			{
				// 여기서 바꿔야함
				fIndexs[0] = m_pVtx[dwVtxIdx[0]];
				fIndexs[1] = m_pVtx[dwVtxIdx[1]];
				fIndexs[2] = m_pVtx[dwVtxIdx[2]];

				return  true;
			}


			dwVtxIdx[0] = dwIndex + (m_iNumVerticesX);
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;


			if (TriangleTests::Intersects(XMLoadFloat4(&vRayPos),
				XMVector3Normalize(XMLoadFloat4(&vRayDir)),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[1]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[0]]),
				XMLoadFloat4(&m_pVtx[dwVtxIdx[2]]),
				fDist))
			{
				fIndexs[0] = m_pVtx[dwVtxIdx[0]];
				fIndexs[1] = m_pVtx[dwVtxIdx[1]];
				fIndexs[2] = m_pVtx[dwVtxIdx[2]];
				return  true;
			}
		}
	}


	return false;
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

	if (!m_bClone)
		Safe_Delete_Array(m_pVtx);
}
