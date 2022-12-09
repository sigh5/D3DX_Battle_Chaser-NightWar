#pragma once

#include "Base.h"

BEGIN(Engine)


class CAnimation :public CBase
{
public:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(HANDLE hFile, class CModel* pModel);
	void    Update_Bones(_double TimeDelta);

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