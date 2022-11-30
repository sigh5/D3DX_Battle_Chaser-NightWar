#include "..\public\VIBuffer_Cube.h"
#include "Transform.h"
#include "GameInstance.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
	
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube & rhs)
	: CVIBuffer(rhs)
	,m_vPos(rhs.m_vPos)
{

}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXCUBETEX);
	m_iNumVertices = 8;
	m_iNumPrimitive = 12;	
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumPrimitive*m_iNumIndicesPerPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXCUBETEX*			pVertices = new VTXCUBETEX[m_iNumVertices];
	m_vPos = new _float4[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXCUBETEX));

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexUV = pVertices[0].vPosition;
	m_vPos[0] = _float4( pVertices[0].vPosition.x, pVertices[0].vPosition.y, pVertices[0].vPosition.z,1.f );

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexUV = pVertices[1].vPosition;
	m_vPos[1] = _float4(pVertices[1].vPosition.x, pVertices[1].vPosition.y, pVertices[1].vPosition.z, 1.f);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexUV = pVertices[2].vPosition;
	m_vPos[2] = _float4(pVertices[2].vPosition.x, pVertices[2].vPosition.y, pVertices[2].vPosition.z, 1.f);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexUV = pVertices[3].vPosition;
	m_vPos[3] = _float4(pVertices[3].vPosition.x, pVertices[3].vPosition.y, pVertices[3].vPosition.z, 1.f);

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexUV = pVertices[4].vPosition;
	m_vPos[4] = _float4(pVertices[4].vPosition.x, pVertices[4].vPosition.y, pVertices[4].vPosition.z, 1.f);

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexUV = pVertices[5].vPosition;
	m_vPos[5] = _float4(pVertices[5].vPosition.x, pVertices[5].vPosition.y, pVertices[5].vPosition.z, 1.f);

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexUV = pVertices[6].vPosition;
	m_vPos[6] = _float4(pVertices[6].vPosition.x, pVertices[6].vPosition.y, pVertices[6].vPosition.z, 1.f);

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexUV = pVertices[7].vPosition;
	m_vPos[7] = _float4(pVertices[7].vPosition.x, pVertices[7].vPosition.y, pVertices[7].vPosition.z, 1.f);

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

	pIndices[0]._0 = 1;
	pIndices[0]._1 = 5;
	pIndices[0]._2 = 6;

	pIndices[1]._0 = 1;
	pIndices[1]._1 = 6;
	pIndices[1]._2 = 2;

	// X-
	pIndices[2]._0 = 4;
	pIndices[2]._1 = 0;
	pIndices[2]._2 = 3;

	pIndices[3]._0 = 4;
	pIndices[3]._1 = 3;
	pIndices[3]._2 = 7;

	// Y+
	pIndices[4]._0 = 4;
	pIndices[4]._1 = 5;
	pIndices[4]._2 = 1;

	pIndices[5]._0 = 4;
	pIndices[5]._1 = 1;
	pIndices[5]._2 = 0;

	// Y-
	pIndices[6]._0 = 3;
	pIndices[6]._1 = 2;
	pIndices[6]._2 = 6;

	pIndices[7]._0 = 3;
	pIndices[7]._1 = 6;
	pIndices[7]._2 = 7;

	// Z+
	pIndices[8]._0 = 7;
	pIndices[8]._1 = 6;
	pIndices[8]._2 = 5;

	pIndices[9]._0 = 7;
	pIndices[9]._1 = 5;
	pIndices[9]._2 = 4;

	// Z-
	pIndices[10]._0 = 0;
	pIndices[10]._1 = 1;
	pIndices[10]._2 = 2;

	pIndices[11]._0 = 0;
	pIndices[11]._1 = 2;
	pIndices[11]._2 = 3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

_bool CVIBuffer_Cube::PickingBuffer(HWND hWnd, class CTransform * pCubeTransCom, _float4 &vPosition)
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
	vPoint.z = 0.f;
	vPoint.w = 1.f;

	_matrix		matProj;
	matProj = pGameIntance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	XMStoreFloat4(&vPoint, XMVector3TransformCoord(XMLoadFloat4(&vPoint), matProj));


	_float4 vecRayPos = _float4(0.0f, 0.f, 0.f, 1.f);
	_float4 vecRayDir;
	XMStoreFloat4(&vecRayDir, XMLoadFloat4(&vPoint) - XMLoadFloat4(&vecRayPos));

	_matrix      ViewMatrixInv;
	ViewMatrixInv = pGameIntance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	ViewMatrixInv = XMMatrixInverse(nullptr, ViewMatrixInv);
	XMStoreFloat4(&vecRayPos, XMVector3TransformCoord(XMLoadFloat4(&vecRayPos), ViewMatrixInv));
	XMStoreFloat4(&vecRayDir, XMVector3TransformNormal(XMLoadFloat4(&vecRayDir), ViewMatrixInv));
	XMStoreFloat4(&vecRayDir, XMVector3Normalize(XMLoadFloat4(&vecRayDir)));
	
	_matrix		matWorld;
	matWorld = pCubeTransCom->Get_WorldMatrix_Inverse();
	XMStoreFloat4(&vecRayPos, XMVector3TransformCoord(XMLoadFloat4(&vecRayPos), matWorld));
	XMStoreFloat4(&vecRayDir, XMVector3TransformNormal(XMLoadFloat4(&vecRayDir), matWorld));
	XMStoreFloat4(&vecRayDir, XMVector3Normalize(XMLoadFloat4(&vecRayDir)));

	RELEASE_INSTANCE(CGameInstance);
	_ulong	dwVtxIdx[3]{};
	_float	fDist = 0;

	// +X
	dwVtxIdx[0] = 1;
	dwVtxIdx[1] = 5;
	dwVtxIdx[2] = 6;

	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}


	dwVtxIdx[0] = 1;
	dwVtxIdx[1] = 6;
	dwVtxIdx[2] = 2;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}


	// -X
	dwVtxIdx[0] = 4;
	dwVtxIdx[1] = 0;
	dwVtxIdx[2] = 3;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}


	dwVtxIdx[0] = 4;
	dwVtxIdx[1] = 3;
	dwVtxIdx[2] = 7;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}

	//// Y
	dwVtxIdx[0] = 4;
	dwVtxIdx[1] = 5;
	dwVtxIdx[2] = 1;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}

	dwVtxIdx[0] = 4;
	dwVtxIdx[1] = 1;
	dwVtxIdx[2] = 0;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}

	//// ~ -Y
	dwVtxIdx[0] = 3;
	dwVtxIdx[1] = 2;
	dwVtxIdx[2] = 6;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}

	dwVtxIdx[0] = 3;
	dwVtxIdx[1] = 6;
	dwVtxIdx[2] = 7;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}


	// Z+
	dwVtxIdx[0] = 7;
	dwVtxIdx[1] = 6;
	dwVtxIdx[2] = 5;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}



	dwVtxIdx[0] = 7;
	dwVtxIdx[1] = 5;
	dwVtxIdx[2] = 4;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}
	// Z-
	dwVtxIdx[0] = 0;
	dwVtxIdx[1] = 1;
	dwVtxIdx[2] = 2;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}


	dwVtxIdx[0] = 0;
	dwVtxIdx[1] = 2;
	dwVtxIdx[2] = 3;
	if (TriangleTests::Intersects(XMLoadFloat4(&vecRayPos),
		XMLoadFloat4(&vecRayDir),
		XMLoadFloat4(&m_vPos[dwVtxIdx[1]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[0]]),
		XMLoadFloat4(&m_vPos[dwVtxIdx[2]]), fDist))
	{
		return true;
	}


	


	return false;
}



CVIBuffer_Cube * CVIBuffer_Cube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Cube::Clone(void * pArg)
{
	CVIBuffer_Cube*		pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CVIBuffer_Cube::Free()
{
	__super::Free();

	if(!m_bClone)
		Safe_Delete_Array(m_vPos);

}
