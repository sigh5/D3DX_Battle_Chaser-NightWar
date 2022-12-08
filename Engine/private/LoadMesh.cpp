#include "..\public\LoadMesh.h"

#include "LoadBone.h"


CLoadMesh::CLoadMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CLoadMesh::CLoadMesh(const CLoadMesh & rhs)
	: CVIBuffer(rhs)
	, m_eType(rhs.m_eType)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_Bones(rhs.m_Bones)
{
	for (auto& pBone : m_Bones)
		Safe_AddRef(pBone);
}

HRESULT CLoadMesh::Initialize_Prototype(CLoadModel * pModel, HANDLE hFile)
{
	DWORD   dwByte = 0;
	_uint	iType = 0;
	ReadFile(hFile, &iType, sizeof(_uint), &dwByte, nullptr);
	m_eType = (CLoadModel::LOAD_TYPE)iType;
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);
	
	m_iNumVertexBuffers = 1;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

	if (CLoadModel::TYPE_NONANIM == m_eType)
	{
		Ready_VertexBuffer_NonAnimModel(hFile,pModel);
	}
	else
	{
		Ready_VertexBuffer_AnimModel(hFile,pModel);
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
		ReadFile(hFile, &pIndices[i]._0, sizeof(unsigned long), &dwByte, nullptr);
		ReadFile(hFile, &pIndices[i]._1, sizeof(unsigned long), &dwByte, nullptr);
		ReadFile(hFile, &pIndices[i]._2, sizeof(unsigned long), &dwByte, nullptr);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		char szName[MAX_PATH] = "";
		ReadFile(hFile, szName, MAX_PATH, &dwByte, nullptr);

		CLoadBone* pBone = pModel->Get_BonePtr(szName);
		if (nullptr == pBone)
			assert("CLoadMesh::Initialize_Prototype");
		
		m_Bones.push_back(pBone);
		Safe_AddRef(pBone);
	}

	return S_OK;
}

HRESULT CLoadMesh::Initialize(void * pArg)
{

	//vector<CLoadBone*>	m_NewBones;

	//for (auto &pBone : m_Bones)
	//{
	//	char szBoneName[MAX_PATH] = "";
	//	strcpy_s(szBoneName, MAX_PATH, pBone->Get_Name());
	//	CLoadBone* pCloneBone =	pModel->Get_BonePtr(szBoneName);
	//	if (nullptr == pCloneBone)
	//		assert("CLoadMesh::Initialize");

	//	m_NewBones.push_back(pCloneBone);
	//	//Safe_AddRef(m_NewBones);
	//}
	//
	//for (auto &pBone : m_Bones)
	//	Safe_Release(pBone);

	//for (auto& pNewBone : m_NewBones)
	//{
	//	m_Bones.push_back(pNewBone);
	//	Safe_AddRef(pNewBone);
	//}


	return S_OK;
}


void CLoadMesh::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
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

HRESULT CLoadMesh::Ready_VertexBuffer_NonAnimModel(HANDLE hFile,  CLoadModel * pModel)
{		
	DWORD   dwByte = 0;

	VTXMODEL*	 	pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ReadFile(hFile, &pVertices[i].vPosition, sizeof(XMFLOAT3), &dwByte, nullptr);
		ReadFile(hFile, &pVertices[i].vNormal, sizeof(XMFLOAT3), &dwByte, nullptr);
		ReadFile(hFile, &pVertices[i].vTexUV, sizeof(XMFLOAT2), &dwByte, nullptr);
		ReadFile(hFile, &pVertices[i].vTangent, sizeof(XMFLOAT3), &dwByte, nullptr);
	}


	m_iStride = sizeof(VTXMODEL);
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

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CLoadMesh::Ready_VertexBuffer_AnimModel(HANDLE hFile,CLoadModel * pModel)
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

	// 여기가 문제
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		ReadFile(hFile, &pAnimVertices[i].vPosition, sizeof(XMFLOAT3), &dwByte, nullptr);
		ReadFile(hFile, &pAnimVertices[i].vNormal, sizeof(XMFLOAT3), &dwByte, nullptr);
		ReadFile(hFile, &pAnimVertices[i].vTexUV, sizeof(XMFLOAT2), &dwByte, nullptr);
		ReadFile(hFile, &pAnimVertices[i].vTangent, sizeof(XMFLOAT3), &dwByte, nullptr);
		ReadFile(hFile, &pAnimVertices[i].vBlendIndex, sizeof(XMUINT4), &dwByte, nullptr);
		ReadFile(hFile, &pAnimVertices[i].vBlendWeight, sizeof(XMFLOAT4), &dwByte, nullptr);
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pAnimVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pAnimVertices);

	return S_OK;
}

CLoadMesh * CLoadMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CLoadModel * pModel, HANDLE hFile)
{
	CLoadMesh*		pInstance = new CLoadMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModel,hFile)))
	{
		MSG_BOX("Failed to Created : CLoadMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CLoadMesh::Clone(void * pArg)
{
	CLoadMesh*		pInstance = new CLoadMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoadMesh::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();
}
