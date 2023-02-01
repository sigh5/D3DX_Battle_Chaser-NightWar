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
	_float			Get_Ratio()const { return (_float)m_Ratio; }


public:
	HRESULT Initialize(HANDLE hFile, class CModel* pModel);
	void	Update_TransformMatrix(_double PlayTime);
	void	Blend_TransformMatrix(_double PlayTime, _float fBlendRatio);


	void Reset_KeyFrameIndex() { m_iCurrentKeyFrameIndex = 0;
	m_KeyFrames[0] = m_OldFrame;}

	


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
	_double				m_Ratio = 0.f;
public:
	static CChannel* Create(HANDLE hFile, class CModel* pModel);
	virtual void Free() override;

};

END
