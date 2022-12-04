#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	CAnimation();
	virtual ~CAnimation() =default;

public:
	HRESULT	Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void	Update_Bones(_double TimeDelta);
private:
	char					m_szAnimName[MAX_PATH];

	/* �ִϸ��̼��� ����ϴµ� �ɸ��� ��ü�ð� �ø��ų��ٿ��� �ִϸ��̼��� ��������*/
	_double					m_Duration = 0.0;				

	/* �ʴ� ����ؾ��� �ִϸ��̼��� �ӵ��̴�. */
	_double					m_TickPerSecond ;			

	_double					m_PlayTime = 0.0;

	/* �� �ִϸ��̼��� ����ϱ����� �����ؾ��ϴ� ���� */
	_uint					m_iNumChannels = 0;
	vector<class CChannel*>	m_Channels;

public:
	static		CAnimation* Create(aiAnimation* pAIAnimation,class CModel* pModel);
	virtual		void		Free()override;


};

END