#include "..\public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
	: m_isLooping(true)
{
}

HRESULT CAnimation::Initialize(aiAnimation * pAIAnimation, CModel* pModel)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_Duration = pAIAnimation->mDuration;

	m_TickPerSecond = pAIAnimation->mTicksPerSecond;

	/* �� �ִϸ��̼� �����ϴµ� �ʿ��� ������ ������.  */
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		aiNodeAnim*		pAINodeAnim = pAIAnimation->mChannels[i];

		CChannel*		pChannel = CChannel::Create(pAINodeAnim, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Update_Bones(_double TimeDelta)
{
	if (true == m_isFinished &&
		false == m_isLooping)
	{
		return;
	}

	m_PlayTime += m_TickPerSecond * TimeDelta;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		m_Channels[i]->Update_TransformMatrix(m_PlayTime);
	}

	if (true == m_isFinished)
		m_isFinished = false;

}

void CAnimation::Save_AnimationData(HANDLE hFile)
{
	DWORD   dwByte = 0;

	WriteFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	WriteFile(hFile,&m_Duration,sizeof(_double), &dwByte, nullptr);
	WriteFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);
	WriteFile(hFile, &m_isFinished, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_isLooping, sizeof(_bool), &dwByte, nullptr);
	WriteFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	for (auto &pChannel : m_Channels)
	{
		pChannel->Save_ChannelData(hFile);
	}
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation, CModel* pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}

