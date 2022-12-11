#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(HANDLE hFile, class CModel* pModel);
	void Update_TransformMatrix(_double PlayTime);
	
	void Set_CurrentFrameIndex(_uint iStartIndex) {m_iCurrentKeyFrameIndex = iStartIndex;}
	void Reset_KeyFrameIndex() { m_iCurrentKeyFrameIndex = 0; }


private:
	char			m_szName[MAX_PATH] = "";
	class CBone*	m_pBone = nullptr;

	_uint				m_iNumKeyframes = 0;
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = 0;

	KEYFRAME			m_OldFrame;
	
	

public:
	static CChannel* Create(HANDLE hFile, class CModel* pModel);
	virtual void Free() override;

};

END
