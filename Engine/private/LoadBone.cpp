#include "..\public\LoadBone.h"

#include "LoadModel.h"

CLoadBone::CLoadBone()
{
}

CLoadBone::CLoadBone(const CLoadBone & rhs)
	: m_CombindTransformMatrix(rhs.m_CombindTransformMatrix)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_TransformMatrix(rhs.m_TransformMatrix)
	
{
	strcpy_s(m_szName,MAX_PATH,rhs.m_szName);
	strcpy_s(m_szParentName, MAX_PATH, rhs.m_szParentName);
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
		strcpy_s(m_szParentName, MAX_PATH, ("nullptr"));
	}
	else
	{
		m_pParent = pModel->Get_BonePtr(szParentName);
		if (m_pParent) assert("CLoadBone::Initialize");
		Safe_AddRef(m_pParent);
		strcpy_s(m_szParentName, MAX_PATH, szParentName);

	}

	return S_OK;
}

HRESULT CLoadBone::Initialize_DeepCopy(void *pArg)
{
	CLoadModel* pModel = reinterpret_cast<CLoadModel*>(pArg);
	m_pParent = pModel->Get_BonePtr(m_szParentName);
	
	if (nullptr != m_pParent)
		Safe_AddRef(m_pParent);

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

CLoadBone * CLoadBone::CreateClone(void *pArg )
{
	CLoadBone*		pInstance = new CLoadBone(*this);

	if (FAILED(pInstance->Initialize_DeepCopy(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadBone::Free()
{
	Safe_Release(m_pParent);
}


