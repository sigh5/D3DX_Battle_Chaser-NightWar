#include "..\public\Mesh.h"

#include "Bone.h"


CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
	, m_eType(rhs.m_eType)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_Bones(rhs.m_Bones)
{
	strcpy_s(m_szName, MAX_PATH, rhs.m_szName);

	for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);
}

HRESULT CMesh::Initialize_Prototype(CModel * pModel, HANDLE hFile, CModel::LOAD_TYPE eType)
{
	DWORD   dwByte = 0;
	_uint	iType = 0;
	m_eType = eType;	//(CModel::LOAD_TYPE)iType;

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);


	m_iMaterialIndex = m_iMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = m_iNumVertices;
	m_iNumPrimitive = m_iNumPrimitive;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;
	
	
	if (CModel::TYPE_NONANIM == m_eType)
	{
		Ready_VertexBuffer_NonAnimModel(hFile, pModel);
	}
	else
	{
		Ready_VertexBuffer_AnimModel(hFile, pModel);
	}
	
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		ReadFile(hFile, &pIndices[i], sizeof(FACEINDICES32), &dwByte, nullptr);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{

	return S_OK;
}

void CMesh::LoadFile(HANDLE hFile,CModel* pModel)
{
	DWORD dwByte = 0;
	
	//ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

	for (size_t i = 0; i < m_iNumBones; ++i)
	{
		char pName[MAX_PATH] = "";

		ReadFile(hFile, pName, sizeof(char) * MAX_PATH, &dwByte, nullptr);
		CBone*	pBone =	pModel->Get_BonePtr(pName);
		if (nullptr == pBone)
			assert("CMesh::LoadFile");
		
		

		_float4x4 OffSetMatrix;
		ReadFile(hFile, &OffSetMatrix, sizeof(_float4x4), &dwByte, nullptr);
		pBone->Set_OffsetMatrix(OffSetMatrix);

		m_Bones.push_back(pBone);
		Safe_AddRef(pBone);

	}

}


void CMesh::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	_uint		iNumBones = 0;

	if (0 == m_iNumBones)
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());

	for (auto& pBone : m_Bones)
	{
		// BoneMatrix = 오프셋매트릭스 * 컴바인드매트릭스;
		XMStoreFloat4x4(&pBoneMatrices[iNumBones++],
			pBone->Get_OffsetMatrix()
			* pBone->Get_CombindMatrix()
			* PivotMatrix);
	}
}

HRESULT CMesh::Ready_VertexBuffer_NonAnimModel(HANDLE hFile, CModel * pModel)
{
	DWORD   dwByte = 0;

	m_iStride = sizeof(VTXMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;


	VTXMODEL*	 	pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL)*m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ReadFile(hFile, &pVertices[i], sizeof(VTXMODEL), &dwByte, nullptr);
	}


	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_AnimModel(HANDLE hFile, CModel * pModel)
{
	DWORD   dwByte = 0;

	m_iStride = sizeof(VTXANIMMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXANIMMODEL*	 	pAnimVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pAnimVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ReadFile(hFile, &pAnimVertices[i], sizeof(VTXANIMMODEL), &dwByte, nullptr);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pAnimVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pAnimVertices);

	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel * pModel, HANDLE hFile, CModel::LOAD_TYPE eType)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModel, hFile, eType)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMesh::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();
}
