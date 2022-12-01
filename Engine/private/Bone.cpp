#include "..\public\Bone.h"


CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAiNode)
{
	strcpy_s(szBoneName, pAiNode->mName.data); // �� data ������Ѵ�.

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	memcpy(&m_TransformMatrix, &pAiNode->mTransformation, sizeof(_float4x4));
	// ���ũ�� �ݹ�������̴� .
	// �츮������Ŵ� �ο���� ����̴ϱ� ��ġ������Ѵ�.

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)));

	XMStoreFloat4x4(&m_CombindTransformMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Compute_CombindTransformationMatrix()
{
	if (nullptr == m_pParent)
		m_CombindTransformMatrix = m_TransformMatrix;
	else
		XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix)*XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));
}

CBone * CBone::Create(aiNode * pAiNode)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAiNode)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
}
