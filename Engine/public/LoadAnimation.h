#pragma once

#include "Base.h"

BEGIN(Engine)


class CLoadAnimation :public CBase
{
public:
	CLoadAnimation();
	virtual ~CLoadAnimation() = default;

public:
	HRESULT Initialize_Copy(class CLoadModel* pModel, CLoadAnimation* pOldAnimation);
	HRESULT Initialize(HANDLE hFile, class CLoadModel* pModel);
	void Update_Bones(_double TimeDelta);

private:
	char								m_szName[MAX_PATH];
	_double								m_Duration = 0.f;
	_double								m_TickPerSecond;
	_double								m_PlayTime = 0.0;

	_bool								m_isFinished = false;
	_bool								m_isLooping = false;
	_uint								m_iNumChannels = 0;
	vector<class CLoadChannel*>				m_Channels;

public:
	static CLoadAnimation* CreateCopy(class CLoadModel* pModel, CLoadAnimation* pOldAnimation);
	static CLoadAnimation* Create(HANDLE hFile, class CLoadModel* pModel);
	virtual void Free() override;


};

END