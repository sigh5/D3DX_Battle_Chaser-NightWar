#include "..\public\Channel.h"

#include "Bone.h"
#include "Model.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(HANDLE hFile, CModel * pModel)
{
	DWORD   dwByte = 0;

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_iNumKeyframes, sizeof(_uint), &dwByte, nullptr);

	m_pBone =		pModel->Get_BonePtr(m_szName);
	if (m_pBone != nullptr)
		Safe_AddRef(m_pBone);

	KEYFRAME keyFrame;
	ZeroMemory(&keyFrame, sizeof(KEYFRAME));

	for (_uint i = 0; i < m_iNumKeyframes; ++i)
	{
		ReadFile(hFile, &keyFrame, sizeof(KEYFRAME), &dwByte, nullptr);
		m_KeyFrames.push_back(keyFrame);
	}

	return S_OK;
}



void CChannel::Update_TransformMatrix(_double PlayTime)
{
	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_matrix			TransformMatrix;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지며.ㄴ */
	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_TransformMatrix(TransformMatrix);
}


CChannel * CChannel::Create(HANDLE hFile, CModel * pModel)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(hFile, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	Safe_Release(m_pBone);

	m_KeyFrames.clear();
}

