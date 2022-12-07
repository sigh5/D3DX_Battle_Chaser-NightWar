#include "..\public\Mesh.h"
#include "Model.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
	, m_pAIMesh(rhs.m_pAIMesh)
	, m_eType(rhs.m_eType)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_iNumBones(rhs.m_iNumBones)
	, m_pVertices(rhs.m_pVertices)
	, m_pAnimVertices(rhs.m_pAnimVertices)
{

}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, aiMesh * pAIMesh, CModel* pModel)
{
	m_pAIMesh = pAIMesh;

	m_eType = eType;
	// 여기 다시저장해야함
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumPrimitive = pAIMesh->mNumFaces;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	HRESULT			hr = 0;

	if (CModel::TYPE_NONANIM == m_eType)
	{
		hr = Ready_VertexBuffer_NonAnimModel(pAIMesh, pModel);
	}
	else
	{
		hr = Ready_VertexBuffer_AnimModel(pAIMesh, pModel);
	}

	if (FAILED(hr))
		return E_FAIL;


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
	/* 여기서 추가해야함 저장*/
	m_pIndices	   = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];

		m_pIndices[i]._0 = pIndices[i]._0;
		m_pIndices[i]._1 = pIndices[i]._1;
		m_pIndices[i]._2 = pIndices[i]._2;

	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void * pArg)
{
	return S_OK;
}

void CMesh::SetUp_MeshBones(CModel * pModel)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = m_pAIMesh->mBones[i];

		CBone*		pBone = pModel->Get_BonePtr(pAIBone->mName.data);
		if (nullptr == pBone)
			return;

		_float4x4		OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		pBone->Set_OffsetMatrix(OffsetMatrix);

		m_Bones.push_back(pBone);

		Safe_AddRef(pBone);
	}

	/* 애니메이션을 가지고 있는 모델을 로드하는 과정에서.
	뼈대가 없는 메시가 존재한다?!
	이 메시는 특정 위치에 고정되어서 그려지는 것이 맞ㄷ다.
	항등상태로 원점에 그려져야할 일은 사실 없다. 칼, 그림자, 모닝스타. */
	/* 이 메시들이 존재해야하는 위치는 같은 이름을가진 뼈대의 위치에 존재하면된다.
	그래서 그 뼈를 찾아야한다. */
	if (0 == m_iNumBones)
	{
		CBone*		pBone = pModel->Get_BonePtr(m_pAIMesh->mName.data);

		if (nullptr == pBone)
			return;

		m_Bones.push_back(pBone);

		m_iNumBones = 1;
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

void CMesh::Save_MeshData(HANDLE hFile)
{
	DWORD   dwByte = 0;

	_int	iType = _int(m_eType);
	WriteFile(hFile, &iType, sizeof(_int), &dwByte, nullptr);
	WriteFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);

	if (CModel::TYPE_NONANIM == m_eType)
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			WriteFile(hFile, &m_pVertices[i].vPosition, sizeof(XMFLOAT3), &dwByte, nullptr);
			WriteFile(hFile, &m_pVertices[i].vNormal, sizeof(XMFLOAT3), &dwByte, nullptr);
			WriteFile(hFile, &m_pVertices[i].vTexUV, sizeof(XMFLOAT2), &dwByte, nullptr);
			WriteFile(hFile, &m_pVertices[i].vTangent, sizeof(XMFLOAT3), &dwByte, nullptr);
		}
	}
	else
	{
		for (_uint i = 0; i < m_iNumVertices; ++i)
		{
			WriteFile(hFile, &m_pAnimVertices[i].vPosition, sizeof(XMFLOAT3), &dwByte, nullptr);
			WriteFile(hFile, &m_pAnimVertices[i].vNormal, sizeof(XMFLOAT3), &dwByte, nullptr);
			WriteFile(hFile, &m_pAnimVertices[i].vTexUV, sizeof(XMFLOAT2), &dwByte, nullptr);
			WriteFile(hFile, &m_pAnimVertices[i].vTangent, sizeof(XMFLOAT3), &dwByte, nullptr);
			WriteFile(hFile, &m_pAnimVertices[i].vBlendIndex, sizeof(XMUINT4), &dwByte, nullptr);
			WriteFile(hFile, &m_pAnimVertices[i].vBlendWeight, sizeof(XMFLOAT4), &dwByte, nullptr);
		}
	}
	
	WriteFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		WriteFile(hFile, &m_pIndices[i]._0, sizeof(unsigned long), &dwByte, nullptr);
		WriteFile(hFile, &m_pIndices[i]._1, sizeof(unsigned long), &dwByte, nullptr);
		WriteFile(hFile, &m_pIndices[i]._2, sizeof(unsigned long), &dwByte, nullptr);
	}
	
	
	for (auto &pBone : m_Bones)
	{
		char szName[MAX_PATH] = "";
		strcpy_s(szName, MAX_PATH,pBone->Get_Name());
		WriteFile(hFile, szName, MAX_PATH, &dwByte, nullptr);
	}
}

HRESULT CMesh::Ready_VertexBuffer_NonAnimModel(aiMesh * pAIMesh, CModel* pModel)
{
	m_iStride = sizeof(VTXMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXMODEL*			pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	m_pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXMODEL) * m_iNumVertices); //저장을 위해서

	_matrix			PivotMatrix = pModel->Get_PivotMatrix();

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		memcpy(&m_pVertices[i].vPosition, &pVertices[i].vPosition, sizeof(_float3)); // for_save

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));
		memcpy(&m_pVertices[i].vNormal, &pVertices[i].vNormal, sizeof(_float3)); // for_save
		
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&m_pVertices[i].vTexUV, &pVertices[i].vTexUV, sizeof(_float2)); // for_save
		
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&m_pVertices[i].vTangent, &pVertices[i].vTangent, sizeof(_float3)); // for_save

	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_AnimModel(aiMesh * pAIMesh, CModel* pModel)
{
	m_iStride = sizeof(VTXANIMMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXANIMMODEL*			pVertices = new VTXANIMMODEL[m_iNumVertices];
	m_pAnimVertices = new VTXANIMMODEL[m_iNumVertices];	// for_Save
	
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);
	ZeroMemory(m_pAnimVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);	// for_Save

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		memcpy(&m_pAnimVertices[i].vPosition, &pVertices[i].vPosition, sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vNormal, &pVertices[i].vNormal, sizeof(_float3));
		memcpy(&m_pAnimVertices[i].vTexUV, &pVertices[i].vTexUV, sizeof(_float2));
		memcpy(&m_pAnimVertices[i].vTangent, &pVertices[i].vTangent, sizeof(_float3));

	}

	/* 메시에 영향ㅇ르 준다.ㅏ */
	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];

		/* 이 뼈는 몇개의 정점에 영향을 주는가?! */
		_uint iNumWeights = pAIBone->mNumWeights;

		for (_uint j = 0; j < iNumWeights; ++j)
		{
			_uint iVertexIndex = pAIBone->mWeights[j].mVertexId;

			if (0.0f == pVertices[iVertexIndex].vBlendWeight.x)
			{
				pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;

				m_pAnimVertices[iVertexIndex].vBlendIndex.x = i;
				m_pAnimVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;

				m_pAnimVertices[iVertexIndex].vBlendIndex.y = i;
				m_pAnimVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;

				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.w)
			{
				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;

				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);



	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eType, aiMesh * pAIMesh, CModel* pModel)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, pModel)))
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

	if (!m_bClone)
	{
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pAnimVertices);
	}


	m_Bones.clear();
}
