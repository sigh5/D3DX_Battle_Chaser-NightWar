#include "..\public\Animation.h"
#include "Channel.h"


CAnimation::CAnimation()
	: m_isLooping(true)
{
}


HRESULT CAnimation::Initialize(HANDLE hFile, CModel * pModel)
{
	DWORD   dwByte = 0;

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_isLooping, sizeof(_bool), &dwByte, nullptr);
	
	ReadFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);
	

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(hFile, pModel);
		if (nullptr == pChannel)
			assert("CAnimation::Initialize");

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
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		m_Channels[i]->Update_TransformMatrix(m_PlayTime);
	}
}

void CAnimation::InitChannel()
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Reset_KeyFrameIndex();
		m_Channels[i]->Update_TransformMatrix(0.0);
	}
}

void CAnimation::Set_Looping(_bool bLoopIng)
{
	 m_isLooping = true;  
	 m_isFinished = false; 
}

void CAnimation::Set_Finished(_bool bFinish)
{
	m_isFinished = bFinish;
	m_PlayTime = 0.0;
}

CAnimation * CAnimation::Create(HANDLE hFile, CModel * pModel)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(hFile, pModel)))
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