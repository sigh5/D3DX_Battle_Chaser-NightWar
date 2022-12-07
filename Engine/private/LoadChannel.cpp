#include "..\public\LoadChannel.h"

#include "LoadBone.h"
#include "LoadModel.h"
CLoadChannel::CLoadChannel()
{
}

HRESULT CLoadChannel::Initialize(HANDLE hFile, CLoadModel * pModel)
{
	DWORD   dwByte = 0;

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	
	char	pBoneName[MAX_PATH] = "";

	ReadFile(hFile, pBoneName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_iNumKeyframes, sizeof(_uint), &dwByte, nullptr);

	m_pBone = pModel->Get_BonePtr(pBoneName);
	Safe_AddRef(m_pBone);

	KEYFRAME keyFrame;
	ZeroMemory(&keyFrame, sizeof(KEYFRAME));

	for (_uint i = 0; i < m_iNumKeyframes; ++i)
	{
		ReadFile(hFile, &keyFrame.Time, sizeof(_double), &dwByte, nullptr);
		ReadFile(hFile, &keyFrame.vScale, sizeof(XMFLOAT3), &dwByte, nullptr);
		ReadFile(hFile, &keyFrame.vRotation, sizeof(XMFLOAT4), &dwByte, nullptr);
		ReadFile(hFile, &keyFrame.vPosition, sizeof(XMFLOAT3), &dwByte, nullptr);
		m_KeyFrames.push_back(keyFrame);
	}
	


	ReadFile(hFile, &m_iCurrentKeyFrameIndex, sizeof(_uint), &dwByte, nullptr);

	return S_OK;
}

HRESULT CLoadChannel::Initialize_Copy(CLoadChannel * pOldChannel, CLoadModel * pModel)
{
	strcpy_s(m_szName, MAX_PATH, pOldChannel->m_szName);

	char szBoneName[MAX_PATH] = "";
	Safe_Release(pOldChannel->m_pBone);
	
	strcpy_s(szBoneName, MAX_PATH, pOldChannel->m_pBone->Get_Name());
	m_pBone = pModel->Get_BonePtr(szBoneName);
	Safe_AddRef(m_pBone);
	m_iNumKeyframes = pOldChannel->m_iNumKeyframes;

	memcpy(&m_KeyFrames, &pOldChannel->m_KeyFrames, sizeof(m_KeyFrames));
	
	m_iCurrentKeyFrameIndex = pOldChannel->m_iCurrentKeyFrameIndex;

	return S_OK;
}

void CLoadChannel::Update_TransformMatrix(_double PlayTime)
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

CLoadChannel * CLoadChannel::Create_Copy(CLoadChannel * OldChannel, CLoadModel * pModel)
{
	CLoadChannel*		pInstance = new CLoadChannel();

	if (FAILED(pInstance->Initialize_Copy(OldChannel, pModel)))
	{
		MSG_BOX("Failed to Created : CLoadChannel_Copy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CLoadChannel * CLoadChannel::Create(HANDLE hFile, CLoadModel * pModel)
{
	CLoadChannel*		pInstance = new CLoadChannel();

	if (FAILED(pInstance->Initialize(hFile, pModel)))
	{
		MSG_BOX("Failed to Created : CLoadChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoadChannel::Free()
{
	Safe_Release(m_pBone);

	m_KeyFrames.clear();
}

