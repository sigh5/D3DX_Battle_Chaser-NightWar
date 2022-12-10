#include "..\public\Bone.h"

#include "Model.h"

CBone::CBone()
{
}

CBone::CBone(const CBone & rhs)
	: m_CombindTransformMatrix(rhs.m_CombindTransformMatrix)
	, m_OffsetMatrix(rhs.m_OffsetMatrix)
	, m_TransformMatrix(rhs.m_TransformMatrix)

{
	strcpy_s(m_szName, MAX_PATH, rhs.m_szName);
	
}

HRESULT CBone::Initialize(CModel* pModel, HANDLE hFile)
{
	DWORD   dwByte = 0;
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_CombindTransformMatrix, XMMatrixIdentity());

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

	
	return S_OK;
}

void CBone::Compute_CombindTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombindTransformMatrix = m_TransformMatrix;

	else
		XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) * XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));
}

CBone * CBone::Create(CModel* pModel, HANDLE hFile)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pModel, hFile)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CBone::Free()
{
	Safe_Release(m_pParent);
}


