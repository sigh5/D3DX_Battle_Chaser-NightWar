#include "..\public\Model.h"
#include "Mesh.h"

#include "Texture.h"
#include "Shader.h"
CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_eType(rhs.m_eType)
	, m_Meshs(rhs.m_Meshs)
	, m_iMeshNum(rhs.m_iMeshNum)
	,m_Materials(rhs.m_Materials)
{
	for (auto &pMesh : m_Meshs)
		Safe_AddRef(pMesh);
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

	if (FAILED(Ready_MeshContainers()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;


	

	//m_pAIScene->HasAnimations(); // 애니메이션 있는지 확인하는 함수


	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	//for (_uint i = 0; i < m_iMeshNum; ++i)
	//{
	//	/* 셰이더에 맵핑해야할 재질(텍스쳐)을 던진다. */

	//	if (nullptr != m_Meshs[i])
	//		m_Meshs[i]->Render();
	//}

	pShader->Begin(iPassIndex);

	m_Meshs[iMeshIndex]->Render();

	return S_OK;		// 나중에 클라로 뺄거임
}

HRESULT CModel::Ready_MeshContainers()
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iMeshNum = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iMeshNum; ++i)
	{
		aiMesh*		pAiMesh = m_pAIScene->mMeshes[i];
		
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, pAiMesh);
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

HRESULT CModel::SetUp_Material(CShader * pShader, const char * pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iNumMaterials)
		return E_FAIL;
	
	ID3D11ShaderResourceView* pSRV = nullptr;
	MODELMATERIAL modleMateral = m_Materials[m_Meshs[iMeshIndex]->Get_MaterialIndex()];
	if (modleMateral.pTexture[eType])
	{
		pSRV = modleMateral.pTexture[eType]->Get_TextureResouceView();
		return pShader->Set_ShaderResourceView(pConstantName, pSRV);
	}
	else
	{
		return E_FAIL;
	}
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

	for (auto &pMesh : m_Meshs)
		Safe_Release(pMesh);
	m_Meshs.clear();



	for (auto &pMeterial : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(pMeterial.pTexture[i]);
	}


	m_Importer.FreeScene(); // 해제 코드
}
