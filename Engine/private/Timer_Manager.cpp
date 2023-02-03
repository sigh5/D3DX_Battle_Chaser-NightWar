#include "..\public\Timer_Manager.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}


_double CTimer_Manager::Get_TimeDelta(const _tchar * pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return 0.0;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::Set_Timedelta(const _tchar * pTimerTag, _double Time)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);
	assert(nullptr != pTimer  && "CTimer_Manager::Set_Timedelta");
		
	pTimer->Set_TimeDelta(Time);


}

_double CTimer_Manager::Get_FixTimeDelta(const _tchar * pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return 0.0;

	return pTimer->Get_FixTimeDelta();
}

HRESULT CTimer_Manager::Ready_Timer(const _tchar * pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}

void CTimer_Manager::Update_Timer(const _tchar * pTimerTag)
{
	CTimer*	pTimer = Find_Timer(pTimerTag);

	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

CTimer * CTimer_Manager::Find_Timer(const _tchar * pTimerTag)
{
	auto		iter = find_if(m_mapTimers.begin(), m_mapTimers.end(), CTag_Finder(pTimerTag));

	if (iter == m_mapTimers.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free(void)
{
	for (auto& Pair : m_mapTimers)
		Safe_Release(Pair.second);
	m_mapTimers.clear();
}
