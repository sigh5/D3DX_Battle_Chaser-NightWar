#pragma once
#include "Base.h"

BEGIN(Engine)

/* 
	���� ������ �ִ� ���� 
	���� �̸�, �������, ��������, + ��ɵ�
	�������� -> �θ������ ���� ��

*/
class CBone final :public CBase
{
public:
	CBone();
	virtual ~CBone() = default;

public:
	const			char* Get_BoneName() const { return szBoneName; }
	void			Set_offSetMatrix(_float4x4 offSetMatrix) { m_OffsetMatrix = offSetMatrix; }

public:
	HRESULT			Initialize(aiNode* pAiNode);
	void			Compute_CombindTransformationMatrix();

private:
	char			szBoneName[MAX_PATH];
	_float4x4		m_OffsetMatrix;		// �ѹ� �о���� ���� �Ұ�( �������� ����??)
	_float4x4		m_TransformMatrix;	// �θ������ ��ȯ���
	_float4x4		m_CombindTransformMatrix;	// �� �θ��� �θ� .... ���� ���س�����
	CBone*			m_pParent = nullptr;

public:
	static  CBone*	Create(aiNode* pAiNode);	//aiNode == ���� ��Ÿ���� Ŭ���� 3������
	virtual void	Free()override;


};

END
