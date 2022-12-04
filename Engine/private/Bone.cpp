#include "..\public\Bone.h"



CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode)
{
	strcpy_s(m_szBoneName, pAINode->mName.data);

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	memcpy(&m_TransformMatrix, &pAINode->mTransformation, sizeof(_float4x4));

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)));

	XMStoreFloat4x4(&m_CombindTransformMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Compute_CombindTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombindTransformMatrix = m_TransformMatrix;

	else
		XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) * XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));

}

CBone * CBone::Create(aiNode * pAINode)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{

}
