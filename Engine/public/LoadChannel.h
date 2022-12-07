#pragma once
#include "Base.h"

BEGIN(Engine)

class CLoadChannel final : public CBase
{
public:
	CLoadChannel();
	virtual ~CLoadChannel() = default;

public:
	HRESULT Initialize(HANDLE hFile, class CLoadModel* pModel);
	HRESULT Initialize_Copy(CLoadChannel* OldChannel, class CLoadModel* pModel);
	void Update_TransformMatrix(_double PlayTime);
	void Reset_KeyFrameIndex() {m_iCurrentKeyFrameIndex = 0;}

private:
	char			m_szName[MAX_PATH] = "";
	class CLoadBone*	m_pBone = nullptr;

	_uint				m_iNumKeyframes = 0;
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = 0;

public:
	static CLoadChannel* Create_Copy(CLoadChannel* OldChannel, class CLoadModel* pModel);
	static CLoadChannel* Create(HANDLE hFile, class CLoadModel* pModel);
	virtual void Free() override;

};

END
