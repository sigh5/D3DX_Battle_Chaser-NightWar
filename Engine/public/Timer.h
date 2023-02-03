#pragma once
#pragma  once

#include "Base.h"
 
BEGIN(Engine)

class ENGINE_DLL CTimer final : public CBase
{
public:
	CTimer();
	virtual ~CTimer() =default;

public:
	_double		Get_TimeDelta() { return m_TimeDelta; }
	void		Set_TimeDelta(_double Time) { m_TimeSlowDelta = Time; }

	_double		Get_FixTimeDelta()const { return m_FixTimeDelta; }

public:
	HRESULT		Ready_Timer();
	void		Update_Timer();

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_double				m_TimeDelta = 0.0;
	_double				m_TimeSlowDelta = 1.0;
	_double				m_FixTimeDelta = 0.0;
public:
	static CTimer*	Create(void);
	virtual void Free(void);

};


END