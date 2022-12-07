#include "..\public\LoadBone.h"

#include "LoadModel.h"

CLoadBone::CLoadBone()
{
}

HRESULT CLoadBone::Initialize(CLoadModel* pModel,HANDLE hFile)
{
	DWORD   dwByte = 0;

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_OffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
	ReadFile(hFile, &m_TransformMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
	ReadFile(hFile, &m_CombindTransformMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);

	char szParentName[MAX_PATH] = "";
	ReadFile(hFile, szParentName, MAX_PATH, &dwByte, nullptr);

	if (!strcmp(szParentName, ("nullptr")))
	{
		m_pParent = nullptr;
	}
	else
	{
		m_pParent = pModel->Get_BonePtr(szParentName);
	}

	return S_OK;
}

HRESULT CLoadBone::Initialize_DeepCopy(class CLoadModel* pModel,CLoadBone * pOriginBone)
{
	strcpy_s(m_szName,MAX_PATH, pOriginBone->m_szName);
	m_OffsetMatrix = pOriginBone->m_OffsetMatrix;
	m_TransformMatrix = pOriginBone->m_TransformMatrix;
	m_CombindTransformMatrix = pOriginBone->m_CombindTransformMatrix;

	if (nullptr == pOriginBone->m_pParent)
		m_pParent = nullptr;
	else
	{
		char szParentName[MAX_PATH] = "";
		strcpy_s(szParentName, MAX_PATH, pOriginBone->m_pParent->m_szName);
		m_pParent = pModel->Get_BonePtr(szParentName);

		if (nullptr == m_pParent)
			assert("CLoadBone::Initialize_DeepCopy");
	}

	return S_OK;
}

void CLoadBone::Compute_CombindTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombindTransformMatrix = m_TransformMatrix;

	else
		XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) * XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));
}

CLoadBone * CLoadBone::Create(CLoadModel* pModel,HANDLE hFile)
{
	CLoadBone*		pInstance = new CLoadBone();

	if (FAILED(pInstance->Initialize(pModel,hFile)))
	{
		MSG_BOX("Failed to Created : CLoadBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CLoadBone * CLoadBone::CreateCopy(class CLoadModel* pModel,CLoadBone * pOriginBone)
{
	CLoadBone*		pInstance = new CLoadBone();

	if (FAILED(pInstance->Initialize_DeepCopy(pModel,pOriginBone)))
	{
		MSG_BOX("Failed to Created : CLoadBone_CreateCopy");
		Safe_Release(pInstance);
	}
	return nullptr;
}

void CLoadBone::Free()
{
	Safe_Release(m_pParent);
}


