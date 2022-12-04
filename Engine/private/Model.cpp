#include "..\public\Model.h"
#include "Mesh.h"

#include "Texture.h"
#include "Shader.h"
#include "Transform.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_eType(rhs.m_eType)
	, m_iMeshNum(rhs.m_iMeshNum)
	, m_Meshs(rhs.m_Meshs)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Bones(rhs.m_Bones)
	, m_iNumBones(rhs.m_iNumBones)
	, m_Animations(rhs.m_Animations)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	
{
	for (auto & Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto &pMesh : m_Meshs)
		Safe_AddRef(pMesh);}

CBone * CModel::Get_BonePtr(char * pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		return !strcmp(pBoneName, pBone->Get_BoneName());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath)
{
	_uint			iFlag = 0;

	if (TYPE_NONANIM == eType)
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
		//		미리 떨어져있는 뼈들을 붙이는 플래그	// 왼손좌표계로변환				// 조금의 최적화
	else 
		iFlag  = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
		// aiProcess_PreTransformVertices 안쓰는 이유 == 내가 직접 조정할려고

	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* 뼈를 로드한다.*/
	// Anum Model 일때만 로드한다.

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode)))	// 최상위 노드부터 넣어야지 CBone에 있는 parent가 빵꾸가 안남
		return E_FAIL;

	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animation()))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

_bool CModel::PicikingModel(HWND hWnd, CTransform * pTransform)
{
	return false;
}

void CModel::Play_Animation(_double TimeDelta)
{
	/* 현재 애니메이션에 맞는 뼈들의 Transformatrix를 갱신한다. */
	m_Animations[m_iCurrentAnimIndex]->Update_Bones(TimeDelta);

	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}
}

HRESULT CModel::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
{	
	if (iMeshIndex >= m_iMeshNum)
		return E_FAIL;

	_uint iMaterialIndex = m_Meshs[iMeshIndex]->Get_MaterialIndex();

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

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex,const char* pBoneConstantName)
{
	if (nullptr != m_Meshs[iMeshIndex])
	{
		if (nullptr != pBoneConstantName)
		{
			_float4x4		BoneMatrices[256];

			m_Meshs[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices);

			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 256);
		}

		pShader->Begin(iPassIndex);

		m_Meshs[iMeshIndex]->Render();
	}


	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode * pNode) // 재귀함수 뼈의 부모부터 시작해서
{											// 뼈의 끝까지 간다.
	
	CBone*	pBone = CBone::Create(pNode); // 뼈를 로드해야되니까 뼈클래스가 필요하곘지?

	if (nullptr == pBone)
		return E_FAIL;
	
	m_Bones.push_back(pBone);

	/* 
		C_STRUCT aiNode** mChildren;
		pNode->mNumChildren 
	*/

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Ready_Bones(pNode->mChildren[i]);	//뼈의 끝까지간다.
				//이 구조는 지금 트리구조임 트리의 끝까지 찾아간다.
	}


	return S_OK;
}

HRESULT CModel::Ready_MeshContainers()
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iMeshNum = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iMeshNum; ++i)
	{
		aiMesh*		pAiMesh = m_pAIScene->mMeshes[i];
		
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType,pAiMesh,this);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshs.push_back(pMesh);

	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(const char * pModelFilePath)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMaterials = m_pAIScene->mNumMaterials;

	char		szDirectory[MAX_PATH] = "";
	_splitpath_s(pModelFilePath, nullptr, 0, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
	
	/* 텍스쳐에 기록되어있는 픽셀단위 재질 정보를 로드한다. */
	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		char		szTextureFileName[MAX_PATH] = "";
		char		szExt[MAX_PATH] = "";
		char		szTexturePath[MAX_PATH] = "";

		MODELMATERIAL	ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MODELMATERIAL));

		aiMaterial*	pAiMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			// j == 0		aiTextureType_NONE
			// j == 1		aiTextureType_Diffuse

			aiString		strTexturePath;

			/* 메쉬 1개당 머터리알 1개가 정답이 아니다. */

			if (FAILED(pAiMaterial->GetTexture(aiTextureType(j), 0, &strTexturePath)))	// 타입, 0 == 만약에 diffuse에 2개 있다 여기서 0번째 1번째 쓸거냐? 라는거임
				continue;

			_splitpath_s(strTexturePath.data, nullptr, 0, nullptr, 0, szTextureFileName, MAX_PATH, szExt, MAX_PATH);

			strcpy_s(szTexturePath, szDirectory);
			strcat_s(szTexturePath, szTextureFileName);
			strcat_s(szTexturePath, szExt);

			_tchar	szFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szTexturePath,  _uint(strlen(szTexturePath)), szFullPath, MAX_PATH);

			ModelMaterial.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szFullPath, CTexture::TYPE_END);

			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}


	return S_OK;
}

HRESULT CModel::Ready_Animation()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];

		CAnimation*			pAnim = CAnimation::Create(pAIAnimation, this);
		if (nullptr == pAnim)
			return E_FAIL;

		m_Animations.push_back(pAnim);
	}


	return S_OK;
}


CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	if (!m_bClone)
	{
		for (auto& pBone : m_Bones)
			Safe_Release(pBone);
	}

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshs)
		Safe_Release(pMesh);
	m_Meshs.clear();

	m_Importer.FreeScene();
}
