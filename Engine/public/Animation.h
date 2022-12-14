#pragma once

#include "Base.h"
#include "Channel.h"
BEGIN(Engine)


class CAnimation :public CBase
{
public:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	const char* Get_Name() const { return m_szName; }
	void		Set_Duration(_double Ratio);
	_double		Get_Duration()const { return m_Duration; }
	
	 _uint		Get_Key_Frame();
	


public:
	HRESULT Initialize(HANDLE hFile, class CModel* pModel);
	void    Update_Bones(_double TimeDelta, _bool IsCombat);

public:
	void		Set_TickPerSecond(_double TickPerSecond) {m_TickPerSecond = TickPerSecond;}
	_double		Get_TickPerSecond()const { return m_TickPerSecond; }

	_bool		Get_Finished()const { return m_isFinished; }
	_bool		Get_IsLooping()const { return m_isLooping; }
	
	void		InitChannel();
	void		Set_Looping(_bool bLoopIng);
	void		Set_Finished(_bool bFinish);
	void		Set_PlayTime() { m_PlayTime = 0.0; }
	
	/* 선형보관*/
	void		InitLerp(vector<class CChannel*> Channels);
	vector<class CChannel*>&		Get_Channles() {return m_Channels;	}
private:
	char								m_szName[MAX_PATH];
	_double								m_Duration = 0.f;
	_double								m_Origin_Duration = 0.f;
	
	_double								m_TickPerSecond;
	_double								m_PlayTime = 0.0;

	_bool								m_isFinished = false;
	_bool								m_isLooping = false;
	_uint								m_iNumChannels = 0;
	
	vector<class CChannel*>				m_Channels;
	_uint								m_iJumpIndexNum = 0;
	

public:
	static CAnimation* Create(HANDLE hFile, class CModel* pModel);
	virtual void Free() override;


};

END