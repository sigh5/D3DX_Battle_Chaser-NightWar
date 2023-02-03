#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CSoundPlayer  :public CBase
{

	DECLARE_SINGLETON(CSoundPlayer);
public:


	typedef struct tag_SoundDesc
	{
		_int iFrame;
		_int iAnimIndex;
		_tchar pSoundTag[MAX_PATH] = L"";
		_int	iSoundChannel;
	}Anim_Model_SoundDesc;


private:
	CSoundPlayer();
	~CSoundPlayer() = default;

public:
	void	Tick(_double TimeDelta);

public:
	void	Add_SoundEffect_Model(class CModel* pModel, tag_SoundDesc& Desc);
	void	Add_SoundHit_Model(class CModel* pModel, tag_SoundDesc& Desc);
	void	Clear_allSound();
private:
	vector<pair<class CModel*, tag_SoundDesc >> Model_EffectSoundVec;
	vector<pair<class CModel*, tag_SoundDesc >> Model_HitSoundVec;


	

#ifdef _DEBUG
	void wc2c(const wchar_t* szWchar, char* szChar)
	{
		_int len;
		_int slength = lstrlen(szWchar) + 1;
		len = ::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, 0, 0, 0, 0);
		::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, szChar, len, 0, 0);
	}


	_bool					m_bSelected = false;
	_int					m_iChangeFrameIndex = 0;
	_int					m_iVectorIndex = 0;

#endif




public:
	virtual void	Free()override;



};
END
