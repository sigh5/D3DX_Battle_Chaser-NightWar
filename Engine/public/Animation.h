#pragma once

#include "Base.h"

BEGIN(Engine)


class CAnimation :public CBase
{
public:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	const char* Get_Name() const { return m_szName; }

public:
	HRESULT Initialize(HANDLE hFile, class CModel* pModel);
	void    Update_Bones(_double TimeDelta);

	void		Set_TickPerSecond(_double TickPerSecond) {m_TickPerSecond = TickPerSecond;}
	_double		Get_TickPerSecond()const { return m_TickPerSecond; }

	_bool		Get_Finished()const { return m_isFinished; }
	_bool		Get_IsLooping()const { return m_isLooping; }
	void		Set_Looping(_bool bLoopIng) { m_isLooping = bLoopIng; }
	void		Set_Finished(_bool bFinish) {m_isFinished = bFinish;}
private:
	char								m_szName[MAX_PATH];
	_double								m_Duration = 0.f;
	_double								m_TickPerSecond;
	_double								m_PlayTime = 0.0;

	_bool								m_isFinished = false;
	_bool								m_isLooping = false;
	_uint								m_iNumChannels = 0;
	vector<class CChannel*>				m_Channels;

public:
	static CAnimation* Create(HANDLE hFile, class CModel* pModel);
	virtual void Free() override;


};

END