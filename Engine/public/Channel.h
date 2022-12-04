#pragma once

#include "Base.h"

BEGIN(Engine)

/* 애니메이션 재생할 떄 필요한 뼈*/
/* 뼈들의 상태를 갱신해야한다. */
/* 
	애니메이션에서 정점의 정보를 저장하는것이 아니라 
	뼈의 상태를 저장한다.			// 애니메이션에 영향을 받는 하나의뼈
	하나의 Key -> keyFrame -> 특정 시간대의 상태를 말하는 것이다.
*/


class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;
public:
	HRESULT				Initialize(aiNodeAnim* pAINodeAnim, class CModel* pModel);
	void				Update_TransformMatrix(_double PlayTime); /* 뼈들의 상태를 갱신해야한다. */

private:
	char				m_szChannelName[MAX_PATH] = "";
	class	CBone*		m_pBone = nullptr;

	_uint				m_iNumKeyFrame = 0; // 한애니메이션을 실행할때 뼈의 keyFrame의 총 개수
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = 0;

public:
	static		CChannel* Create(aiNodeAnim* pAINodeAnim, class CModel* pModel);
	virtual		void	Free()override;

};

END