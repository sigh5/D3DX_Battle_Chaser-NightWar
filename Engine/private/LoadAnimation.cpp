#include "..\public\LoadAnimation.h"
#include "LoadChannel.h"


CLoadAnimation::CLoadAnimation()
	: m_isLooping(true)
{
}

HRESULT CLoadAnimation::Initialize_Copy(CLoadModel * pModel, CLoadAnimation * pOldAnimation)
{
	strcpy_s(m_szName, MAX_PATH, pOldAnimation->m_szName);
	m_Duration = pOldAnimation->m_Duration;
	m_TickPerSecond = pOldAnimation->m_TickPerSecond;
	m_isFinished = pOldAnimation->m_isFinished;
	m_isLooping = pOldAnimation->m_isLooping;
	m_iNumChannels = pOldAnimation->m_iNumChannels;


	for (auto& pOldChannel : pOldAnimation->m_Channels)
	{
		CLoadChannel* pNewChannel = CLoadChannel::Create_Copy(pOldChannel, pModel);
		if (nullptr == pNewChannel)
			assert("CLoadAnimation::Initialize_Copy");

		m_Channels.push_back(pNewChannel);
	}

	


	return S_OK;
}

HRESULT CLoadAnimation::Initialize(HANDLE hFile, CLoadModel * pModel)
{
	DWORD   dwByte = 0;

	ReadFile(hFile, m_szName, MAX_PATH, &dwByte, nullptr);
	ReadFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr);
	ReadFile(hFile, &m_isFinished, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_isLooping, sizeof(_bool), &dwByte, nullptr);
	ReadFile(hFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CLoadChannel* pChannel = CLoadChannel::Create(hFile, pModel);
		if (nullptr == pChannel)
			assert("CLoadAnimation::Initialize");

		m_Channels.push_back(pChannel);
	}
	return S_OK;
}

void CLoadAnimation::Update_Bones(_double TimeDelta)
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

CLoadAnimation * CLoadAnimation::CreateCopy(CLoadModel * pModel, CLoadAnimation * pOldAnimation)
{
	CLoadAnimation*		pInstance = new CLoadAnimation();

	if (FAILED(pInstance->Initialize_Copy(pModel, pOldAnimation)))
	{
		MSG_BOX("Failed to Created : CLoadAnimation_CreateCopy");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CLoadAnimation * CLoadAnimation::Create(HANDLE hFile, CLoadModel * pModel)
{
	CLoadAnimation*		pInstance = new CLoadAnimation();

	if (FAILED(pInstance->Initialize(hFile, pModel)))
	{
		MSG_BOX("Failed to Created : CLoadAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoadAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}