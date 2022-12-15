#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const char*		GetName()const { return m_szName; }
	void			Set_DurationTime(_double Ratio);
public:
	HRESULT Initialize(HANDLE hFile, class CModel* pModel);
	void Update_TransformMatrix(_double PlayTime);
	
	void Reset_KeyFrameIndex() { m_iCurrentKeyFrameIndex = 0;
	m_KeyFrames[0] = m_OldFrame;}

	void BlendingFrame(CChannel* pPrevChannel,_double TimeDelta, _bool IsLerp);



	const _uint Get_CurrentKeyFrameIndex()const { return m_iCurrentKeyFrameIndex; }

private:
	char			m_szName[MAX_PATH] = "";
	class CBone*	m_pBone = nullptr;

	_uint				m_iNumKeyframes = 0;
	vector<KEYFRAME>	m_KeyFrames;
	
	vector<_double>		m_Origin_TimeVec;



	_uint				m_iCurrentKeyFrameIndex = 0;
	


	KEYFRAME			m_OldFrame;
	_double				m_FixTime = 0.5;
	_double				m_newFixTime = 0.0;

public:
	static CChannel* Create(HANDLE hFile, class CModel* pModel);
	virtual void Free() override;

};

END
