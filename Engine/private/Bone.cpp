#include "..\public\Bone.h"



CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode, CBone* pParent)
{
	strcpy_s(m_szName, pAINode->mName.data);

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	memcpy(&m_TransformMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)));

	XMStoreFloat4x4(&m_CombindTransformMatrix, XMMatrixIdentity());

	m_pParent = pParent;

	Safe_AddRef(m_pParent);

	return S_OK;
}

void CBone::Compute_CombindTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombindTransformMatrix = m_TransformMatrix;

	else
		XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) * XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));

}

void CBone::Save_BoneData(HANDLE hFile)
{
	DWORD   dwByte = 0;
	
	WriteFile(hFile, &m_szName, MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile, &m_OffsetMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
	WriteFile(hFile, &m_TransformMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
	WriteFile(hFile, &m_CombindTransformMatrix, sizeof(XMFLOAT4X4), &dwByte, nullptr);
	
	if(m_pParent != nullptr)
		WriteFile(hFile, m_pParent->m_szName, MAX_PATH, &dwByte, nullptr);
	else
	{
		char  szNullParentName[MAX_PATH] = "nullptr";
		WriteFile(hFile, szNullParentName, MAX_PATH, &dwByte, nullptr);
	}
}

CBone * CBone::Create(aiNode * pAINode, CBone* pParent)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, pParent)))
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
