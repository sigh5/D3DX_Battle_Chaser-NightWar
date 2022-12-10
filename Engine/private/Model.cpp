#include "..\public\Model.h"

#include "Texture.h"
#include "Shader.h"

#include "Bone.h"
#include "Mesh.h"
#include "Animation.h"
#include "GameUtils.h"
#include <string>
CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Bones(rhs.m_Bones)
	, m_iNumBones(rhs.m_iNumBones)
	, m_Animations(rhs.m_Animations)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_pCameraBone(rhs.m_pCameraBone)
{
	//Test 얕은복사
	for (auto& m_Bones : m_Bones)
	{
		Safe_AddRef(m_Bones);
	}

	for (auto& pAnimations : m_Animations)
	{
		Safe_AddRef(pAnimations);
	}

	strcpy_s(m_szModelPath, MAX_PATH, rhs.m_szModelPath);

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pMesh : rhs.m_Meshes)
		Safe_AddRef(pMesh);

}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
	{
		return !strcmp(pBoneName, pBone->Get_Name());
	});

	if (iter == m_Bones.end())
		return nullptr;

	return *iter;

}

HRESULT CModel::Initialize_Prototype(LOAD_TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile)
{
	_uint			iFlag = 0;
	strcpy_s(m_szModelPath, MAX_PATH, pModelFilePath);
	m_eType = eType;
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);


	if (FAILED(Ready_MeshContainers(hFile, eType)))
		return E_FAIL;

	if (FAILED(Ready_Materials(hFile, pModelFilePath, eType)))
		return E_FAIL;

	if (FAILED(Ready_Bones(hFile, nullptr)))	// 깊은복사 필요
		return E_FAIL;

	for (auto& pMesh : m_Meshes)
	{
		pMesh->LoadFile(hFile,this);
	}


	
	if (FAILED(Ready_Animation(hFile)))		// 깊은 복사 필요
		return E_FAIL;


	if (FAILED(Ready_CameraBone()))
		return E_FAIL;



	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	ZeroMemory(&m_ModelDesc, sizeof(m_ModelDesc));

	if (nullptr != pArg)
		memcpy(&m_ModelDesc, pArg, sizeof(m_ModelDesc));


	return S_OK;
}

void CModel::Play_Animation(_double TimeDelta, _bool m_IsSequnce )
{
	if (TYPE_NONANIM == m_eType)
		return;

	if (m_IsSequnce)
	{
		if (!m_AnimIndexQueue.empty()&&m_Animations[m_iNameArrayCurIndex]->Get_Finished())
		{
			m_Animations[m_iCurrentAnimIndex]->Set_Finished(false);
			m_iNameArrayCurIndex = m_AnimIndexQueue.front();
			m_AnimIndexQueue.pop();
			m_iCurrentAnimIndex = m_iNameArrayCurIndex;
			m_Animations[m_iCurrentAnimIndex]->Set_Looping(false);
		}
		else
		{
			m_Animations[m_iCurrentAnimIndex]->Set_Looping(true);
		}
		
	}
	else
	{
		m_Animations[m_iCurrentAnimIndex]->Set_Looping(true);
	}

	m_Animations[m_iCurrentAnimIndex]->Update_Bones(TimeDelta);

	for (auto& pBone : m_Bones)
	{
		if (nullptr != pBone)
			pBone->Compute_CombindTransformationMatrix();
	}
}



HRESULT CModel::Bind_Material(CShader * pShader, _uint iMeshIndex, aiTextureType eType, const char * pConstantName)
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

HRESULT CModel::Render(CShader * pShader, _uint iMeshIndex, _uint iShaderIndex, const char * pBoneConstantName,const char* pNoRenderName)
{
	if (pNoRenderName != nullptr && !strcmp(m_Meshes[iMeshIndex]->Get_MeshName(), pNoRenderName))
		return S_OK;

	if (!strcmp(m_szNoRenderMeshName, m_Meshes[iMeshIndex]->Get_MeshName()))
		return S_OK;
	
	if (m_iNoRenderIndex == iMeshIndex)
		return S_OK;

	if (m_iMeshIndex == iMeshIndex)
		return S_OK;

	if (nullptr != m_Meshes[iMeshIndex])
	{
		if (nullptr != pBoneConstantName)
		{
			_float4x4		BoneMatrices[256];

			m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			pShader->Set_MatrixArray(pBoneConstantName, BoneMatrices, 256);
		}

		pShader->Begin(iShaderIndex);

		m_Meshes[iMeshIndex]->Render();
	}


	return S_OK;
}

void CModel::Imgui_RenderProperty()
{
	ImGui::Begin("BoneName");

	if (ImGui::TreeNode("BoneNameViewer"))
	{
		char szBoneTag[MAX_PATH];
		
		if (ImGui::BeginListBox("##"))
		{
			for (auto& bone : m_Bones)
			{
				if (bone != nullptr)
				{

					strcpy_s(szBoneTag, MAX_PATH,bone->Get_Name());
					ImGui::Text(szBoneTag);
				}
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	_uint iIndex = 0;
	if (ImGui::TreeNode("Animation"))
	{
		char szAnimTag[MAX_PATH];

		if (ImGui::BeginListBox("##"))
		{
			for (auto& pAnim : m_Animations)
			{
				if (pAnim != nullptr)
				{
					strcpy_s(szAnimTag, MAX_PATH, pAnim->Get_Name());
					
					if (ImGui::Selectable(szAnimTag))
					{
						m_iCurrentAnimIndex = iIndex;
					}
					++iIndex;
				}
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	_uint iMeshNoRenderIndex = 0;
	if (ImGui::TreeNode("MeshName"))
	{
		char szMeshName[MAX_PATH];

		if (ImGui::BeginListBox("##"))
		{
			for (auto& pMesh : m_Meshes)
			{
				if (pMesh != nullptr)
				{
					strcpy_s(szMeshName, MAX_PATH, pMesh->Get_MeshName());

					if (ImGui::Selectable(szMeshName))
					{
						m_iMeshIndex = iMeshNoRenderIndex;
					}
					++iMeshNoRenderIndex;
				}
			}
			ImGui::EndListBox();
		}

		ImGui::TreePop();
	}

	_double iTickPerSecond =	m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();

	char szTickPerSecond[MAX_PATH];
	//to_string(iTickPerSecond);
	strcpy_s(szTickPerSecond,MAX_PATH, to_string(iTickPerSecond).c_str());

	ImGui::Text(szTickPerSecond);

	ImGui::InputFloat("TickPerSecone", &m_iTickPerSecond);
	
	m_Animations[m_iCurrentAnimIndex]->Set_TickPerSecond((_double)m_iTickPerSecond);


	//ImGui::InputInt("MeshIndex", &m_iMeshIndex);


	ImGui::End();

}

_vector CModel::GetModelCameraBone()
{
	if (nullptr == m_pCameraBone)
		return	_vector();

	_float4x4 m_CameraCOmbindMatrix;

	XMStoreFloat4x4(&m_CameraCOmbindMatrix, m_pCameraBone->Get_CombindMatrix());

	_vector vPos;
	memcpy(&vPos, &m_CameraCOmbindMatrix._41, sizeof(_vector));

	return  vPos;
}

void CModel::Set_AnimName(char * pAnimName )
{
	_uint iIndex = 0;

	for (auto& pAnim : m_Animations)
	{
		if (!strcmp(pAnim->Get_Name(), pAnimName))
		{
			Set_AnimIndex(iIndex);
			return;
		}
		++iIndex;
	}
	
}

void CModel::Set_SeaunceAnim(queue<_uint> iIndexs)
{
	while (!iIndexs.empty())
	{
		_uint iTemp = iIndexs.front();
		m_AnimIndexQueue.push(iTemp);
		iIndexs.pop();
	}

	m_iCurrentAnimIndex = m_AnimIndexQueue.front();
	m_AnimIndexQueue.pop();
	m_iNameArrayCurIndex = m_iCurrentAnimIndex;
}

HRESULT CModel::Ready_Bones(HANDLE hFile, CBone * pParent)
{
	DWORD   dwByte = 0;


	CBone* pBone = CBone::Create(this, hFile);
	if (nullptr == pBone)
		assert("CLoadModel_Ready_Bones");
	
	if(pParent!=nullptr)
		pBone->Set_Parent(pParent);
	m_Bones.push_back(pBone);

	_uint NumChildren = 0;
	ReadFile(hFile, &NumChildren, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < NumChildren; ++i)
	{
		Ready_Bones(hFile, pBone);

	}
		

	
	return S_OK;
}

HRESULT CModel::Ready_MeshContainers(HANDLE hFile, LOAD_TYPE eType)
{
	DWORD   dwByte = 0;
	ReadFile(hFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh *pMesh = CMesh::Create(m_pDevice, m_pContext, this, hFile,eType);

		if (nullptr == pMesh)
			assert("CLoadModel::Ready_MeshContainers");

		m_Meshes.push_back(pMesh);
	}


	return S_OK;
}

HRESULT CModel::Ready_Materials(HANDLE hFile, const char * pModelFilePath, LOAD_TYPE eType)
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
			_uint TextureFilePathLen = 0;
			ReadFile(hFile, &TextureFilePathLen, sizeof(_uint), &dwByte, nullptr);
			ReadFile(hFile, szFilePath,sizeof(_tchar)* TextureFilePathLen, &dwByte, nullptr);

			if (!lstrcmp(szFilePath, TEXT("")))
				assert("CLoadModel: Ready_Materials issue");

			ModelMaterial.pTexture[iTextureIndex] = CTexture::Create(m_pDevice, m_pContext ,szFilePath);
			if (nullptr == ModelMaterial.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animation(HANDLE hFile)
{
	DWORD   dwByte = 0;
	ReadFile(hFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(hFile, this);

		if (nullptr == pAnimation)
			assert("CLoadModel::Ready_Animation");

		m_Animations.push_back(pAnimation);
	}



	return S_OK;
}

HRESULT CModel::Ready_CameraBone()
{
	m_pCameraBone = Get_BonePtr("Node_Camera");
	
	if (nullptr != m_pCameraBone)
		Safe_AddRef(m_pCameraBone);
	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LOAD_TYPE eType, const char * pModelFilePath, _fmatrix PivotMatrix, HANDLE hFile)
{
	CModel*		pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, PivotMatrix, hFile)))
	{
		MSG_BOX("Failed to Created : CLoadModel");
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

	Safe_Release(m_pCameraBone);

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

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
}
