#pragma once

#include "Base.h"

BEGIN(Engine)

/* �ִϸ��̼� ����� �� �ʿ��� ��*/
/* ������ ���¸� �����ؾ��Ѵ�. */
/* 
	�ִϸ��̼ǿ��� ������ ������ �����ϴ°��� �ƴ϶� 
	���� ���¸� �����Ѵ�.			// �ִϸ��̼ǿ� ������ �޴� �ϳ��ǻ�
	�ϳ��� Key -> keyFrame -> Ư�� �ð����� ���¸� ���ϴ� ���̴�.
*/


class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;
public:
	HRESULT				Initialize(aiNodeAnim* pAINodeAnim, class CModel* pModel);
	void				Update_TransformMatrix(_double PlayTime); /* ������ ���¸� �����ؾ��Ѵ�. */

private:
	char				m_szChannelName[MAX_PATH] = "";
	class	CBone*		m_pBone = nullptr;

	_uint				m_iNumKeyFrame = 0; // �Ѿִϸ��̼��� �����Ҷ� ���� keyFrame�� �� ����
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = 0;

public:
	static		CChannel* Create(aiNodeAnim* pAINodeAnim, class CModel* pModel);
	virtual		void	Free()override;

};

END