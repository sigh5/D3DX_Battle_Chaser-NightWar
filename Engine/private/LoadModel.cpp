#include "..\public\LoadModel.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

#include "LoadBone.h"
#include "LoadMesh.h"
#include "LoadAnimation.h"
CLoadModel::CLoadModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CLoadModel::CLoadModel(const CLoadModel & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_Bones(rhs.m_Bones)
	, m_Animations(rhs.m_Animations)
{
	strcpy_s(m_szModelPath, MAX_PATH, rhs.m_szModelPath);
	
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}
	for (auto& pMesh : rhs.m_Meshes)
		m_Meshes.push_back((CLoadMesh*)pMesh->Clone());


	/*for (auto &pOldBone : rhs.m_Bones)
	{
		CLoadBone* pNewBone = CLoadBone::CreateCopy(this,pOldBone);
		if (nullptr == pNewBone)
			assert("CLoadModel & rhs Issue");
		m_Bones.push_back(pNewBone);
	}*/

	//for (auto &pOldAnim : rhs.m_Animations)
	//{
	//	CLoadAnimation* pNewAnim = CLoadAnimation::CreateCopy(this, pOldAnim);
	//	if (nullptr == pNewAnim)
	//		assert("CLoadModel & rhs Issue");

	//	m_Animations.push_back(pNewAnim);
	//}
}

CLoadBone * CLoadModel::Get_BonePtr(const char * pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CLoadBone* pBone)->_bool
	{
		return !strcmp(pBoneName, pBone->Get_Name());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;

}

HRESULT CLoadModel::Initialize_Prototype(LOAD_TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile)
{
	_uint			iFlag = 0;
	strcpy_s(m_szModelPath, MAX_PATH, pModelFilePath);
	m_eType = eType;
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	if (FAILED(Ready_Bones(hFile,nullptr)))	// �������� �ʿ�
		return E_FAIL;

	if (FAILED(Ready_MeshContainers(hFile)))
		return E_FAIL;

	if (FAILED(Ready_Materials(hFile, pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animation(hFile)))		// ���� ���� �ʿ�
		return E_FAIL;




	return S_OK;
}

HRESULT CLoadModel::Initialize(void * pArg)
{
	ZeroMemory(&m_ModelDesc, sizeof(m_ModelDesc));

	if (nullptr != pArg)
		memcpy(&m_ModelDesc, pArg, sizeof(m_ModelDesc));



	return S_OK;
}

void CLoadModel::Play_Animation(_double TimeDelta)
{
	if (TYPE_NONANIM == m_eType)
		return;

	/* ���� �ִϸ��̼ǿ� �´� ������ TranformMAtrix�� �����ϳ�. */
	m_Animations[m_iCurrentAnimIndex]->Update_Bones(TimeDelta);

	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}
}

HRESULT CLoadModel::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshes[iMeshIndex]->Get_MaterialIndex();

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (nullptr != m_Materials[iMaterialIndex].pTexture[eType])
	{
		m_Materials[iMaterialIndex].pTexture[eType]->Bind_ShaderResource(pShader, pConstantName);
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadModel::Render(CShader * pShader, _uint iMeshIndex, _uint iShaderIndex, const char * pBoneConstantName)
{
	if (!strcmp(m_Meshes[iMeshIndex]->Get_MeshName(), "FishingRod"))
		return S_OK;

	if (nullptr != m_Meshes[iMeshIndex])
	{
		if (nullptr != pBoneConstantName)
		{
			_float4x4		BoneMatrices[128];

			m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 128);
		}

		pShader->Begin(iShaderIndex);

		m_Meshes[iMeshIndex]->Render();
	}


	return S_OK;
}

HRESULT CLoadModel::Ready_Bones(HANDLE hFile,CBone * pParent)
{
	DWORD   dwByte = 0;
	_uint	iBoneNum = 0;
	ReadFile(hFile, &iBoneNum, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iBoneNum; ++i)
	{
		CLoadBone* pBone = CLoadBone::Create(this,hFile);
		
		if (pBone == nullptr)
			assert("CLoadModel_Ready_Bones");

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CLoadModel::Ready_MeshContainers(HANDLE hFile)
{
	DWORD   dwByte = 0;
	ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CLoadMesh *pMesh = CLoadMesh::Create(m_pDevice, m_pContext, this, hFile);

		if (nullptr == pMesh)
			assert("CLoadModel::Ready_MeshContainers");

		m_Meshes.push_back(pMesh);
	}


	return S_OK;
}

HRESULT CLoadModel::Ready_Materials(HANDLE hFile,const char * pModelFilePath)
{
	DWORD   dwByte = 0;
	ReadFile(hFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MODELMATERIAL ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(ModelMaterial));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			_uint iTextureIndex = 0;
			ReadFile(hFile, &iTextureIndex, sizeof(_uint), &dwByte, nullptr);
			if (AI_TEXTURE_TYPE_MAX == iTextureIndex)
			{
				continue;
			}

			_tchar szFilePath[MAX_PATH] = TEXT("");
			ReadFile(hFile, szFilePath, sizeof(_tchar[MAX_PATH]), &dwByte, nullptr);

			if (!lstrcmp(szFilePath, TEXT("")))
				assert("CLoadModel: Ready_Materials issue");

			ModelMaterial.pTexture[iTextureIndex] = CTexture::Create(m_pDevice, m_pContext, szFilePath);
			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CLoadModel::Ready_Animation(HANDLE hFile)
{
	DWORD   dwByte = 0;
	ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CLoadAnimation* pAnimation = CLoadAnimation::Create(hFile, this);

		if (nullptr == pAnimation)
			assert("CLoadModel::Ready_Animation");

		m_Animations.push_back(pAnimation);
	}



	return S_OK;
}

CLoadModel * CLoadModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LOAD_TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile)
{
	CLoadModel*		pInstance = new CLoadModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix, hFile)))
	{
		MSG_BOX("Failed to Created : CLoadModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CLoadModel::Clone(void * pArg)
{
	CLoadModel*		pInstance = new CLoadModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();



}