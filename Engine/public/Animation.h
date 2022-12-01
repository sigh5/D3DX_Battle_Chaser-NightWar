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
	_double					m_Duration = 0.0;				// �ִϸ��̼� �Ѱ��� ����ð� �̰��� �ø��ų� �ٿ��� �ִϸ��̼��� �����ų� ������ �����.
	_double					m_TickPerSecond = 0.0;			// TickPerSecond * TimeDelta > duration  �Ѿִϸ��̼��� ������.

	/* �� �ִϸ��̼��� ����ϱ����� �����ؾ��ϴ� ���� */
	_uint					m_iNumBones = 0;
	vector<class CBone*>	m_Bones;

public:
	static		CAnimation* Create(aiAnimation* pAIAnimation);
	virtual		void		Free()override;


};

END