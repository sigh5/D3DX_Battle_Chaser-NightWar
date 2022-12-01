#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	CAnimation();
	virtual ~CAnimation() =default;

public:
	HRESULT	Initialize(aiAnimation* pAIAnimation);

private:
	char					m_szAnimName[MAX_PATH];
	_double					m_Duration = 0.0;				// 애니메이션 한개의 재생시간 이것을 늘리거나 줄여서 애니메이션을 빠르거나 느리게 만든다.
	_double					m_TickPerSecond = 0.0;			// TickPerSecond * TimeDelta > duration  한애니메이션이 끝났다.

	/* 이 애니메이션을 재생하기위해 갱신해야하는 뼈들 */
	_uint					m_iNumBones = 0;
	vector<class CBone*>	m_Bones;

public:
	static		CAnimation* Create(aiAnimation* pAIAnimation);
	virtual		void		Free()override;


};

END